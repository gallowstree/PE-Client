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
        float rotation = 0;
        sf::Sprite sprite;


        Player(int16_t playerID,float posx,float posy,sf::Texture &texture);
        void setTexture(const sf::Texture &texture);
};

#endif //TEST_CLIENT_PLAYER_H
