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
const int c_port_join = 50422;

sf::RenderWindow mWindow(sf::VideoMode(800, 600), "President Evil", sf::Style::Close);

int main()
{
    World::loadTextures();
    Menu menu(mWindow);
    menu.run(1);
    loopStage2:
    menu.run(2);
    ServerSocket sSocket(menu.s_ip, s_port);
    ClientSocket cSocketJoin(menu.c_ip, c_port_join, &menu);
    cSocketJoin.timeout = 5;
    cSocketJoin.keepAlive = false;
    Serialization::shortToChars(c_are_you_there, buffer, 0);
    printf("mandando nick %s\n",menu.nick);
    strcpy(buffer + 2, menu.nick);
    sSocket.send(buffer, COMMAND_BUFFER_SIZE);
    if(cSocketJoin.isListening)
        cSocketJoin.run();

    if (menu.currentStage == 2)
    {
        goto loopStage2;
    }

    printf("result %d\n",menu.connResult);
    if(menu.connResult == 1)
        menu.run(3);

    menu.menuSong.stop();

    Game game(mWindow,&sSocket,menu.selectedTeam);
    ClientSocket cSocket(menu.c_ip, c_port, &game);
    cSocket.timeout = 0;
    cSocket.keepAlive = true;
    pthread_t listening_thread;
    pthread_create(&listening_thread, nullptr, &ClientSocket::runThread, &cSocket);


    game.getID();
    game.run();
    if(game.restart)
    {
        game.reset();
        cSocket.timeout =1;
        cSocket.keepAlive = false;
        menu.message.setString("");
        menu.connResult = 0;
        goto loopStage2;
    }

    return 0;
}



