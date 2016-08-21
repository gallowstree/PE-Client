//
// Created by daniel on 8/08/16.
//

#ifndef TEST_CLIENT_PROJECTILE_H
#define TEST_CLIENT_PROJECTILE_H


#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>
#include "Entity.h"

class Projectile : public Entity {
    public:
    int16_t bulletID;
    int16_t playerId;
    int16_t type;
    sf::Vector2f position;
    sf::Vector2f origin;
    bool  valid;
    sf::RectangleShape projectile;

    Projectile(int16_t bulletID,int16_t type,  sf::Vector2f position,sf::Vector2f origin, int16_t playerId);
    void update(sf::Time elapsedTime);
    void intersectedWith(Entity *other, sf::FloatRect intersection);

    private:
        sf::Vector2f velocity;
        float range = 700;
        float speed = 800;
        float angle;


};


#endif //TEST_CLIENT_PROJECTILE_H
