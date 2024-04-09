/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 20:56:50
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 14:00:57
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

#define UDP_MAX_LEN 65507       // UDP sendto最大数据包长度
#define UDP_MAX_DATA_LEN 65505  // UDP 1个字节帧类型, 1个字节帧数量或帧序号

class UdpClient {
public:
    UdpClient(const char* ip, int port);
    ~UdpClient();

    uint32_t send(uint8_t* data, uint32_t len);
    int recv(uint8_t* data, uint32_t len);

private:
    int sockfd_;
    struct sockaddr_in servaddr;
};

#endif
