//
// Created by Alejandro on 1/09/16.
//

#ifndef TEST_SERVER_PICKUP_H
#define TEST_SERVER_PICKUP_H


#include "Entity.h"
#include "Textures.h"

enum PickupType
{
    Ammo_T,
    Health_T,
};

class Pickup : public Entity
{
public:
    Pickup(int l, int t, int w, int h, int type, int seconds, int initialState, int16_t id);
    PickupType pickupType;
    int16_t pickupId;
    bool enabled;

    void initSprite(sf::Texture &texture);

    sf::Sprite sprite;

    Textures getTexture();
};


#endif //TEST_SERVER_PICKUP_H
