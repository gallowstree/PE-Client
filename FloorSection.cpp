//
// Created by Alejandro on 28/08/16.
//

#include "FloorSection.h"

FloorSection::FloorSection(int texture, float left, float top, float width, float height) :
texture(texture)
{
    printf("texture %i\n", texture);
    this->type = EntityType::FloorSection_T;
    this->boundingBox = BoundingBox(left, top, width, height);
}

Textures FloorSection::getTexture() {
    return texture == 1 ? Textures ::FLOOR_PURPLE_CHESS : Textures ::FLOOR_BLUE_BRICK;
}




