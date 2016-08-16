#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include "serialization.h"
#include "command.h"
#include "Player.h"
#include "Projectile.h"
#include "SFUtils.h"
#include "ResourceHolder.h"
#include <fstream>

enum Textures
{
    CROSSHAIR,
    PLAYER_RED_SG
};


int moves = 0;
sf::View camera;
sf::Vector2f camCenter;
sf::FloatRect bounds(0,0,2400.0f, 2400.0f);
int msgnm = 0, lastServerMsgid = -1;
sf::Sprite cursorSprite;

/***SOCKET**/
int clientSocket, nBytes;
int16_t playerID = -1;

//Client commands
const int16_t  c_input_command = 0;
const int16_t c_join_game_command = 2;

//Server commands
const int16_t s_players_command = 0;
const int16_t  s_projectile_command = 1;
const int16_t s_player_id_command = 2;

char * c_ip;
char * s_ip;
const int s_port = 50420;
const int c_port = 50421;
ResourceHolder<sf::Texture, Textures> textureHolder;

const int COMMAND_DATA_SIZE= 1400;
char outputBuff[COMMAND_DATA_SIZE];
char inputBuff[COMMAND_DATA_SIZE];
struct sockaddr_in serverAddr;
socklen_t addr_size;
int msgid;
float rotation;

sf::RenderWindow mWindow(sf::VideoMode(800, 600), "President Evil", sf::Style::Close);
const sf::Time TimePerFrame = sf::seconds(1.f/50.f);
std::queue<command_t> commandQueue;
std::queue<int32_t> projectileACKQueue;
std::map<int16_t,Projectile> projectiles;
std::vector<Player> players;
pthread_mutex_t commandQueueMutex;
pthread_mutex_t projectileACKMutex;
pthread_t listeningThread;

void requestJoinGame()
{
    int16_t offset = 0;
    shortToChars(c_join_game_command,outputBuff,offset);
    offset += 2;
    shortToChars(-1,outputBuff,offset);
    sendto(clientSocket,outputBuff,COMMAND_DATA_SIZE,0,(struct sockaddr *)&serverAddr,addr_size);
}


void readConfig()
{
    std::ifstream configFile("pe-client.config");
    std::string line;

    while (std::getline(configFile, line))
    {
        if (line.find("client-ip=") == 0)
        {
            u_long end = line.find(";");
            c_ip = (char *) malloc((end - 10) * sizeof(char));
            strcpy(c_ip, line.substr(10, end - 10).c_str());
            printf("Client IP: %s\n", c_ip);
        }
        else if (line.find("server-ip=") == 0)
        {
            u_long end = line.find(";");
            s_ip = (char *) malloc((end - 10) * sizeof(char));
            strcpy(s_ip, line.substr(10, end - 10).c_str());
            printf("Server IP: %s\n", s_ip);
        }
    }
}


void initSocket()
{
    /*Create UDP socket*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(s_port);
    serverAddr.sin_addr.s_addr = inet_addr(s_ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
}

void calculateCamCenter()
{
    camCenter = players[playerID].sprite.getPosition();

    if (players[playerID].sprite.getPosition().x < mWindow.getSize().x /2)
        camCenter.x = mWindow.getSize().x /2;
    else if (players[playerID].sprite.getPosition().x > bounds.width - mWindow.getSize().x /2)
        camCenter.x = players[playerID].sprite.getPosition().x;//bounds.width - 300;

    if (players[playerID].sprite.getPosition().y < mWindow.getSize().y /2)
        camCenter.y = mWindow.getSize().y /2;
    else if (players[playerID].sprite.getPosition().y > bounds.height - mWindow.getSize().y /2)
        camCenter.y = players[playerID].sprite.getPosition().y;//bounds.height - 300;
}


void render()
{
    mWindow.clear();

    calculateCamCenter();
    sf::FloatRect visibleRect(camCenter.x - mWindow.getSize().x / 2, camCenter.y - mWindow.getSize().y / 2, mWindow.getSize().x, mWindow.getSize().y);
    mWindow.setView(camera);
    camera.setCenter(camCenter);

    for(auto const &projectile : projectiles)
    {
        if(projectile.second.valid)
            mWindow.draw(projectile.second.projectile);
    }

    for (auto &player : players)
    {
        double angle = player.rotation * 180/M_PI;
        sf::IntRect spriteRect = player.sprite.getTextureRect();
        player.sprite.setOrigin(sf::Vector2f(spriteRect.width/ 2, spriteRect.height / 2));
        player.sprite.setRotation(angle);
        mWindow.draw(player.sprite);
    }
    mWindow.draw(cursorSprite);
    mWindow.display();
}



void processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        // Request for closing the window
        if (event.type == sf::Event::Closed)
            mWindow.close();
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

    sf::Vector2f mousePositionFloat = mWindow.mapPixelToCoords((sf::Mouse::getPosition(mWindow)));
    sf::Vector2f spriteCenter = getSpriteCenter(players[playerID].sprite);
    sf::Vector2f facing = mousePositionFloat - spriteCenter;
    rotation = atan2f(facing.y, facing.x);

}

void processServerEvents()
{
    pthread_mutex_lock(&commandQueueMutex);
    while(!commandQueue.empty())
    {
        command_t command = commandQueue.front();
        commandQueue.pop();
        if(command.type == c_input_command)
        {
            players[command.playerID].sprite.setPosition(command.posx, command.posy);
            players[command.playerID].rotation = command.rotation;
        }
        else if (command.type == s_projectile_command)
        {
            sf::Vector2f position = sf::Vector2f(command.posx,command.posy);
            sf::Vector2f origin = sf::Vector2f(command.originx,command.originy);
            Projectile projectile = Projectile(command.bulletID,command.bulletType,position,origin);
            printf("insertando proyectil %d \n",command.bulletID);
            projectiles.insert(std::pair<int16_t , Projectile>(command.bulletID, projectile));
        }
        else if (command.type == s_player_id_command)
        {
            playerID = command.playerID;
        }
    }
    pthread_mutex_unlock(&commandQueueMutex);
}

void sendCommands()
{
    if (playerID == -1)
    {
        requestJoinGame();
        return;
    }


    int16_t offset = 0;
    shortToChars(c_input_command,outputBuff,offset);
    offset+=2;
    shortToChars(playerID,outputBuff,offset);
    offset+=2;
    intToChars(msgnm,outputBuff,offset);
    offset+=4;
    intToChars(moves,outputBuff,offset);
    offset+=4;
    floatToChars(rotation,outputBuff,offset);
    offset+=4;
    pthread_mutex_lock(&projectileACKMutex);
    while(!projectileACKQueue.empty())
    {
        shortToChars(s_projectile_command,outputBuff,offset);
        offset+=2;
        int32_t ack = projectileACKQueue.front();
        projectileACKQueue.pop();
        intToChars(ack,outputBuff,offset);
        offset+=4;
    }
    pthread_mutex_unlock(&projectileACKMutex);
    shortToChars(-1,outputBuff,offset);
    sendto(clientSocket,outputBuff,COMMAND_DATA_SIZE,0,(struct sockaddr *)&serverAddr,addr_size);
    msgnm++;
}

void *listenToServer(void * args)
{
    /*Create UDP socket*/
    int udpSocket;
    struct sockaddr_in serverAddr2;
    struct sockaddr_storage serverStorage2;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    /*Configure settings in address struct*/
    serverAddr2.sin_family = AF_INET;
    serverAddr2.sin_port = htons(c_port);
    serverAddr2.sin_addr.s_addr = inet_addr(c_ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Bind socket with address struct*/
    bind(udpSocket, (struct sockaddr *) &serverAddr2, sizeof(serverAddr2));

    /*Initialize size variable to be used later on*/
    socklen_t addr_size2 = sizeof serverStorage2;

    while(1)
    {
        /* Try to receive any incoming UDP datagram. Address and port of
          requesting client will be stored on serverStorage variable */

        nBytes = recvfrom(udpSocket, inputBuff, COMMAND_DATA_SIZE, 0, (struct sockaddr *)&serverStorage2, &addr_size2);

        int32_t  msgNum;
        short offset = 0;
        charsToInt(inputBuff,msgNum,offset);
        lastServerMsgid = msgid;
        int16_t command_type;
        offset += 4;
        charsToShort(inputBuff, command_type, offset);
        offset += 2;
        bool eom = false;

        if(command_type == s_players_command) {
            if(msgNum > lastServerMsgid)
            {
                do {
                    command srvCmd;
                    srvCmd.msgNum = msgNum;
                    srvCmd.type = command_type;
                    charsToShort(inputBuff, srvCmd.playerID, offset);
                    if (srvCmd.playerID != -1)
                    {
                        offset += 2;
                        charsToFloat(inputBuff, srvCmd.posx, offset);
                        offset += 4;
                        charsToFloat(inputBuff, srvCmd.posy, offset);
                        offset += 4;
                        charsToFloat(inputBuff, srvCmd.rotation, offset);
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
        else if(command_type == s_projectile_command)
        {
            do
            {
                command srvCmd;
                srvCmd.msgNum = msgNum;
                srvCmd.type = command_type;
                charsToShort(inputBuff, srvCmd.bulletID, offset);
                if (srvCmd.bulletID != -1)
                {
                    offset += 2;
                    charsToShort(inputBuff, srvCmd.bulletType, offset);
                    offset += 2;
                    charsToFloat(inputBuff, srvCmd.posx, offset);
                    offset += 4;
                    charsToFloat(inputBuff, srvCmd.posy, offset);
                    offset += 4;
                    charsToFloat(inputBuff, srvCmd.originx, offset);
                    offset += 4;
                    charsToFloat(inputBuff, srvCmd.originy, offset);
                    offset += 4;

                    pthread_mutex_lock(&commandQueueMutex);
                    if(projectiles.count(srvCmd.bulletID) == 0)
                    {
                        printf("x1 : %f y1 : %f, x2: %f y2: %f ",srvCmd.posx,srvCmd.posy,srvCmd.originx,srvCmd.posy);
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
            charsToShort(inputBuff, srvCmd.playerID, offset);
            pthread_mutex_lock(&commandQueueMutex);
            commandQueue.push(srvCmd);
            pthread_mutex_unlock(&commandQueueMutex);
        }
    }

    return 0;
}

void updateCrosshair()
{
    sf::Vector2f mousePositionFloat = mWindow.mapPixelToCoords((sf::Mouse::getPosition(mWindow)));
    sf::IntRect cursorSpriteRect = cursorSprite.getTextureRect();
    cursorSprite.setPosition(mousePositionFloat - sf::Vector2f(cursorSpriteRect.width*1.5f , cursorSpriteRect.height*1.5f));
}

void loadTextures()
{
    textureHolder.load(Textures::CROSSHAIR, "files/crosshair.png");
    textureHolder.load(Textures::PLAYER_RED_SG, "files/sprite.png");
}

void init()
{
    loadTextures();

    camera.reset(sf::FloatRect(0,0, mWindow.getSize().x, mWindow.getSize().y));
    camera.setViewport(sf::FloatRect(0,0, 1.0f, 1.0f));

    cursorSprite.setTexture(textureHolder.get(Textures::CROSSHAIR));
    pthread_mutex_init(&commandQueueMutex, NULL);
    pthread_mutex_init(&projectileACKMutex, NULL);

    players.push_back(Player(0,0,0,textureHolder.get(Textures::PLAYER_RED_SG)));
    players.push_back(Player(1,100,100,textureHolder.get(Textures::PLAYER_RED_SG)));
}

void deleteInvalidProjectiles()
{
    //delete invalid projectiles
    if(projectiles.size() >= 100)
    {
        for (auto it = projectiles.cbegin(); it != projectiles.cend();) {
            if (!(*it).second.valid) {
                printf("proyectil %d eliminado\n", (*it).first);
                projectiles.erase(it++);
            }
            else {
                ++it;
            }
        }
    }
}

void update(sf::Time elapsedTime)
{


    for (auto &projectile : projectiles)
    {
        if (projectile.second.valid)
        {
            projectile.second.update(elapsedTime);
        }
    }

    deleteInvalidProjectiles();
    updateCrosshair();
}


int main()
{
    readConfig();
    init();
    initSocket();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    pthread_create(&listeningThread, NULL, listenToServer, NULL);


    mWindow.setVerticalSyncEnabled(true);
    bool should_render = true;

    while (mWindow.isOpen())
    {
        sf::Time elapsedTime = clock.restart();
        timeSinceLastUpdate += elapsedTime;
        while (timeSinceLastUpdate > TimePerFrame)
        {
            timeSinceLastUpdate -= TimePerFrame;
            processServerEvents();
            processEvents();
            sendCommands();
            should_render = true;
            update(TimePerFrame);

        }
        //updateStatistics(elapsedTime);
        if (should_render)
        {
            render();
            should_render = false;
        }

    }

    return 0;
}

