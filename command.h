//
// Created by daniel on 7/08/16.
//

#ifndef TEST_CLIENT_COMMAND_H
#define TEST_CLIENT_COMMAND_H


#include <cstdint>

typedef struct command {
    int16_t playerId;
    int16_t command_type;
    int32_t msgNum;
    int32_t posx;
    int32_t posy;
} command_t;


#endif //TEST_CLIENT_COMMAND_H
