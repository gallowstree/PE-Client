//
// Created by daniel on 8/08/16.
//

#include <zconf.h>
#include <math.h>
#include "Projectile.h"
#include "Player.h"

Projectile::Projectile(int16_t bulletID, int16_t type,  sf::Vector2f position,sf::Vector2f origin, int16_t playerId):
bulletID(bulletID),
type(type),
position(position),
origin(origin),
valid(true),
playerId(playerId),
sprite()
{
    boundingBox = BoundingBox(position.x, position.y, 6, 6);
    sf::Vector2f facing = position - origin;
    float rads = atan2f(facing.y, facing.x);
    velocity = sf::Vector2f(speed * cosf(rads), speed * sinf(rads));
    //sprite.setFillColor(sf::Color::Yellow);
    angle  = rads * 180/M_PI;
    sprite.setRotation(angle);
    sprite.setPosition(position);
}

void  Projectile::update  (sf::Time elapsedTime)
{
    position += velocity * elapsedTime.asSeconds();
    sprite.setPosition(position);
    boundingBox.left = position.x;
    boundingBox.top = position.y;

    sf::Vector2f distance = position - origin;

    if (sqrtf(distance.x*distance.x + distance.y*distance.y) >= range)
    {
        valid = false;
    }
}

void Projectile::intersectedWith(Entity *other, sf::FloatRect intersection)
{
    if (other->type == EntityType::Player_T)
    {
        if (((Player*) other)->playerID == this->playerId)
            return;
    }

    valid = false;
}
