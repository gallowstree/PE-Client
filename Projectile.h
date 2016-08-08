//
// Created by daniel on 8/08/16.
//

#ifndef TEST_CLIENT_PROJECTILE_H
#define TEST_CLIENT_PROJECTILE_H


#include <SFML/Graphics.hpp>
#include <SFML/Graphics.hpp>

class Projectile {
    public:
        int16_t type;
        sf::Vector2f position;
        sf::Vector2f origin;
        bool  valid;
        sf::RectangleShape projectile;

        Projectile(int16_t type,  sf::Vector2f position,sf::Vector2f origin);
        void update(sf::Time elapsedTime);

    private:
        sf::Vector2f velocity;
        float range = 200;
        float speed = 500;
        float angle = 0;
};


#endif //TEST_CLIENT_PROJECTILE_H
