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
    teamTexture2.loadFromFile("files/sprite2.png");
    menuIcon.setTexture(menuIconTexture);

    ipRegexMatch = regcomp(&ipRegex, "^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)."
            "(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$", REG_EXTENDED);
    if (ipRegexMatch) {
        fprintf(stderr, "Could not compile regex\n");
        exit(1);
    }

    nick = (char *)malloc(sizeof(char)*7);
    memset(nick,0,7);

    menuSong.openFromFile("files/sound/delay.ogg");
    menuSong.setLoop(true);
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

    window.setView(window.getDefaultView());
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
    ipConf[1].setPosition(275,275);
    ipConf[1].setColor(sf::Color::White);
    ipConf[1].setCharacterSize(35);

    ipConf[2].setFont(menuFont);
    ipConf[2].setString(nick);
    ipConf[2].setPosition(275,400);
    ipConf[2].setColor(sf::Color::White);
    ipConf[2].setCharacterSize(35);

    ipConf[3].setFont(menuFont);
    ipConf[3].setString("JOIN SERVER");
    ipConf[3].setPosition(290,475);
    ipConf[3].setColor(sf::Color::White);
    ipConf[3].setCharacterSize(35);

    ipConf[4].setFont(menuFont);
    ipConf[4].setString("YOUR IP");
    ipConf[4].setPosition(320,100);
    ipConf[4].setColor(sf::Color::White);
    ipConf[4].setCharacterSize(35);

    ipConf[5].setFont(menuFont);
    ipConf[5].setString("SERVER IP");
    ipConf[5].setPosition(310,225);
    ipConf[5].setColor(sf::Color::White);
    ipConf[5].setCharacterSize(35);

    ipConf[6].setFont(menuFont);
    ipConf[6].setString("NICKNAME");
    ipConf[6].setPosition(300,350);
    ipConf[6].setColor(sf::Color::White);
    ipConf[6].setCharacterSize(35);

    message.setCharacterSize(20);
    message.setFont(menuFont);
    message.setPosition(230,520);

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
                            if (selectedOption + 1 < 4)
                                selectedOption++;
                            break;
                        case sf::Keyboard::Key::BackSpace:
                            deleteLastChar();
                            break;
                        case sf::Keyboard::Num0:
                            addChar('0');
                            break;
                        case sf::Keyboard::Num1:
                            addChar('1');
                            break;
                        case sf::Keyboard::Num2:
                            addChar('2');
                            break;
                        case sf::Keyboard::Num3:
                            addChar('3');
                            break;
                        case sf::Keyboard::Num4:
                            addChar('4');
                            break;
                        case sf::Keyboard::Num5:
                            addChar('5');
                            break;
                        case sf::Keyboard::Num6:
                            addChar('6');
                            break;
                        case sf::Keyboard::Num7:
                            addChar('7');
                            break;
                        case sf::Keyboard::Num8:
                            addChar('8');
                            break;
                        case sf::Keyboard::Num9:
                            addChar('9');
                            break;
                        case sf::Keyboard::Period:
                            addChar('.');
                            break;
                        case sf::Keyboard::A:
                            addChar('a');
                            break;
                        case sf::Keyboard::B:
                            addChar('b');
                            break;
                        case sf::Keyboard::C:
                            addChar('c');
                            break;
                        case sf::Keyboard::D:
                            addChar('d');
                            break;
                        case sf::Keyboard::E:
                            addChar('e');
                            break;
                        case sf::Keyboard::F:
                            addChar('f');
                            break;
                        case sf::Keyboard::G:
                            addChar('g');
                            break;
                        case sf::Keyboard::H:
                            addChar('h');
                            break;
                        case sf::Keyboard::I:
                            addChar('i');
                            break;
                        case sf::Keyboard::J:
                            addChar('j');
                            break;
                        case sf::Keyboard::K:
                            addChar('k');
                            break;
                        case sf::Keyboard::L:
                            addChar('l');
                            break;
                        case sf::Keyboard::M:
                            addChar('m');
                            break;
                        case sf::Keyboard::N:
                            addChar('n');
                            break;
                        case sf::Keyboard::O:
                            addChar('o');
                            break;
                        case sf::Keyboard::P:
                            addChar('p');
                            break;
                        case sf::Keyboard::Q:
                            addChar('q');
                            break;
                        case sf::Keyboard::R:
                            addChar('r');
                            break;
                        case sf::Keyboard::S:
                            addChar('s');
                            break;
                        case sf::Keyboard::T:
                            addChar('t');
                            break;
                        case sf::Keyboard::U:
                            addChar('u');
                            break;
                        case sf::Keyboard::V:
                            addChar('v');
                            break;
                        case sf::Keyboard::W:
                            addChar('w');
                            break;
                        case sf::Keyboard::X:
                            addChar('x');
                            break;
                        case sf::Keyboard::Y:
                            addChar('y');
                            break;
                        case sf::Keyboard::Z:
                            addChar('z');
                            break;
                        case sf::Keyboard::Dash:
                            addChar('-');
                            break;
                        case sf::Keyboard::Key::Return:
                            message.setString("");
                            message.setColor(sf::Color::Red);
                            if(selectedOption == 3)
                            {
                                ipRegexMatch = regexec(&ipRegex, c_ip, 0, NULL, 0);
                                if(!ipRegexMatch)
                                {
                                    ipRegexMatch = regexec(&ipRegex, s_ip, 0, NULL, 0);
                                    if(!ipRegexMatch)
                                    {
                                        if(strlen(nick) > 0)
                                        {
                                            printf("nick size %d\n",strlen(nick));
                                            selectTeam = true;
                                        }
                                        else
                                        {
                                            message.setString("Please enter your nickname");
                                            message.setPosition(270,520);
                                        }
                                    }
                                    else
                                    {
                                        message.setString("Invalid server IP");
                                        message.setPosition(325,520);
                                    }
                                }
                                else
                                {
                                    message.setString("Invalid client IP");
                                    message.setPosition(325,520);
                                }
                            }
                            break;
                    }
                default:
                    break;
            }
        }

        ipConf[0].setString(c_ip);
        ipConf[1].setString(s_ip);
        ipConf[2].setString(nick);
        window.clear();

        window.draw(mainTitle);
        for(int i =0; i < 7;i++)
        {
            ipConf[i].setColor(sf::Color::White);
            if(i < 3)
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
            if(selectedOption == 3 && i == selectedOption)
            {
                menuIcon.setPosition(ipConf[i].getPosition().x - 55, ipConf[i].getPosition().y + 2);
                window.draw(menuIcon);
            }
            window.draw(ipConf[i]);

        }


        if(this->connResult == -1)
        {
            message.setString("Timeout: could not connect to server :(");
            message.setColor(sf::Color::Red);
            message.setPosition(230,520);

        }
        else if(this->connResult == 1)
        {
            message.setString("Connection Successful :D");
            message.setColor(sf::Color::White);
            message.setPosition(230,520);
        }
        else if(this->connResult == 3)
        {
            message.setString("Error: the arena is full, try again later :(");
            message.setColor(sf::Color::Red);
            message.setPosition(230,520);
        }
        else if(this->connResult == 4)
        {
            message.setString("Error: Could not reuse client IP Address");
            message.setColor(sf::Color::Red);
            message.setPosition(240,520);
        }
        else if(this->connResult == 5)
        {
            message.setString("Error: Could not reuse client port");
            message.setColor(sf::Color::Red);
            message.setPosition(255,520);
        }
        else if(this->connResult == 6)
        {
            message.setString("Error: could not bind client socket");
            message.setColor(sf::Color::Red);
            message.setPosition(250,520);
        }

        window.draw(message);
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
    team[1].setTexture(teamTexture2);
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

void Menu::deleteLastChar()
{
    char * str = NULL;
    if(selectedOption == 0)
        str = c_ip;
    else if(selectedOption == 1)
        str = s_ip;
    else if(selectedOption == 2)
        str = nick;

    if(str != NULL && strlen(str) > 0)
    {
        str[strlen(str)-1] = '\0';
    }
}

void Menu::addChar(char c)
{
    char * str = NULL;
    int16_t max;
    if(selectedOption == 0)
    {
        str = c_ip;
        max = 15;
    }
    else if(selectedOption == 1)
    {
        str = s_ip;
        max = 15;
    }
    else if(selectedOption == 2)
    {
        str = nick;
        max = 6;
    }

    if(str != NULL && strlen(str) < max)
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
        if (this->connResult == 1 || this->connResult == 2) {
            if(this->connResult == 1)
            {
                Serialization::charsToShort(buffer, this->playersTeam[0], 2);
                Serialization::charsToShort(buffer, this->playersTeam[1], 4);
            }
            this->currentStage = 3;
        }

    }
}
