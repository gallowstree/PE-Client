//
// Created by daniel on 15/08/16.
//

#include "Wall.h"

Wall::Wall(float x, float y, float width, float height, sf::Texture & texture)
{
    boundingBox.top = y;
    boundingBox.left = x;
    boundingBox.height = height;
    boundingBox.width = width;


    isStatic = true;

    type = EntityType::Wall_T;

    this->sprite = sf::Sprite();
    sprite.setTextureRect(sf::IntRect(boundingBox.left,
                                      boundingBox.top,
                                      boundingBox.width,
                                      boundingBox.height));
    sprite.setPosition(boundingBox.left, boundingBox.top);
    sprite.setTexture(texture);
}
