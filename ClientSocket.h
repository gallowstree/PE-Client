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

public:

    bool tuputamadre ;
    SocketListener * listener;
    ClientSocket(const char * ip, int port, SocketListener * listener);
    void run();

    static void * runThread(void * clientSocket);

private:
    const char * ip;
    int port;
};


#endif //TEST_CLIENT_CLIENTSOCKET_H
