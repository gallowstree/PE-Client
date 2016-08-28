//
// Created by daniel on 5/08/16.
//

#include "Player.h"


Player::Player(int16_t playerID, float posx, float posy,sf::Texture &texture, const char * nick):
playerID(playerID),
sprite(),
nick(nick)
{
    type = EntityType::Player_T;
    boundingBox = BoundingBox(posx, posy, 50, 50);
    sprite.setTexture(texture);
    nickText.setString(nick);
}

void Player::setTexture(const sf::Texture &texture)
{
    sprite.setTexture(texture);
}
