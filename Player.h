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
        const char * nick;
        sf::Text nickText;
        sf::RectangleShape healthWrapper;
        sf::RectangleShape healthBox;
        int16_t health = 100;

        Player(int16_t playerID,float posx,float posy,sf::Texture &texture, const char* nick);
        void setTexture(const sf::Texture &texture);
};

#endif //TEST_CLIENT_PLAYER_H
