//
// Created by Alejandro on 1/09/16.
//

#include "Pickup.h"
#include "Textures.h"

Pickup::Pickup(int l, int t, int w, int h, int type, int seconds, int initialState) {
    boundingBox.top = t;
    boundingBox.left = l;
    boundingBox.height = h;
    boundingBox.width = w;
    pickupId = nextPickupId++;
    isStatic = true;
    enabled = initialState == 1;
    this->type = Pickup_T;
    pickupType = type == 0 ? Ammo_T : Health_T;
}

void Pickup::initSprite(sf::Texture &texture)
{
    this->sprite = sf::Sprite(texture);
    sprite.setTextureRect(sf::IntRect(0,
                                      0,
                                      boundingBox.width,
                                      boundingBox.height));

    sprite.setPosition(boundingBox.left, boundingBox.top);
}

Textures Pickup::getTexture() {
    return pickupType == 0 ? Textures ::PICKUP_AMMO : Textures ::PICKUP_HEALTH;
}

