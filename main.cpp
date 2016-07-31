#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <SFML/Graphics.hpp>
#include "serialization.h"
#include <pthread.h>

int moves;
int msgnm = 0;


/***SOCKET**/
int clientSocket, portNum, nBytes;
int const COMMAND_BUFFER_SIZE = 1024;
char buff[1024];
struct sockaddr_in serverAddr;
socklen_t addr_size;
int msgid,playerID;
float x1,y_1,x2,y2;

sf::RenderWindow mWindow(sf::VideoMode(800, 700), "SFML Application", sf::Style::Close);
const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
sf::CircleShape circle(10);
sf::CircleShape circle2(10);
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
        moves |= 0x0001;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        moves |= 0x0010;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        moves |= 0x0100;
    if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        moves |= 0x1000;
}

void sendCommands()
{

    short client = 1;

    intToChars(client,buff,0);
    intToChars(msgnm,buff,4);
    intToChars(moves,buff,8);
    int result = sendto(clientSocket,buff,1024,0,(struct sockaddr *)&serverAddr,addr_size);
    msgnm++;
    //printf("mandando %04x, %d, %d\n",moves,msgnm,client);
}

void initSocket()
{
    /*Create UDP socket*/
    clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(7891);
    serverAddr.sin_addr.s_addr = inet_addr("192.168.1.78");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

    /*Initialize size variable to be used later on*/
    addr_size = sizeof serverAddr;
}

void *listenToServer(void * args)
{
    int udpSocket, nBytes;
    char buffer[COMMAND_BUFFER_SIZE];
    struct sockaddr_in serverAddr, clientAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    int i;



    while(1)
    {
        /* Try to receive any incoming UDP datagram. Address and port of
          requesting client will be stored on serverStorage variable */
        nBytes = recvfrom(clientSocket, buffer, COMMAND_BUFFER_SIZE, 0, (struct sockaddr *)&serverStorage, &addr_size);

        charsToInt(buffer,msgid,0);
        charsToInt(buffer,playerID,4);
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

