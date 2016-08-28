//
// Created by Alejandro on 28/08/16.
//

#ifndef TEST_CLIENT_FLOORSECTION_H
#define TEST_CLIENT_FLOORSECTION_H


#include "Entity.h"
#include "World.h"

class FloorSection : public Entity
{
public:
    Textures texture;
    FloorSection(Textures texture);

};


#endif //TEST_CLIENT_FLOORSECTION_H
