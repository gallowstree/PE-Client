//
// Created by daniel on 5/08/16.
//

#include "Player.h"


Player::Player(int16_t playerID, float posx, float posy, sf::Color color):
playerID(playerID),
circle(10)
{
    circle.setPosition(posx,posy);
    circle.setFillColor(color);
}