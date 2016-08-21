//
// Created by daniel on 20/08/16.
//

#ifndef TEST_CLIENT_SERVERSOCKET_H
#define TEST_CLIENT_SERVERSOCKET_H

#include <netinet/in.h>
#include <arpa/inet.h>

class ServerSocket {

public:
    ServerSocket(const char * ip, int port);
    void send(char buffer[], int COMMAND_DATA_SIZE);

private:
    const char * ip;
    int port;
    int serverSocket;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size = sizeof serverStorage;


};


#endif //TEST_CLIENT_SERVERSOCKET_H
