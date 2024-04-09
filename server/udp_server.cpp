/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 15:07:43
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

/**
 * @brief 包头类型
 */
enum {
    PACKET_FIRST = 0xF1,        // 第一个包
    PACKET_INTERMEDATE = 0xF2,  // 中间包
};

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

uint32_t UdpServer::send(const uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return -1;
    }

    return sendto(sockfd_, data, len, 0, (struct sockaddr *)&client_addr_, client_addrLen_);
}

uint32_t UdpServer::recv(uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return 0;
    }

    int ret = -1;
    uint32_t recvedLen = 0; // 已接收
    uint32_t sequence = 1;
    uint32_t sequence_max = 0; // 最大序列号

    while (1)
    {
        ret = recvfrom(sockfd_, data+recvedLen, len-recvedLen, 0, (struct sockaddr *)&client_addr_, &client_addrLen_);
        if (ret < 2) // 至少收到包头两个字节
        {
            printf("recvfrom fail, errno: %d\n", errno);
            return 0;
        }

        if (PACKET_FIRST == data[recvedLen])            // 第一个包
        {
            sequence = 2;
            sequence_max = data[recvedLen+1];

            // 去掉包头
            memmove(data, data+recvedLen+2, ret - 2);
            recvedLen = (ret - 2);
            // printf("JH --- PACKET_FIRST recvedLen[%u] sequence_max[%u]\n", recvedLen, sequence_max);
        }
        else if (PACKET_INTERMEDATE == data[recvedLen])  // 中间包
        {
            if (sequence != data[recvedLen+1])
            {
                // printf("recv invalid sequence: %d, expected: %d\n", data[recvedLen+1], sequence);
                return 0;
            }

            // 去掉包头
            memmove(data+recvedLen, data+recvedLen+2, ret - 2);
            recvedLen += (ret - 2);
            // printf("JH --- PACKET_INTERMEDATE recvedLen[%u] sequence[%u]\n", recvedLen, sequence);

            if (sequence_max == sequence)
            {
                // printf("recv all data, sequence_max: %d, sequence: %d\n", sequence_max, sequence);
                return recvedLen;
            }

            sequence++;
        }
        else
        {
            printf("recv invalid packet type: %d\n", data[recvedLen]);
            return 0;
        }
    }
}
