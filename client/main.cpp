/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-13 10:03:01
 * @Description: udp通信客户端
 */
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <vector>

#include "cameraapp.h"
// #include "udp_client.h"
#include "../common/tcp_socket.h"
#include "../common/common.h"
#include "../common/protocol.h"

using namespace std;

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        printf("Please input camear index, ip and port\n");
        return -1;
    }

    int ret = cam_init(atoi(argv[1]), WIDTH, HEIGHT);
    if(ret < 0)
    {
        printf("open camera failed\n");
        return -1;
    }

    camera_streamon();

    int frameLen = 0;
    // auto udpClient = make_shared<UdpClient>(argv[2], atoi(argv[3]));
    auto client = make_shared<Socket>(argv[2], atoi(argv[3]), CLIENT);
    client->init();
    if (false == client->connect_server())
    {
        return -1;
    }

    vector<uint8_t> buf(WIDTH*HEIGHT*3/2+2);
    vector<uint8_t> packet(WIDTH*HEIGHT*3/2+2);
    uint32_t packetLen = 0;

    auto protocol = make_shared<Protocol>();

    // for (size_t i = 0; i < 5; i++)
    while (1)
    {
        camera_capture(buf.data(), &frameLen);
        if (frameLen <= 0)
        {
            printf("capture frame failed\n");
            continue;
        }

        // char name[128] = {0};
        // snprintf(name, sizeof(name), "frame_%lu.jpg", i);
        // save_data_to_file(data, frameLen, name);

        // udpClient->send(data, frameLen);
        // client->tcp_send(data, frameLen);

        protocol->build_packet(buf.data(), frameLen, packet.data(), packetLen);

        client->writen(packet.data(), packetLen);

        // print_hex_data("recv len", packet.data(), frameLen);
    }

    return 0;
}
