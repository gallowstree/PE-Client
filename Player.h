//
// Created by daniel on 5/08/16.
//

#ifndef TEST_CLIENT_PLAYER_H
#define TEST_CLIENT_PLAYER_H


#include <cstdint>
#include <SFML/Graphics.hpp>

class Player {
    public:
        int16_t playerID;
        sf::CircleShape circle;



        Player(int16_t playerID,float posx,float posy,sf::Color);
};

#endif //TEST_CLIENT_PLAYER_H
