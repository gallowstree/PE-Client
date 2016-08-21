//
// Created by daniel on 20/08/16.
//

#include <cstring>
#include <cstdio>
#include <errno.h>
#include "ServerSocket.h"

ServerSocket::ServerSocket(const char *ip, int port) :
ip(ip),
port(port)
{
    serverSocket = socket(PF_INET, SOCK_DGRAM, 0);

    /*Configure settings in address struct*/
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
}
void ServerSocket::send(char buffer[], int COMMAND_DATA_SIZE)
{
    sendto(serverSocket,buffer,COMMAND_DATA_SIZE,0,(struct sockaddr *)&serverAddr,addr_size);
}