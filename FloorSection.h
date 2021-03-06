//
// Created by Alejandro on 28/08/16.
//

#ifndef TEST_CLIENT_FLOORSECTION_H
#define TEST_CLIENT_FLOORSECTION_H


#include "Entity.h"
#include "Textures.h"
#include "World.h"


class FloorSection : public Entity
{
public:
    Textures getTexture();
    sf::Sprite sprite;
    FloorSection(int texture, float left, float top, float width, float height);


    void initSprite(sf::Texture &texture);

private:
    int texture;

};


#endif //TEST_CLIENT_FLOORSECTION_H
