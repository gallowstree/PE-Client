//
// Created by daniel on 15/08/16.
//

#ifndef GAMEPLAY_PROTOTYPE_WALL_H
#define GAMEPLAY_PROTOTYPE_WALL_H


#include "Entity.h"
#include "Textures.h"

class Wall : public Entity
{
public:
    sf::Sprite sprite;

    Wall(float x, float y, float width, float height, sf::Texture & texture);
};


#endif //GAMEPLAY_PROTOTYPE_WALL_H