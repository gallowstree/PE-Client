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
    void closeFD();

private:
    const char * ip;
    int port;
    int serverSocket;
    struct sockaddr_in serverAddr;
    socklen_t addr_size = sizeof serverAddr;

};


#endif //TEST_CLIENT_SERVERSOCKET_H
