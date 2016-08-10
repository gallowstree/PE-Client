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
    sf::Vector2f facing = position - origin;

    float rads = atan2f(facing.y, facing.x);
    printf("radianes %f\n",rads);
    velocity = sf::Vector2f(speed * cosf(rads), speed * sinf(rads));
    projectile.setFillColor(sf::Color::Yellow);
    angle  = rads * 180/M_PI;
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