//
// Created by daniel on 21/08/16.
//

#include <zconf.h>
#include <math.h>
#include "Game.h"


#include "serialization.h"
#include "CommandCode.h"
#include "command.h"
#include "SFUtils.h"
int16_t Pickup::nextPickupId;
Game::Game(sf::RenderWindow&  window, ServerSocket * sSocket, int selectedTeam):
window(window),
sSocket(sSocket),
selectedTeam(selectedTeam),
world(World(window,&players, &projectiles))
{
    pthread_mutex_init(&commandQueueMutex, NULL);
    pthread_mutex_init(&projectileACKMutex, NULL);
    Pickup::nextPickupId = 0;
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timeSinceLastProjectileUpdate = sf::Time::Zero;


    window.setVerticalSyncEnabled(true);
    bool should_render = true;
    while (!gameOver)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
            }
        }
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        timeSinceLastProjectileUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processServerEvents();
            processEvents();
            sendCommands();
            should_render = true;
            world.update(TimePerFrame);

        }

        while (timeSinceLastProjectileUpdate > TimePerProjectileUpdate)
        {
            timeSinceLastProjectileUpdate -= TimePerProjectileUpdate;
            world.updateProjectiles(TimePerProjectileUpdate);
        }

        //updateStatistics(elapsedTime);
        if (should_render)
        {
            render();
            should_render = false;
        }

    }
    restart = world.gameOver(gameOverCode);
}

void Game::sendCommands()
{
    if (playerID == -1)
    {
        requestJoinGame();
        return;
    }


    int16_t offset = 0;
    Serialization::shortToChars(c_input_command,buffer,offset);
    offset+=2;
    Serialization::shortToChars(playerID,buffer,offset);
    offset+=2;
    Serialization::intToChars(msgnm,buffer,offset);
    offset+=4;
    Serialization::intToChars(moves,buffer,offset);
    offset+=4;
    Serialization::floatToChars(rotation,buffer,offset);
    offset+=4;
    pthread_mutex_lock(&projectileACKMutex);
    Serialization::intToChars((int)projectileACKQueue.size(), buffer, offset);
    offset+=4;

    while(!projectileACKQueue.empty())
    {
        Serialization::shortToChars(s_projectile_command,buffer,offset);
        offset+=2;
        int32_t ack = projectileACKQueue.front();
        projectileACKQueue.pop();
        Serialization::intToChars(ack,buffer,offset);
        offset+=4;
    }
    pthread_mutex_unlock(&projectileACKMutex);
    Serialization::shortToChars(-1,buffer,offset);

    sSocket->send(buffer,COMMAND_BUFFER_SIZE);
    msgnm++;
}

void Game::processEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        // Request for closing the window
        if (event.type == sf::Event::Closed)
            window.close();
    }

    moves = 0x0000;
    if (window.hasFocus()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            moves |= 0x1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            moves |= 0x2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            moves |= 0x4;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            moves |= 0x8;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            moves |= 0x10;

        sf::Vector2f mousePositionFloat = window.mapPixelToCoords((sf::Mouse::getPosition(window)));
        sf::Vector2f spriteCenter = getSpriteCenter(players[playerID].sprite);
        sf::Vector2f facing = mousePositionFloat - spriteCenter;
        rotation = atan2f(facing.y, facing.x);
    }

}

void Game::processServerEvents()
{
    pthread_mutex_lock(&commandQueueMutex);
    while(!commandQueue.empty())
    {
        command_t command = commandQueue.front();
        commandQueue.pop();
        if(command.type == c_input_command)
        {

            if(!world.findPlayer(command.playerID,&players))
            {
                char * nick = (char *)calloc(strlen(command.nickname)+1, sizeof(char));
                printf("%s esto recibo pue\n",command.nickname);
                strcpy(nick,command.nickname);
                players.push_back(Player(command.playerID, command.team, command.posx, command.posy, world.textureHolder.get(command.team == 0 ? Textures::PLAYER_RED : Textures::PLAYER_GREEN) , nick));
            }
            else
            {
                if(command.team == 0)
                    players[command.playerID].setTexture( world.textureHolder.get(Textures::PLAYER_RED));
                else
                    players[command.playerID].setTexture( world.textureHolder.get(Textures::PLAYER_GREEN));
            }

            if (players[command.playerID].ammo < command.ammo)
                world.sfxReload.play();

            players[command.playerID].health = command.health;
            players[command.playerID].valid = command.validPlayer;
            players[command.playerID].boundingBox.left = command.posx;
            players[command.playerID].boundingBox.top = command.posy;
            players[command.playerID].rotation = command.rotation;
            players[command.playerID].ammo = command.ammo;
        }
        else if (command.type == s_projectile_command)
        {
            sf::Vector2f position = sf::Vector2f(command.posx,command.posy);
            sf::Vector2f origin = sf::Vector2f(command.originx,command.originy);
            Projectile projectile = Projectile(command.bulletID,command.bulletType,position,origin, command.playerID);
            projectile.sprite.setTexture(world.textureHolder.get(Textures::BULLET_SMALL));
            world.sfxShotGun.play();
            projectiles.insert(std::pair<int16_t , Projectile>(command.bulletID, projectile));
        }
        else if (command.type == s_player_id_command)
        {
            playerID = command.playerID;
        }
        else if (command.type == s_game_over)
        {
            gameOver = true;
            gameOverCode = command.team;
        }
        else if (command.type == s_pickups)
        {
            world.all_pickups[command.pickupId]->enabled = command.pickupEnabled;
        }
    }
    pthread_mutex_unlock(&commandQueueMutex);
}


void Game::receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr)
{
    if (nBytes <= 0 || gameOver)
        return;

    int32_t  msgNum;
    short offset = 0;
    Serialization::charsToInt(buffer,msgNum,offset);
    int16_t command_type;
    offset += 4;
    Serialization::charsToShort(buffer, command_type, offset);
    offset += 2;
    bool eom = false;

    if(command_type == s_players_command) {
        if(msgNum > lastServerMsgid)
        {
            do {
                command srvCmd;
                srvCmd.msgNum = msgNum;
                srvCmd.type = command_type;
                int16_t playerInfo = 0;
                Serialization::charsToShort(buffer, playerInfo, offset);

                if (playerInfo != -1)
                {
                    srvCmd.playerID = playerInfo & 0x00FF;
                    srvCmd.team = playerInfo & 0x0100;
                    srvCmd.validPlayer = playerInfo & 0x0200;
                    offset += 2;
                    Serialization::charsToFloat(buffer, srvCmd.posx, offset);
                    offset += 4;
                    Serialization::charsToFloat(buffer, srvCmd.posy, offset);
                    offset += 4;
                    Serialization::charsToFloat(buffer, srvCmd.rotation, offset);
                    offset += 4;
                    Serialization::charsToShort(buffer, srvCmd.health, offset);
                    offset += 2;

                    memset(srvCmd.nickname, 0, 6);
                    strcpy(srvCmd.nickname, buffer + offset);
                    offset += strlen(srvCmd.nickname) + 1;

                    memcpy(&srvCmd.ammo, buffer + offset, 1);
                    offset++;

                    pthread_mutex_lock(&commandQueueMutex);
                    commandQueue.push(srvCmd);
                    pthread_mutex_unlock(&commandQueueMutex);
                }
                else
                {
                    eom = true;
                }
            } while (!eom);
        }
    }
    else if(command_type == s_projectile_command)
    {
        do
        {
            command srvCmd;
            srvCmd.msgNum = msgNum;
            srvCmd.type = command_type;
            Serialization::charsToShort(buffer, srvCmd.bulletID, offset);
            if (srvCmd.bulletID != -1)
            {
                offset += 2;
                Serialization::charsToShort(buffer, srvCmd.bulletType, offset);
                offset += 2;
                Serialization::charsToFloat(buffer, srvCmd.posx, offset);
                offset += 4;
                Serialization::charsToFloat(buffer, srvCmd.posy, offset);
                offset += 4;
                Serialization::charsToFloat(buffer, srvCmd.originx, offset);
                offset += 4;
                Serialization::charsToFloat(buffer, srvCmd.originy, offset);
                offset += 4;
                Serialization::charsToShort(buffer, srvCmd.playerID, offset);
                offset += 2;

                pthread_mutex_lock(&commandQueueMutex);
                if(projectiles.count(srvCmd.bulletID) == 0)
                {
                    commandQueue.push(srvCmd);
                }
                pthread_mutex_unlock(&commandQueueMutex);
            }
            else {
                eom = true;
            }

        } while (!eom);
        pthread_mutex_lock(&projectileACKMutex);
        projectileACKQueue.push(msgNum);
        pthread_mutex_unlock(&projectileACKMutex);
    }
    else if (command_type == s_player_id_command)
    {
        command srvCmd;
        srvCmd.msgNum = msgNum;
        srvCmd.type = command_type;
        Serialization::charsToShort(buffer, srvCmd.playerID, offset);
        if(playerID == -1)
            playerID = srvCmd.playerID;
    }
    else if (command_type == s_game_over)
    {
        command srvCmd;
        srvCmd.msgNum = msgNum;
        srvCmd.type = command_type;
        Serialization::charsToShort(buffer, srvCmd.team, offset);
        pthread_mutex_lock(&commandQueueMutex);
        commandQueue.push(srvCmd);
        pthread_mutex_unlock(&commandQueueMutex);
    }
    else if (command_type == s_pickups)
    {
        if(msgNum > lastServerMsgid)
        {
            do
            {
                command srvCmd;
                srvCmd.msgNum = msgNum;
                srvCmd.type = command_type;
                int16_t pickupId = 0;
                Serialization::charsToShort(buffer, pickupId, offset);

                if (pickupId != -1)
                {
                    srvCmd.pickupId = pickupId;
                    offset += 2;
                    srvCmd.pickupEnabled = buffer[offset] != 0;
                    offset++;
                    pthread_mutex_lock(&commandQueueMutex);
                    commandQueue.push(srvCmd);
                    pthread_mutex_unlock(&commandQueueMutex);
                }
                else
                {
                    break;
                }
            } while (true);
        }
    }
}

void Game::requestJoinGame()
{
    int16_t offset = 0;
    Serialization::shortToChars(c_join_game_command,buffer,offset);
    offset += 2;
    Serialization::shortToChars(selectedTeam,buffer,offset);
    offset += 2;
    Serialization::shortToChars(-1,buffer,offset);
    sSocket->send(buffer,COMMAND_BUFFER_SIZE);
}

void Game::getID()
{
    while(playerID == -1)
    {
        requestJoinGame();
        sleep(1);
    }
    world.playerID = playerID;
}

void Game::render()
{
    window.clear();
    world.render();
    window.display();
}