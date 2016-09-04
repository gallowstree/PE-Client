//
// Created by daniel on 15/08/16.
//

#ifndef GAMEPLAY_PROTOTYPE_AREA_H
#define GAMEPLAY_PROTOTYPE_AREA_H


#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "FloorSection.h"
#include "Pickup.h"
#include "Wall.h"

class FloorSection;

class Area
{
public:
    Area(float x, float y, float width, float height);
    void draw(sf::RenderTarget &window, bool debugGrid);
    sf::FloatRect rect;
    std::vector<Wall*> walls;
    std::vector<Pickup*> pickups;
    std::vector<FloorSection*> floors;
    std::vector<Entity*> moving_entities;
};


#endif //GAMEPLAY_PROTOTYPE_AREA_H
