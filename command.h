//
// Created by daniel on 7/08/16.
//

#ifndef TEST_CLIENT_COMMAND_H
#define TEST_CLIENT_COMMAND_H


#include <cstdint>

typedef struct command {
    int32_t msgNum;
    int16_t playerID;
    int16_t bulletID;
    int16_t bulletType;
    int16_t type;
    int16_t team;
    bool validPlayer;
    float posx;
    float posy;
    float rotation;
    float originx;
    float originy;
} command_t;


#endif //TEST_CLIENT_COMMAND_H
