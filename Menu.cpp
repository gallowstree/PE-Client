//
// Created by daniel on 20/08/16.
//

#include "Menu.h"
#include "serialization.h"
#include <fstream>


Menu::Menu(sf::RenderWindow& window):
window(window)
{
    readConfig();
    menuIconTexture.loadFromFile("files/skull-icon.png");
    menuFont.loadFromFile("files/sansation.ttf");
    teamTexture1.loadFromFile("files/sprite.png");
    menuIcon.setTexture(menuIconTexture);

}

void Menu::run(int stage)
{
    currentStage = stage;
    if(currentStage == 1)
        stage1();
    else if (currentStage == 2)
        stage2();
    else if (currentStage == 3)
        stage3();
}

void Menu::stage1()
{
    mainTitle.setString("President Evil");
    mainTitle.setCharacterSize(100);
    mainTitle.setPosition(90,100);
    mainTitle.setColor(sf::Color::Red);
    mainTitle.setFont(menuFont);

    menu[0].setFont(menuFont);
    menu[0].setString("JOIN ARENA");
    menu[0].setPosition(300,270);
    menu[0].setColor(sf::Color::White);
    menu[0].setCharacterSize(35);

    menu[1].setFont(menuFont);
    menu[1].setString("EXIT");
    menu[1].setPosition(360,330);
    menu[1].setColor(sf::Color::White);
    menu[1].setCharacterSize(35);

    bool joinArena = false;
    while (!joinArena)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Key::Up:
                            if (selectedOption > 0)
                                selectedOption--;
                            break;
                        case sf::Keyboard::Key::Down:
                            if (selectedOption + 1 < MENU_SIZE)
                                selectedOption++;
                            break;
                        case sf::Keyboard::Key::Return:
                            if (selectedOption == 0)
                            {
                                joinArena = true;
                                currentStage = 2;
                            }
                            else if (selectedOption == 1)
                            {
                                window.close();
                                exit(0);
                            }
                    }
                default:
                    break;
            }
        }

        window.clear();
        window.draw(mainTitle);
        for (int i = 0; i < MENU_SIZE; i++) {
            window.draw(menu[i]);
            if (i == selectedOption) {
                menuIcon.setPosition(menu[i].getPosition().x - 55, menu[i].getPosition().y + 2);
            }
        }
        window.draw(menuIcon);
        window.display();
    }
}

void Menu::stage2()
{
    selectedOption = 0;

    mainTitle.setString("IP SETTINGS");
    mainTitle.setCharacterSize(40);
    mainTitle.setPosition(280,20);
    mainTitle.setColor(sf::Color::Red);
    mainTitle.setFont(menuFont);

    ipConf[0].setFont(menuFont);
    ipConf[0].setString(c_ip);
    ipConf[0].setPosition(275,150);
    ipConf[0].setColor(sf::Color::White);
    ipConf[0].setCharacterSize(35);

    ipConf[1].setFont(menuFont);
    ipConf[1].setString(s_ip);
    ipConf[1].setPosition(275,300);
    ipConf[1].setColor(sf::Color::White);
    ipConf[1].setCharacterSize(35);


    ipConf[2].setFont(menuFont);
    ipConf[2].setString("TEST SERVER");
    ipConf[2].setPosition(290,400);
    ipConf[2].setColor(sf::Color::White);
    ipConf[2].setCharacterSize(35);

    ipConf[3].setFont(menuFont);
    ipConf[3].setString("YOUR IP");
    ipConf[3].setPosition(320,100);
    ipConf[3].setColor(sf::Color::White);
    ipConf[3].setCharacterSize(35);


    ipConf[4].setFont(menuFont);
    ipConf[4].setString("SERVER IP");
    ipConf[4].setPosition(310,250);
    ipConf[4].setColor(sf::Color::White);
    ipConf[4].setCharacterSize(35);


    connResultTxt.setCharacterSize(20);
    connResultTxt.setFont(menuFont);

    sf::RectangleShape ipRect(sf::Vector2f(260,35));
    ipRect.setFillColor(sf::Color::White);
    bool selectTeam = false;
    while (!selectTeam)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Key::Up:
                            if (selectedOption > 0)
                                selectedOption--;
                            break;
                        case sf::Keyboard::Key::Down:
                            if (selectedOption + 1 < 3)
                                selectedOption++;
                            break;
                        case sf::Keyboard::Key::BackSpace:
                                if(selectedOption == 0)
                                    deleteLastChar(c_ip);
                                if(selectedOption == 1)
                                    deleteLastChar(s_ip);
                            break;
                        case sf::Keyboard::Num0:
                            if(selectedOption == 0)
                                addChar(c_ip,'0',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'0',15);
                            break;
                        case sf::Keyboard::Num1:
                            if(selectedOption == 0)
                                addChar(c_ip,'1',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'1',15);
                            break;
                        case sf::Keyboard::Num2:
                            if(selectedOption == 0)
                                addChar(c_ip,'2',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'2',15);
                            break;
                        case sf::Keyboard::Num3:
                            if(selectedOption == 0)
                                addChar(c_ip,'3',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'3',15);
                            break;
                        case sf::Keyboard::Num4:
                            if(selectedOption == 0)
                                addChar(c_ip,'4',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'4',15);
                            break;
                        case sf::Keyboard::Num5:
                            if(selectedOption == 0)
                                addChar(c_ip,'5',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'5',15);
                            break;
                        case sf::Keyboard::Num6:
                            if(selectedOption == 0)
                                addChar(c_ip,'6',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'6',15);
                            break;
                        case sf::Keyboard::Num7:
                            if(selectedOption == 0)
                                addChar(c_ip,'7',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'7',15);
                            break;
                        case sf::Keyboard::Num8:
                            if(selectedOption == 0)
                                addChar(c_ip,'8',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'8',15);
                            break;
                        case sf::Keyboard::Num9:
                            if(selectedOption == 0)
                                addChar(c_ip,'9',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'9',15);
                            break;
                        case sf::Keyboard::Period:
                            if(selectedOption == 0)
                                addChar(c_ip,'.',15);
                            if(selectedOption == 1)
                                addChar(s_ip,'.',15);
                            break;
                        case sf::Keyboard::Key::Return:
                            if(selectedOption == 2)
                            {
                                selectTeam = true;
                            }
                            break;
                    }
                default:
                    break;
            }
        }

        ipConf[0].setString(c_ip);
        ipConf[1].setString(s_ip);

        window.clear();

        window.draw(mainTitle);
        for(int i =0; i < 5;i++)
        {
            ipConf[i].setColor(sf::Color::White);
            if(i < 2)
            {
                ipRect.setPosition(ipConf[i].getPosition().x - 10, ipConf[i].getPosition().y + 5);
                if (selectedOption == i)
                {
                    ipConf[i].setColor(sf::Color(30,30,30));
                    ipRect.setFillColor(sf::Color::White);
                }
                else
                {
                    ipRect.setFillColor(sf::Color(30,30,30));
                }

                window.draw(ipRect);
            }
            if(selectedOption == 2 && i == selectedOption)
            {
                menuIcon.setPosition(ipConf[i].getPosition().x - 55, ipConf[i].getPosition().y + 2);
                window.draw(menuIcon);
            }
            window.draw(ipConf[i]);

        }


        if(this->connResult == -1)
        {
            connResultTxt.setString("Timeout: could not connect to server :/");
            connResultTxt.setColor(sf::Color::Red);
            connResultTxt.setPosition(230,480);
        }
        else if(this->connResult == 1)
        {
            connResultTxt.setString("Connection Successful :D");
            connResultTxt.setColor(sf::Color::White);
            connResultTxt.setPosition(280,480);
        }
        else if(this->connResult == 2)
        {
            connResultTxt.setString("Error: the arena is full, try again later :(");
            connResultTxt.setColor(sf::Color::Red);
            connResultTxt.setPosition(225,480);
        }
        else
        {
            connResultTxt.setString("");
        }
        window.draw(connResultTxt);
        window.display();
    }
}

void Menu::stage3()
{
    selectedOption = 0;

    mainTitle.setString("CHOOSE YOUR DESTINY");
    mainTitle.setCharacterSize(40);
    mainTitle.setPosition(170,20);
    mainTitle.setColor(sf::Color::Red);
    mainTitle.setFont(menuFont);

    team[0].setTexture(teamTexture1);
    team[0].setPosition(220,250);
    team[1].setTexture(teamTexture1);
    team[1].setPosition(490,250);

    strcpy(currPlayers1,"CURRENT PLAYERS  ");
    strcpy(currPlayers2,"CURRENT PLAYERS  ");
    currPlayers1[16] = (char) playersTeam[0] + 48; //concatenamos la cantidad de players del team 1
    currPlayers2[16] = (char) playersTeam[1] + 48; //concatenamos la cantidad de players del team 2

    currentPlayers[0].setFont(menuFont);
    currentPlayers[0].setString(currPlayers1);
    currentPlayers[0].setColor(sf::Color::White);
    currentPlayers[0].setCharacterSize(15);
    currentPlayers[0].setPosition(190,350);

    currentPlayers[1].setFont(menuFont);
    currentPlayers[1].setString(currPlayers2);
    currentPlayers[1].setCharacterSize(15);
    currentPlayers[1].setPosition(460,350);

    sf::RectangleShape selection(sf::Vector2f(200,200));
    selection.setOutlineColor(sf::Color::White);
    selection.setFillColor(sf::Color::Transparent);
    selection.setOutlineThickness(5);
    bool startGame = false;
    while (!startGame)
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
                case sf::Event::Closed:
                    window.close();
                    exit(0);
                case sf::Event::KeyPressed:
                    switch (event.key.code)
                    {
                        case sf::Keyboard::Key::Left:
                            if (selectedOption > 0)
                                selectedOption--;
                            break;
                        case sf::Keyboard::Key::Right:
                            if (selectedOption + 1 < 2)
                                selectedOption++;
                            break;
                        case sf::Keyboard::Key::Return:
                            selectedTeam = selectedOption;
                            startGame = true;
                            break;
                    }
                default:
                    break;
            }
        }


        window.clear();

        for(int i = 0; i < 2; i++)
        {
            window.draw(team[i]);
            window.draw(currentPlayers[i]);
            if(selectedOption == i)
            {
                selection.setPosition(team[i].getPosition().x -60, team[i].getPosition().y -50 );
            }
        }
        window.draw(selection);
        window.draw(mainTitle);

        window.display();
    }
}

void Menu::readConfig()
{
    std::ifstream configFile("pe-client.config");
    std::string line;

    while (std::getline(configFile, line))
    {
        if (line.find("client-ip=") == 0)
        {
            u_long end = line.find(";");
            c_ip = (char *) malloc(16 * sizeof(char));
            strcpy(c_ip, line.substr(10, end - 10).c_str());
        }
        else if (line.find("server-ip=") == 0)
        {
            u_long end = line.find(";");
            s_ip = (char *) malloc(16 * sizeof(char));
            strcpy(s_ip, line.substr(10, end - 10).c_str());
        }
    }
}

void Menu::deleteLastChar(char * str)
{
    if(strlen(str) > 0)
    {
        str[strlen(str)-1] = '\0';
    }
}

void Menu::addChar(char * str, char c, int max)
{
    if(strlen(str) < max)
    {
        str[strlen(str)+1] = '\0';
        str[strlen(str)] = c;
    }
}


void Menu::receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr)
{

    if(currentStage == 2)
    {
        Serialization::charsToShort(buffer, this->connResult, 0);
        if (this->connResult == 1) {
            Serialization::charsToShort(buffer, this->playersTeam[0], 2);
            Serialization::charsToShort(buffer, this->playersTeam[1], 4);
            this->currentStage = 3;
        }
    }
}
