//
// Created by daniel on 7/08/16.
//

#ifndef TEST_CLIENT_COMMAND_H
#define TEST_CLIENT_COMMAND_H


#include <cstdint>

typedef struct command {
    int32_t msgNum;
    int16_t oid;
    int16_t type;
    float posx;
    float posy;
    float angle;
    float originx;
    float originy;
} command_t;


#endif //TEST_CLIENT_COMMAND_H
