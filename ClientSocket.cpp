//
// Created by daniel on 20/08/16.
//

#include "ClientSocket.h"
#include "serialization.h"
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>
#include <zconf.h>


ClientSocket::ClientSocket(const char * ip, int port, SocketListener * listener)
{
    this->ip = ip;
    this->port = port;
    this->listener = listener;

    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);

    int reuse = 1;


    if (setsockopt(udpSocket,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))) {
        Serialization::shortToChars(4,buffer,0);
        if (listener != nullptr) {
            listener->receiveMessage(buffer, 0, &serverAddr);
        }
    }

    if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int))) {
        Serialization::shortToChars(5,buffer,0);
        if (listener != nullptr) {
            listener->receiveMessage(buffer, 0, &serverAddr);
        }
    }


    isListening = bind(udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) + 1; //bind retorna 0 si es exitoso por eso se le suma 1 para que isListening sea 1 en bind exitoso
    if(!isListening )
    {
        isListening = 0;
        Serialization::shortToChars(6,buffer,0);
        if (listener != nullptr) {
            listener->receiveMessage(buffer, 0, &serverAddr);
        }
    }

    addr_size = sizeof serverAddr;

}

void ClientSocket::run()
{

    do
    {
        ssize_t nBytes = 0;

        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0; //1 segundo de timeout

        if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            printf("Error seteando timeout %s\n",strerror(errno));
        }


        nBytes = recvfrom(udpSocket, buffer, COMMAND_BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addr_size);

        if(nBytes < 0)
        {
            Serialization::shortToChars(-1,buffer,0);
        }

        if (listener != nullptr) {
            listener->receiveMessage(buffer, nBytes, &serverAddr);
        }
    }
    while(keepAlive);
    close(udpSocket);
}

void *ClientSocket::runThread(void *clientSocket) {
    ((ClientSocket*) clientSocket)->run();
    return nullptr;
}