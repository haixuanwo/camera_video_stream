/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 20:56:50
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 09:10:46
 * @Description: file content
 */

#ifndef UDP_CLIENT_H_
#define UDP_CLIENT_H_

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

class UdpClient {
public:
    UdpClient(const char* ip, int port);
    ~UdpClient();

    int send(const uint8_t* data, uint32_t len);
    int recv(uint8_t* data, uint32_t len);

private:
    int sockfd_;
    struct sockaddr_in servaddr;
};

#endif
