/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 09:21:50
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
#include <stdio.h>

#include "udp_client.h"

#define PORT 9999

UdpClient::UdpClient(const char* ip, int port)
{
    sockfd_ = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd_ < 0)
    {
        printf("socket fail\n");
        // return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);
    printf("ip: %s, port: %d\n", ip, port);
}

UdpClient::~UdpClient()
{
    close(sockfd_);
}

int UdpClient::send(const uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return -1;
    }

    printf("send data len: %d\n", len);
    return sendto(sockfd_, data, len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
}

int UdpClient::recv(uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return -1;
    }

    return recvfrom(sockfd_, data, len, 0, nullptr, nullptr);
}

