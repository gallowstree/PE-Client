#include <stdio.h>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <SFML/Graphics.hpp>
#include "serialization.h"
#include "command.h"
#include <pthread.h>

int moves;
int msgnm = 0;


/***SOCKET**/
int clientSocket, portNum, nBytes;
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
float x1,y_1,x2,y2;

sf::RenderWindow mWindow(sf::VideoMode(1024, 768), "SFML Application", sf::Style::Close);
const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
sf::CircleShape circle(10);
sf::CircleShape circle2(10);
std::queue<command_t> commandQueue;
pthread_mutex_t commandQueueMutex;
pthread_t listeningThread;


void render()
{
    mWindow.clear();
    mWindow.draw(circle);
    mWindow.draw(circle2);
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

void sendCommands()
{
    shortToChars(c_input_command,buff,0);
    shortToChars(playerID,buff,2);
    intToChars(msgnm,buff,4);
    intToChars(moves,buff,8);
    sendto(clientSocket,buff,1024,0,(struct sockaddr *)&serverAddr,addr_size);
    msgnm++;
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

        charsToInt(buffer,msgid,0);
        charsToShort(buffer,playerID,4);
        charsToShort(buffer,playerID,6);
        charsToFloat(buffer,x1,8);
        charsToFloat(buffer,y_1,12);
        circle.setPosition(x1,y_1);
        charsToFloat(buffer,x2,20);
        charsToFloat(buffer,y2,24);
        circle2.setPosition(x2,y2);
    }

    return 0;
}

int main()
{
    mWindow.setVerticalSyncEnabled(true);

    circle.setFillColor(sf::Color::Blue);
    circle2.setFillColor(sf::Color::Red);

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
            processEvents();
            sendCommands();
            //update(TimePerFrame);

        }
        //updateStatistics(elapsedTime);

        render();
    }

    return 0;
}

