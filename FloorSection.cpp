//
// Created by Alejandro on 28/08/16.
//

#include "FloorSection.h"

FloorSection::FloorSection(int texture, float left, float top, float width, float height)
{
    this->texture = texture;
    this->type = EntityType::FloorSection_T;
    this->boundingBox = BoundingBox(left, top, width, height);
}

Textures FloorSection::getTexture() {
    return texture == 1 ? Textures ::FLOOR_2 : Textures ::FLOOR_W00D;
}

void FloorSection::initSprite(sf::Texture &texture)
{
    this->sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect(boundingBox.left,
                                      boundingBox.top,
                                      boundingBox.width,
                                      boundingBox.height));
    sprite.setPosition(boundingBox.left, boundingBox.top);
}





