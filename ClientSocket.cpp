//
// Created by daniel on 20/08/16.
//

#include "ClientSocket.h"
#include "serialization.h"
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <errno.h>


ClientSocket::ClientSocket(const char * ip, int port, SocketListener * listener)
{
    this->ip = ip;
    this->port = port;
    this->listener = listener;
}

void ClientSocket::run()
{

    int udpSocket, COMMAND_BUFFER_SIZE = 1500;
    ssize_t nBytes = 0;
    char buffer[COMMAND_BUFFER_SIZE];
    struct sockaddr_in clientAddr,serverAddr;
    struct sockaddr_storage clientStorage;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);


    bind(udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr));

    socklen_t addr_size = sizeof serverAddr;

    while(keepAlive)
    {

        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0; //1 segundo de timeout
        if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            printf("Error seteando timeout %s\n",strerror(errno));
        }

        if(recvfrom(udpSocket, buffer, COMMAND_BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addr_size) < 0)
        {
            Serialization::shortToChars(-1,buffer,0);
            keepAlive=false;
        }

        if (listener != nullptr) {
            listener->receiveMessage(buffer, nBytes, &serverAddr);
        }
    }
}

void *ClientSocket::runThread(void *clientSocket) {
    ((ClientSocket*) clientSocket)->run();
    return nullptr;
}