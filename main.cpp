#include <stdio.h>
#include <pthread.h>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <math.h>
#include "command.h"
#include "Player.h"
#include "Projectile.h"
#include "SFUtils.h"
#include "ResourceHolder.h"
#include "Area.h"
#include "Menu.h"
#include "Wall.h"
#include <fstream>
#include <zconf.h>
#include "serialization.h"

enum Textures
{
    CROSSHAIR,
    PLAYER_RED_SG,
    WALL
};


int moves = 0;
sf::View camera;
sf::Vector2f camCenter;
sf::FloatRect bounds(0,0,2400.0f, 2400.0f);
std::vector<std::vector<Entity*>> static_objects;
std::vector<Entity> world_entities;
std::vector<Area*> areas;
std::vector<const char *> maps = {"maps/level1.txt","maps/level2.txt"};
int msgnm = 0, lastServerMsgid = -1;
sf::Sprite cursorSprite;

/*************************NO TOCAR***************************/
static const int COMMAND_BUFFER_SIZE = 1500;
char buffer[COMMAND_BUFFER_SIZE];
/************************************************************/

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
const int16_t s_are_you_there = 3;

char * c_ip;
char * s_ip;
const int s_port = 50420;
const int c_port = 50421;
ResourceHolder<sf::Texture, Textures> textureHolder;

const int COMMAND_DATA_SIZE= 1400;
char outputBuff[COMMAND_DATA_SIZE];
char inputBuff[COMMAND_DATA_SIZE];
struct sockaddr_in serverAddr;

void debug_drawPlayerBoundingBox(const Player &player);

socklen_t addr_size;
int msgid;
float rotation;

sf::RenderWindow mWindow(sf::VideoMode(800, 600), "President Evil", sf::Style::Close);
const sf::Time TimePerFrame = sf::seconds(1.f/30.f);
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
    Serialization::shortToChars(c_join_game_command,outputBuff,offset);
    offset += 2;
    Serialization::shortToChars(-1,outputBuff,offset);
    sendto(clientSocket,outputBuff,COMMAND_DATA_SIZE,0,(struct sockaddr *)&serverAddr,addr_size);
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
    sf::FloatRect visibleRect(camCenter.x - mWindow.getSize().x, camCenter.y - mWindow.getSize().y, mWindow.getSize().x * 2, mWindow.getSize().y * 2);

    for (auto &area : areas)
    {
        if (area->rect.intersects(visibleRect))
        {
            area->draw(mWindow, true);
        }
    }

    for (const auto &staticEntity : world_entities)
    {
        auto drawRect = sf::RectangleShape(sf::Vector2f(staticEntity.boundingBox.width, staticEntity.boundingBox.height));
        drawRect.setPosition(staticEntity.boundingBox.left, staticEntity.boundingBox.top);
        drawRect.setFillColor(sf::Color(144,144,144));

        mWindow.draw(drawRect);
    }

    mWindow.setView(camera);
    camera.setCenter(camCenter);

    for(auto const &projectile : projectiles)
    {
        if(projectile.second.valid)
            mWindow.draw(projectile.second.projectile);
    }

    for (auto &player : players)
    {
        debug_drawPlayerBoundingBox(player);
        double angle = player.rotation * 180/M_PI;
        sf::IntRect spriteRect = player.sprite.getTextureRect();
        player.sprite.setPosition(player.boundingBox.left + player.boundingBox.width/2, player.boundingBox.top + player.boundingBox.height/2);
        player.sprite.setOrigin(sf::Vector2f(spriteRect.width/ 2, spriteRect.height / 2));
        player.sprite.setRotation(angle);
        mWindow.draw(player.sprite);

    }
    mWindow.draw(cursorSprite);
    mWindow.display();
}

void debug_drawPlayerBoundingBox(const Player &player) {
/*    auto bb = sf::RectangleShape(sf::Vector2f(player.boundingBox.width, player.boundingBox.height));
    bb.setPosition(player.boundingBox.getPosition());
    bb.setFillColor(sf::Color::Green);
    mWindow.draw(bb);*/
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
            players[command.playerID].boundingBox.left = command.posx;
            players[command.playerID].boundingBox.top = command.posy;
            players[command.playerID].rotation = command.rotation;
        }
        else if (command.type == s_projectile_command)
        {
            sf::Vector2f position = sf::Vector2f(command.posx,command.posy);
            sf::Vector2f origin = sf::Vector2f(command.originx,command.originy);
            Projectile projectile = Projectile(command.bulletID,command.bulletType,position,origin);
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
    Serialization::shortToChars(c_input_command,outputBuff,offset);
    offset+=2;
    Serialization::shortToChars(playerID,outputBuff,offset);
    offset+=2;
    Serialization::intToChars(msgnm,outputBuff,offset);
    offset+=4;
    Serialization::intToChars(moves,outputBuff,offset);
    offset+=4;
    Serialization::floatToChars(rotation,outputBuff,offset);
    offset+=4;
    pthread_mutex_lock(&projectileACKMutex);
    Serialization::intToChars((int)projectileACKQueue.size(), outputBuff, offset);
    offset+=4;

    while(!projectileACKQueue.empty())
    {
        Serialization::shortToChars(s_projectile_command,outputBuff,offset);
        offset+=2;
        int32_t ack = projectileACKQueue.front();
        projectileACKQueue.pop();
        Serialization::intToChars(ack,outputBuff,offset);
        offset+=4;
    }
    pthread_mutex_unlock(&projectileACKMutex);
    Serialization::shortToChars(-1,outputBuff,offset);
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
        Serialization::charsToInt(inputBuff,msgNum,offset);
        lastServerMsgid = msgid;
        int16_t command_type;
        offset += 4;
        Serialization::charsToShort(inputBuff, command_type, offset);
        offset += 2;
        bool eom = false;

        if(command_type == s_players_command) {
            if(msgNum > lastServerMsgid)
            {
                do {
                    command srvCmd;
                    srvCmd.msgNum = msgNum;
                    srvCmd.type = command_type;
                    Serialization::charsToShort(inputBuff, srvCmd.playerID, offset);
                    if (srvCmd.playerID != -1)
                    {
                        offset += 2;
                        Serialization::charsToFloat(inputBuff, srvCmd.posx, offset);
                        offset += 4;
                        Serialization::charsToFloat(inputBuff, srvCmd.posy, offset);
                        offset += 4;
                        Serialization::charsToFloat(inputBuff, srvCmd.rotation, offset);
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
                Serialization::charsToShort(inputBuff, srvCmd.bulletID, offset);
                if (srvCmd.bulletID != -1)
                {
                    offset += 2;
                    Serialization::charsToShort(inputBuff, srvCmd.bulletType, offset);
                    offset += 2;
                    Serialization::charsToFloat(inputBuff, srvCmd.posx, offset);
                    offset += 4;
                    Serialization::charsToFloat(inputBuff, srvCmd.posy, offset);
                    offset += 4;
                    Serialization::charsToFloat(inputBuff, srvCmd.originx, offset);
                    offset += 4;
                    Serialization::charsToFloat(inputBuff, srvCmd.originy, offset);
                    offset += 4;

                    pthread_mutex_lock(&commandQueueMutex);
                    if(projectiles.count(srvCmd.bulletID) == 0)
                    {
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
            Serialization::charsToShort(inputBuff, srvCmd.playerID, offset);
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

std::vector<int16_t> areasForEntity(const Entity &entity)
{
    std::vector<int16_t> areaslocal;

    int i = 0;
    for (auto &area : areas)
    {

        if (area->rect.intersects(entity.boundingBox))
        {
            areaslocal.push_back(i);
        }

        i++;
    }
    //printf("\n");
    return areaslocal;
}

int parseMapParameter(std::string & line)
{
    auto commaPos = line.find(',');
    char * parameter = (char *)malloc((commaPos+1)*sizeof(char));
    strcpy(parameter,line.substr(0,commaPos).c_str());
    line.erase(0, line.find(',') + 1);
    int value = atoi(parameter);
    free(parameter);
    return value;
}

void readMap(int map)
{
    std::ifstream mapFile(maps[map]);
    std::string line;

    while (std::getline(mapFile, line))
    {
        int objectType = parseMapParameter(line);
        int left =  parseMapParameter(line);
        int top =  parseMapParameter(line);
        int width =   parseMapParameter(line);
        int height =  atoi(line.c_str());

        if (objectType == 0) //wall
            world_entities.push_back(Wall(left, top, width, height));

    }
}
void createStaticObjects()
{
    readMap(0);

    for (auto& entity : world_entities)
    {
        if (entity.isStatic)
        {
            for (auto& area : areasForEntity(entity))
            {
                static_objects[area].push_back(&entity);
            }
        }
    }

}

void init()
{
    loadTextures();

    camera.reset(sf::FloatRect(0,0, mWindow.getSize().x, mWindow.getSize().y));
    camera.setViewport(sf::FloatRect(0,0, 1.0f, 1.0f));

    int noAreasX = 0;
    int noAreasY = 0;

    float area_size = 400;

    noAreasX = bounds.width / area_size;
    noAreasY = bounds.height / area_size;

    for (int x = 0; x < noAreasX; x++)
    {
        for (int y = 0; y < noAreasY; y++)
        {
            Area* newArea = new Area(x*area_size, y*area_size, area_size, area_size);
            areas.push_back(newArea);
            static_objects.push_back(std::vector<Entity*>());
        }
    }

    createStaticObjects();

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

void launchMenu()
{

}

int main()
{

    /*****************MENU******************/
    Menu menu(mWindow);
    menu.run(1);
    loopStage2:
    menu.run(2);
    ServerSocket sSocket(menu.s_ip, 50420);
    ClientSocket cSocket(menu.c_ip, 50421, &menu);
    cSocket.timeout = 2;
    pthread_t listening_thread;
    pthread_create(&listening_thread, nullptr, &ClientSocket::runThread, &cSocket);
    Serialization::shortToChars(3, buffer, 0);
    sSocket.send(buffer, COMMAND_BUFFER_SIZE);
    sleep(2);
    if(menu.currentStage == 2)
        goto loopStage2;

    cSocket.timeout = 0;
    menu.run(3);
    /*************END MENU***************/
    //Serialization::shortToChars(s_are_you_there, buffer, 0);
    //sSocket.send(buffer, COMMAND_BUFFER_SIZE);
    /*init();
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
*/
    return 0;
}

