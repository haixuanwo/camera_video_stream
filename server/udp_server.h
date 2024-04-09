/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 20:56:50
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-08 22:18:21
 * @Description: file content
 */

#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

class UdpServer {
public:
    UdpServer(int port);
    ~UdpServer();

    int send(const uint8_t* data, uint32_t len);
    int recv(uint8_t* data, uint32_t len);

private:
    int sockfd_;
    socklen_t client_addrLen_;
    struct sockaddr_in server_addr_;
    struct sockaddr_in client_addr_;
};

#endif
