//
// Created by daniel on 20/08/16.
//

#include "ClientSocket.h"
#include "serialization.h"
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <errno.h>

ClientSocket::ClientSocket(const char * ip, int port, SocketListener * listener):
ip(ip),
port(port),
listener(listener),
tuputamadre(true)
{
    printf("%d hola\n",tuputamadre);
}

void ClientSocket::run()
{
    /*Create UDP socket*/
    int udpSocket, COMMAND_BUFFER_SIZE = 1500;
    ssize_t nBytes;
    char buffer[COMMAND_BUFFER_SIZE];
    struct sockaddr_in clientAddr,serverAddr;
    struct sockaddr_storage clientStorage;
    udpSocket = socket(PF_INET, SOCK_DGRAM, 0);
    /*Configure settings in address struct*/
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(port);
    clientAddr.sin_addr.s_addr = inet_addr(ip);
    memset(clientAddr.sin_zero, '\0', sizeof clientAddr.sin_zero);


    /*Bind socket with address struct*/
    bind(udpSocket, (struct sockaddr *) &clientAddr, sizeof(clientAddr));

    /*Initialize size variable to be used later on*/
    socklen_t addr_size = sizeof serverAddr;
    printf("fuck this shit %d\n",tuputamadre);

    while(tuputamadre)
    {

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0; //1 segundo de timeout
        if (setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
            printf("Error seteando timeout %s",strerror(errno));
        }

        if(recvfrom(udpSocket, buffer, COMMAND_BUFFER_SIZE, 0, (struct sockaddr *)&serverAddr, &addr_size) < 0)
        {
            int16_t offset = 0;
            Serialization::shortToChars(-1,buffer,offset);
            printf("timeout!");
            //keepListening=false;
        }

        if (listener != nullptr) {
            listener->receiveMessage(buffer, nBytes, &serverAddr);
        }

    }
    printf("bye bye");
}

void *ClientSocket::runThread(void *serverSocket) {
    ((ClientSocket*) serverSocket)->run();
    return nullptr;
}