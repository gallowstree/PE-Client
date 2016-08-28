//
// Created by daniel on 20/08/16.
//

#ifndef TEST_CLIENT_MENU_H
#define TEST_CLIENT_MENU_H

#include <SFML/Graphics.hpp>
#include <netinet/in.h>
#include <memory.h>
#include "ClientSocket.h"
#include "ServerSocket.h"

class Menu : public SocketListener {
    static const int MENU_SIZE = 2;

public:
    Menu(sf::RenderWindow& window);

    void stage1();
    void stage2();
    void stage3();
    void run(int stage);
    char * c_ip;
    char * s_ip;
    int currentStage = 0;
    int16_t selectedTeam = 0;

private:
    int selectedOption = 0;
    char * currPlayers1 = (char *)malloc(strlen("CURRENT PLAYERS   ")+1);
    char * currPlayers2 = (char *)malloc(strlen("CURRENT PLAYERS   ")+1);
    sf::RenderWindow&  window;
    sf::Texture	menuIconTexture;
    sf::Sprite menuIcon;
    sf::Text menu[MENU_SIZE];
    sf::Text ipConf[7];
    sf::Text mainTitle;
    sf::Text connResultTxt;
    sf::Font menuFont;
    sf::Sprite team[2];
    sf::Texture teamTexture1;
    sf::Texture teamTexture2;
    sf::Text currentPlayers[2];
    int16_t connResult = 0;
    void deleteLastChar(char * str);
    void addChar(char * str, char c, int max);
    void readConfig();
    void receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr);
    int16_t playersTeam[2];
};


#endif //TEST_CLIENT_MENU_H
