/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2022-03-09 21:10:25
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-10 17:36:42
 * @Description: file content
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <string>

#define UDP_MAX_LEN 65507       // UDP sendto最大数据包长度
#define UDP_MAX_DATA_LEN 65505  // UDP 1个字节帧类型, 1个字节帧数量或帧序号

#define PORT 8888

/**
 * @brief 包头类型
 */
enum {
    PACKET_FIRST = 0xF1,        // 第一个包
    PACKET_INTERMEDATE = 0xF2,  // 中间包
};

// typedef enum{
//     TCP,
//     UDP,
// }tcpOrUdp_t;

typedef enum{
    SERVER,
    CLIENT,
}serverOrClient_t;

class Socket
{
public:
    Socket(const std::string &IP, unsigned short port, serverOrClient_t serverOrClient)
    {
        this->IP = IP;
        this->port = port;
        this->serverOrClient = serverOrClient;
    }

    bool init()
    {
        serverSocketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocketFd < 0)
        {
            perror("socket error");
            return false;
        }

        if (SERVER == serverOrClient)
        {
            struct sockaddr_in serverAddr;
            bzero(&serverAddr, sizeof(serverAddr));
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            serverAddr.sin_port = htons(port);

            int ret = bind(serverSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
            if (ret < 0)
            {
                perror("bind error");
                return false;
            }

            ret = listen(serverSocketFd, 10);
            if (ret < 0)
            {
                perror("listen error");
                return false;
            }

            printf("server run\n");
        }

        return true;
    }

    bool accept_client()
    {
        clientSocketFd = accept(serverSocketFd, (struct sockaddr*)nullptr, nullptr);
        if (clientSocketFd < 0)
        {
            printf("accept error\n");
            return false;
        }
        return true;
    }

    bool connect_server()
    {
        struct sockaddr_in serverAddr;
        bzero(&serverAddr, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        if (inet_pton(AF_INET, IP.c_str(), &serverAddr.sin_addr) <= 0)
        {
            printf("inet_pton error\n");
            return false;
        }

        if (connect(serverSocketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
        {
            perror("connect error\n");
            return false;
        }

        return true;
    }

    int get_client_fd()
    {
        return clientSocketFd;
    }

    int read(char *buf, int bufSize)
    {
        if (SERVER == serverOrClient)
        {
            return ::read(clientSocketFd, buf, bufSize);
        }
        else if(CLIENT == serverOrClient)
        {
            return ::read(serverSocketFd, buf, bufSize);
        }

        return -1;
    }

    int write(char *buf, int len)
    {
        if (SERVER == serverOrClient)
        {
            return ::write(clientSocketFd, buf, len);
        }
        else if(CLIENT == serverOrClient)
        {
            return ::write(serverSocketFd, buf, len);
        }

        return -1;
    }

    ssize_t readn(void *buf, size_t n)
    {
        size_t nleft = n;
        ssize_t nread;
        char *ptr = static_cast<char*>(buf);

        while (nleft > 0)
        {
            if (SERVER == serverOrClient)
            {
                nread = ::read(clientSocketFd, ptr, nleft);
            }
            else if(CLIENT == serverOrClient)
            {
                nread = ::read(serverSocketFd, ptr, nleft);
            }

            if (nread  < 0)
            {
                if (errno == EINTR) // 被中断的系统调用重新发起
                    nread = 0;
                else
                    return -1; // 出错
            }
            else if (nread == 0)
            {
                break; // EOF
            }

            nleft -= nread;
            ptr += nread;
        }
        return (n - nleft);
    }

    ssize_t writen(const void *buf, size_t n)
    {
        size_t nleft = n;
        ssize_t nwritten = 0;
        const char *ptr = static_cast<const char*>(buf);

        while (nleft > 0)
        {
            if (SERVER == serverOrClient)
            {
                nwritten = ::write(clientSocketFd, ptr, nleft);
            }
            else if(CLIENT == serverOrClient)
            {
                nwritten = ::write(serverSocketFd, ptr, nleft);
            }

            if (nwritten <= 0)
            {
                if (nwritten < 0 && errno == EINTR) // 被中断的系统调用重新发起
                {
                    nwritten = 0;
                }
                else
                {
                    return -1; // 出错
                }
            }

            nleft -= nwritten;
            ptr += nwritten;
        }

        return n;
    }

    uint32_t send(uint8_t* data, uint32_t len)
    {
        if (nullptr == data || 0 == len)
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

            int ret = -1;
            if (SERVER == serverOrClient)
            {
                ret =  ::write(clientSocketFd, data + sendedLen - 2, sendLen + 2);
            }
            else if(CLIENT == serverOrClient)
            {
                ret =  ::write(serverSocketFd, data + sendedLen - 2, sendLen + 2);
            }

            if (ret <= 0)
            {
                printf("sendto fail, errno: %d\n", errno);
                return 0;
            }
            sendedLen += (ret - 2); // 2 bytes for packet header

            // printf("send[%d] bytes, sendedLen[%u] bytes sequence:[%u]\n", ret, sendedLen, sequence);
        }
        return sendedLen;
    }

    uint32_t recv(uint8_t* data, uint32_t len)
    {
        if (nullptr == data || 0 == len)
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
            if (SERVER == serverOrClient)
            {
                ret =  ::read(clientSocketFd, data+recvedLen, len-recvedLen);
            }
            else if(CLIENT == serverOrClient)
            {
                ret =  ::read(serverSocketFd, data+recvedLen, len-recvedLen);
            }

            // ret = recvfrom(sockfd_, data+recvedLen, len-recvedLen, 0, (struct sockaddr *)&client_addr_, &client_addrLen_);
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

    uint32_t tcp_send(uint8_t* data, uint32_t len)
    {
        *(data - 5) = 0xFF; // 头标志
        *(data - 4) = len>>24&0xff;
        *(data - 3) = len>>16&0xff;
        *(data - 2) = len>>8&0xff;
        *(data - 1) = len&0xff;

        if (SERVER == serverOrClient)
        {
            return ::write(serverSocketFd, data - 5, len + 5);
        }
        else if(CLIENT == serverOrClient)
        {
            return ::write(clientSocketFd, data - 5, len + 5);
        }

        return 0;
    }

    uint32_t tcp_recv(uint8_t* data, uint32_t len)
    {
        uint32_t readLen = 0;
        if (SERVER == serverOrClient)
        {
            readLen = ::read(serverSocketFd, data , 5);
        }
        else if(CLIENT == serverOrClient)
        {
            readLen = ::read(clientSocketFd, data , 5);
        }

        return readLen;
    }

    ~Socket()
    {
        close(clientSocketFd);
        close(serverSocketFd);
    }
private:
    std::string IP;
    unsigned short port;
    int serverSocketFd;
    int clientSocketFd;
    serverOrClient_t serverOrClient;
};
