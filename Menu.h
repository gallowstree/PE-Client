//
// Created by daniel on 20/08/16.
//

#ifndef TEST_CLIENT_MENU_H
#define TEST_CLIENT_MENU_H

#include <SFML/Graphics.hpp>
#include <netinet/in.h>
#include "ClientSocket.h"

class Menu : public SocketListener {
    static const int MENU_SIZE = 2;
    static const int COMMAND_BUFFER_SIZE = 1500;
public:
    Menu(sf::RenderWindow& window);

    void stage1();
    void stage2();
    void run(int stage);

private:
    const int16_t s_are_you_there = 3; //comando para ver si el server esta vivo
    char buffer[COMMAND_BUFFER_SIZE];
    int selectedOption = 0;
    int currentStage = 0;
    sf::RenderWindow&  window;
    sf::Texture	menuIconTexture;
    sf::Sprite menuIcon;
    sf::Text menu[MENU_SIZE];
    sf::Text ipConf[5];
    sf::Text mainTitle;
    sf::Font menuFont;
    char * c_ip;
    char * s_ip;
    void deleteLastChar(char * str);
    void addChar(char * str, char c, int max);
    void readConfig();
    void receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr);
};


#endif //TEST_CLIENT_MENU_H
