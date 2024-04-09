/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-08 23:23:17
 * @Description: udp通信客户端
 */

#include<errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#include "udp_server.h"

UdpServer::UdpServer(int port)
{
    bzero(&server_addr_, sizeof(server_addr_));
    server_addr_.sin_family      = AF_INET;
    server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr_.sin_port        = htons(port);

    bzero(&(client_addr_.sin_zero),8);

    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0)
    {
        printf("socket fail:%s\n", strerror(errno));
    }

    int rcvbuf_size = 1024 * 1024;
    setsockopt(sockfd_, SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, sizeof(rcvbuf_size));

    if (bind(sockfd_, (struct sockaddr *)(&server_addr_), sizeof(server_addr_)) < 0)
    {
        printf("bind fail:%s\n", strerror(errno));
    }

    printf("udp server start, port: %d\n", port);
}

UdpServer::~UdpServer()
{
    close(sockfd_);
}

int UdpServer::send(const uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return -1;
    }

    return sendto(sockfd_, data, len, 0, (struct sockaddr *)&client_addr_, client_addrLen_);
}

int UdpServer::recv(uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return -1;
    }

    printf("want recv data len: %d\n", len);
    return recvfrom(sockfd_, data, len, 0, (struct sockaddr *)&client_addr_, &client_addrLen_);
}
