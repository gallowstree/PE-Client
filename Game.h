//
// Created by daniel on 21/08/16.
//

#ifndef TEST_CLIENT_GAME_H
#define TEST_CLIENT_GAME_H


#include "ClientSocket.h"
#include "Player.h"
#include "command.h"
#include "Projectile.h"
#include "ServerSocket.h"
#include "World.h"

class Game : public SocketListener {
    static const int COMMAND_BUFFER_SIZE = 1500;

public:
    int playerID = -1;
    sf::RenderWindow&  window;
    Game(sf::RenderWindow&  window,ServerSocket * sSocket, int selectedTeam);
    std::vector<Player> players;
    ServerSocket * sSocket;
    bool restart = true;
    int selectedTeam = 0;
    void receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr);
    void run();
    void getID();

private:
    World world;
    const sf::Time TimePerFrame = sf::seconds(1.f/30.f);
    const sf::Time TimePerProjectileUpdate = sf::seconds(1.f/60.f);
    int lastServerMsgid = -1;
    pthread_mutex_t commandQueueMutex;
    pthread_mutex_t projectileACKMutex;
    std::queue<command_t> commandQueue;
    std::queue<int32_t> projectileACKQueue;
    std::map<int16_t,Projectile> projectiles;
    char buffer[COMMAND_BUFFER_SIZE];
    int moves = 0;
    int msgnm = 0;
    bool gameOver = false;
    int16_t gameOverCode = 0;
    float rotation = 0;

    void sendCommands();
    void processServerEvents();
    void requestJoinGame();
    void render();
    void processEvents();

};

#endif //TEST_CLIENT_GAME_H
