/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 18:19:31
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-13 09:34:32
 * @Description: file content
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <vector>

// #include "udp_server.h"
#include "../common/tcp_socket.h"
#include "../common/common.h"
#include "../common/protocol.h"
#include "display_image.h"

using namespace std;

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Please input port\n");
        return -1;
    }

    auto protocol = make_shared<Protocol>();

    uint32_t len = 0;
    // auto udpServer = make_shared<UdpServer>(atoi(argv[1]));
    auto server = make_shared<Socket>("0.0.0.0", atoi(argv[1]), SERVER);
    server->init();

    auto buf = vector<uint8_t>(WIDTH * HEIGHT*3/2, 0);
    auto frame = vector<uint8_t>(WIDTH * HEIGHT*3/2, 0);
    uint32_t frameLen = 0;

    auto displayImage = make_shared<DisplayImage>();
    displayImage->startThread();

    while (true)
    {
        if (false == server->accept_client())
        {
            return -1;
        }

        while(1)
        {
            // len = udpServer->recv(buf.data(), buf.size());
            len = server->read((char *)buf.data(), buf.size());
            if (len <= 0)
            {
                break;
            }

            if (!protocol->get_frame_from_data(buf.data(), len, frame.data(), frameLen))
            {
                continue;
            }

            displayImage->push(frame.data(), frameLen);

            // static uint32_t index = 0;
            // char name[128] = {0};
            // snprintf(name, sizeof(name), "frame_%u.jpg", index);
            // save_data_to_file(buf.data(), len, name);
            // index++;
        }
    }

    return 0;
}
