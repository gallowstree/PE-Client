//
// Created by daniel on 5/08/16.
//

#include "Player.h"


Player::Player(int16_t playerID, float posx, float posy,sf::Texture &texture):
playerID(playerID),
sprite()
{
    sprite.setPosition(posx,posy);
    sprite.setTexture(texture);
}

void Player::setTexture(const sf::Texture &texture)
{
    sprite.setTexture(texture);
}
