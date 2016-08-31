//
// Created by daniel on 20/08/16.
//

#ifndef TEST_CLIENT_CLIENTSOCKET_H
#define TEST_CLIENT_CLIENTSOCKET_H

#include <netinet/in.h>

class SocketListener
{
public:
    virtual void receiveMessage(char buffer[], size_t nBytes, sockaddr_in* serverAddr) = 0;
};


class ClientSocket {
    int static const COMMAND_BUFFER_SIZE = 1500;
public:
    int udpSocket = 0;

    char buffer[COMMAND_BUFFER_SIZE];
    int keepAlive = 1 ;
    int timeout = 0;
    int isListening = 0;
    const char * ip;
    struct sockaddr_in clientAddr,serverAddr;
    socklen_t addr_size;
    int port;
    SocketListener * listener;
    ClientSocket(const char * ip, int port, SocketListener * listener);
    void run();

    static void * runThread(void * clientSocket);

};


#endif //TEST_CLIENT_CLIENTSOCKET_H
