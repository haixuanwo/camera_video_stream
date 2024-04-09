/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 14:01:09
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

/**
 * @brief 包头类型
 */
enum {
    PACKET_FIRST = 0xF1,        // 第一个包
    PACKET_INTERMEDATE = 0xF2,  // 中间包
};

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

uint32_t UdpClient::send(uint8_t* data, uint32_t len)
{
    if (sockfd_ < 0 || nullptr == data || 0 == len)
    {
        printf("%s: invalid parameters\n", __func__);
        return 0;
    }

    uint32_t sequence = 0;
    uint32_t sendLen = -1;
    uint32_t sendedLen = 0;
    while (sendedLen < len)
    {
        sequence++;
        sendLen = (len - sendedLen) > UDP_MAX_DATA_LEN ? UDP_MAX_DATA_LEN : (len - sendedLen);

        if (sendedLen == 0) // 第一个包，类型+总包数+数据
        {
            *(data - 2) = PACKET_FIRST;
            *(data - 1) = len/UDP_MAX_DATA_LEN;
            if (len%UDP_MAX_DATA_LEN)
            {
                *(data - 1) += 1;
            }
        }
        else                // 中间包，类型+包序号+数据
        {
            *(data + sendedLen - 2) = PACKET_INTERMEDATE;
            *(data + sendedLen - 1) = sequence;
        }

        int ret = sendto(sockfd_, data + sendedLen - 2, sendLen + 2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        if (ret < 0)
        {
            printf("sendto fail, errno: %d\n", errno);
            return 0;
        }
        sendedLen += (ret - 2); // 2 bytes for packet header

        printf("send[%d] bytes, sendedLen[%u] bytes sequence:[%u]\n", ret, sendedLen, sequence);
    }
    return sendedLen;
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

