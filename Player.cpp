//
// Created by daniel on 5/08/16.
//

#include "Player.h"


Player::Player(int16_t playerID, int16_t team, float posx, float posy,sf::Texture &texture, const char * nick):
playerID(playerID),
team(team),
sprite(),
nick(nick)
{
    type = EntityType::Player_T;
    boundingBox = BoundingBox(posx, posy, 50, 50);
    sprite.setTexture(texture);
    nickText.setString(nick);
    healthWrapper.setSize(sf::Vector2f(40,2));
    healthWrapper.setOutlineColor(sf::Color::White);
    healthWrapper.setFillColor(sf::Color::Black);
    healthWrapper.setOutlineThickness(1);
}

void Player::setTexture(const sf::Texture &texture)
{
    sprite.setTexture(texture);
}
