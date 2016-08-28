#include <pthread.h>
#include <queue>
#include <sys/socket.h>
#include "Player.h"
#include "Menu.h"
#include <zconf.h>
#include "serialization.h"
#include "CommandCode.h"
#include "Game.h"


static const int COMMAND_BUFFER_SIZE = 1500;
char buffer[COMMAND_BUFFER_SIZE];
const int s_port = 50420;
const int c_port = 50421;

sf::RenderWindow mWindow(sf::VideoMode(800, 600), "President Evil", sf::Style::Close);

int main()
{
    Menu menu(mWindow);
    menu.run(1);
    loopStage2:
    menu.run(2);
    ServerSocket sSocket(menu.s_ip, s_port);
    ClientSocket cSocket(menu.c_ip, c_port, &menu);
    cSocket.timeout = 2;
    pthread_t listening_thread;
    pthread_create(&listening_thread, nullptr, &ClientSocket::runThread, &cSocket);
    Serialization::shortToChars(c_are_you_there, buffer, 0);
    printf("mandando nick %s\n",menu.nick);
    memcpy(buffer + 2, menu.nick,strlen(menu.nick));
    sSocket.send(buffer, COMMAND_BUFFER_SIZE);
    sleep(2);
    if (menu.currentStage == 2)
        goto loopStage2;

    cSocket.timeout = 0;
    menu.run(3);


    Game game(mWindow,&sSocket,menu.selectedTeam);
    cSocket.listener = &game;
    game.getID();
    game.run();
    if(game.restart)
    {
        cSocket.listener = &menu;
        goto loopStage2;
    }
    return 0;
}



