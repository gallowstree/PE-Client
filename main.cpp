#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <SFML/Graphics.hpp>
#include "serialization.h"
#include "command.h"
#include "Player.h"

int moves;
int msgnm = 0, lastServerMsgid = -1;


/***SOCKET**/
int clientSocket, nBytes;
int16_t playerID = 0;
int16_t const c_input_command = 0;
const char * c_ip = "192.168.1.12";
const char * s_ip = "192.168.1.90";
const int s_port = 50420;
const int c_port = 50421;

int const COMMAND_BUFFER_SIZE = 1024;
char buff[1024];
struct sockaddr_in serverAddr;
socklen_t addr_size;
int msgid;

sf::RenderWindow mWindow(sf::VideoMode(1024, 768), "President Evil", sf::Style::Close);
const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
std::queue<command_t> commandQueue;
std::vector<Player> players;
pthread_mutex_t commandQueueMutex;
pthread_t listeningThread;

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



void render()
{
    mWindow.clear();
    for (auto &player : players)
    {
        mWindow.draw(player.circle);
    }
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
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        moves |= 0x1;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        moves |= 0x2;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moves |= 0x4;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moves |= 0x8;
    if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        moves |= 0x10;
}

void processServerEvents()
{
    pthread_mutex_lock(&commandQueueMutex);
    while(!commandQueue.empty())
    {
        command_t command = commandQueue.front();
        commandQueue.pop();
        players[command.playerId].circle.setPosition(command.posx,command.posy);
    }
    pthread_mutex_unlock(&commandQueueMutex);
}

void sendCommands()
{
    shortToChars(c_input_command,buff,0);
    shortToChars(playerID,buff,2);
    intToChars(msgnm,buff,4);
    intToChars(moves,buff,8);
    sendto(clientSocket,buff,1024,0,(struct sockaddr *)&serverAddr,addr_size);
    msgnm++;
}

void *listenToServer(void * args)
{
    /*Create UDP socket*/
    int udpSocket;
    struct sockaddr_in serverAddr2;
    struct sockaddr_storage serverStorage;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    char buffer[1024];
    /*Configure settings in address struct*/
    serverAddr2.sin_family = AF_INET;
    serverAddr2.sin_port = htons(c_port);
    serverAddr2.sin_addr.s_addr = inet_addr(c_ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Bind socket with address struct*/
    bind(udpSocket, (struct sockaddr *) &serverAddr2, sizeof(serverAddr2));

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverStorage;

    while(1)
    {
        /* Try to receive any incoming UDP datagram. Address and port of
          requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(udpSocket, buffer, COMMAND_BUFFER_SIZE, 0, (struct sockaddr *)&serverStorage, &addr_size);

        int32_t  msgNum;
        short offset = 0;
        charsToInt(buffer,msgNum,offset);
        if(msgNum > lastServerMsgid)
        {
            lastServerMsgid = msgid;
            int16_t command_type;
            offset += 4;
            charsToShort(buffer, command_type, offset);
            if(command_type == c_input_command)
            {
                offset += 2;
                bool eom = false;
                do {
                    command srvCmd;
                    charsToShort(buffer, srvCmd.playerId, offset);
                    if (srvCmd.playerId != -1) {
                        offset += 2;
                        charsToFloat(buffer, srvCmd.posx, offset);
                        offset += 4;
                        charsToFloat(buffer, srvCmd.posy, offset);
                        offset += 4;
                        pthread_mutex_lock(&commandQueueMutex);
                        commandQueue.push(srvCmd);
                        pthread_mutex_unlock(&commandQueueMutex);
                    }
                    else {
                        eom = true;
                    }

                } while (!eom);
            }
        }
    }

    return 0;
}

void init()
{
    pthread_mutex_init(&commandQueueMutex, NULL);
    players.push_back(Player(0,0,0,sf::Color::Blue));
    players.push_back(Player(1,0,0,sf::Color::Red));
}

int main()
{
    init();

    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    pthread_create(&listeningThread, NULL, listenToServer, NULL);
    initSocket();
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
            //update(TimePerFrame);

        }
        //updateStatistics(elapsedTime);

        render();
    }

    return 0;
}

