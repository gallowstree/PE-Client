//
// Created by daniel on 15/08/16.
//

#ifndef GAMEPLAY_PROTOTYPE_AREA_H
#define GAMEPLAY_PROTOTYPE_AREA_H


#include <SFML/Graphics.hpp>
#include "Entity.h"

class Area
{
public:
    Area(float x, float y, float width, float height);
    void draw(sf::RenderTarget &window, bool debugGrid);
    sf::FloatRect rect;
    std::vector<Entity*> walls;
    std::vector<Entity*> pickups;
    std::vector<Entity*> floors;
    std::vector<Entity*> moving_entities;
};


#endif //GAMEPLAY_PROTOTYPE_AREA_H
