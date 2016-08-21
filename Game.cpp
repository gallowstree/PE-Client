//
// Created by daniel on 21/08/16.
//

#include <zconf.h>
#include "Game.h"


#include "serialization.h"
#include "CommandCode.h"
#include "command.h"
#include "SFUtils.h"

Game::Game(sf::RenderWindow&  window,ServerSocket * sSocket, int selectedTeam):
window(window),
sSocket(sSocket),
selectedTeam(selectedTeam),
world(World(window,&players))
{
    pthread_mutex_init(&commandQueueMutex, NULL);
    pthread_mutex_init(&projectileACKMutex, NULL);
}

void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    sf::Time timeSinceLastProjectileUpdate = sf::Time::Zero;


    window.setVerticalSyncEnabled(true);
    bool should_render = true;

    while (window.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        timeSinceLastProjectileUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processServerEvents();
            processEvents();

            //if (window.hasFocus())
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
}

void Game::sendCommands()
{
    if (playerID == -1)
    {
        requestJoinGame();
        return;
    }


    int16_t offset = 0;
    Serialization::shortToChars(CommandCode::c_input_command,buffer,offset);
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
        Serialization::shortToChars(CommandCode::s_projectile_command,buffer,offset);
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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        moves |= 0x1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        moves |= 0x2;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        moves |= 0x4;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        moves |= 0x8;
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        moves |= 0x10;

    sf::Vector2f mousePositionFloat = window.mapPixelToCoords((sf::Mouse::getPosition(window)));
    sf::Vector2f spriteCenter = getSpriteCenter(players[playerID].sprite);
    sf::Vector2f facing = mousePositionFloat - spriteCenter;
    rotation = atan2f(facing.y, facing.x);

}

void Game::processServerEvents()
{
    pthread_mutex_lock(&commandQueueMutex);
    while(!commandQueue.empty())
    {
        command_t command = commandQueue.front();
        commandQueue.pop();
        if(command.type == CommandCode::c_input_command)
        {
            players[command.playerID].boundingBox.left = command.posx;
            players[command.playerID].boundingBox.top = command.posy;
            players[command.playerID].rotation = command.rotation;
        }
        else if (command.type == CommandCode::s_projectile_command)
        {
            sf::Vector2f position = sf::Vector2f(command.posx,command.posy);
            sf::Vector2f origin = sf::Vector2f(command.originx,command.originy);
            Projectile projectile = Projectile(command.bulletID,command.bulletType,position,origin, command.playerID);
            projectiles.insert(std::pair<int16_t , Projectile>(command.bulletID, projectile));
        }
        else if (command.type == CommandCode::s_player_id_command)
        {
            printf("entrando");
            playerID = command.playerID;
        }
    }
    pthread_mutex_unlock(&commandQueueMutex);
}


void Game::receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr)
{
    int32_t  msgNum;
    short offset = 0;
    Serialization::charsToInt(buffer,msgNum,offset);
    int16_t command_type;
    offset += 4;
    Serialization::charsToShort(buffer, command_type, offset);
    offset += 2;
    bool eom = false;

    if(command_type == CommandCode::s_players_command) {
        if(msgNum > lastServerMsgid)
        {
            do {
                command srvCmd;
                srvCmd.msgNum = msgNum;
                srvCmd.type = command_type;
                Serialization::charsToShort(buffer, srvCmd.playerID, offset);
                if (srvCmd.playerID != -1)
                {
                    offset += 2;
                    Serialization::charsToFloat(buffer, srvCmd.posx, offset);
                    offset += 4;
                    Serialization::charsToFloat(buffer, srvCmd.posy, offset);
                    offset += 4;
                    Serialization::charsToFloat(buffer, srvCmd.rotation, offset);
                    offset += 4;
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
    else if(command_type == CommandCode::s_projectile_command)
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
    else if (command_type == CommandCode::s_player_id_command)
    {
        command srvCmd;
        srvCmd.msgNum = msgNum;
        srvCmd.type = command_type;
        Serialization::charsToShort(buffer, srvCmd.playerID, offset);
        if(playerID == -1)
            playerID = srvCmd.playerID;
    }
}

void Game::requestJoinGame()
{
    int16_t offset = 0;
    Serialization::shortToChars(CommandCode::c_join_game_command,buffer,offset);
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