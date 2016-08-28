//
// Created by daniel on 5/08/16.
//

#ifndef TEST_CLIENT_PLAYER_H
#define TEST_CLIENT_PLAYER_H


#include <cstdint>
#include <SFML/Graphics.hpp>
#include "BoundingBox.h"
#include "Entity.h"

class Player : public Entity {
    public:
        int16_t playerID;
        bool valid = 0;
        float rotation = 0;
        sf::Sprite sprite;


        Player(int16_t playerID,float posx,float posy,sf::Texture &texture);
        void setTexture(const sf::Texture &texture);
};

#endif //TEST_CLIENT_PLAYER_H
