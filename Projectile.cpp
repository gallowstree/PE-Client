//
// Created by daniel on 8/08/16.
//

#include <zconf.h>
#include "Projectile.h"

Projectile::Projectile(int16_t bulletID, int16_t type,  sf::Vector2f position,sf::Vector2f origin):
bulletID(bulletID),
type(type),
position(position),
origin(origin),
valid(true),
projectile(sf::Vector2f(6,6))
{
    velocity = sf::Vector2f(speed * cosf(angle), speed * sinf(angle));
    projectile.setFillColor(sf::Color::Cyan);
    projectile.setRotation(angle);
    projectile.setPosition(position);
}

void  Projectile::update  (sf::Time elapsedTime)
{
    position += velocity * elapsedTime.asSeconds();
    projectile.setPosition(position);
    //printf("position: %f, %f\n", position.x, position.y);

    sf::Vector2f distance = position - origin;

    if (sqrtf(distance.x*distance.x + distance.y*distance.y) >= range)
    {
        valid = false;
        //printf("%d invalid!!!!!!!!!!!!\n",bulletID);
    }
}