/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 18:19:31
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-12 16:13:24
 * @Description: file content
 */
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <memory>
#include <vector>
#include <iostream>
#include <chrono>
#include <ctime>

// #include "udp_server.h"
#include "../common/tcp_socket.h"
#include "../common/common.h"
#include "../common/protocol.h"

using namespace std;

#define WIDTH 1920
#define HEIGHT 1080

bool save_data_to_file(unsigned char *data, uint32_t len, const char *name)
{
    FILE *fp = fopen(name, "wb");
    if(fp == NULL)
    {
        printf("open file %s failed\n", name);
        return false;
    }

    fwrite(data, 1, len, fp);
    fclose(fp);
    return true;
}

void test_fps(void)
{
    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 将当前时间点转换为毫秒数
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    // 获取毫秒数
    static uint64_t last_ms = 0;
    auto now_value = now_ms.time_since_epoch().count();

    static uint32_t count = 0;
    if (0 == count)
    {
        count = 1;
        last_ms = now_value;
        return;
    }

    count++;
    if ((now_value - last_ms) > 3000)
    {
        printf("JH --- fps[%u] ----\n", count/3);
        count = 0;
        last_ms = now_value;
    }
}


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

            // print_hex_data("frame", frame.data(), frameLen);
            // continue;

            // static uint32_t index = 0;
            // char name[128] = {0};
            // snprintf(name, sizeof(name), "frame_%u.jpg", index);
            // save_data_to_file(buf.data(), len, name);
            // index++;

            test_fps();

            #if 0
            cv::Mat image = cv::imdecode(cv::Mat(HEIGHT, WIDTH, CV_8UC3, frame.data()), cv::IMREAD_COLOR);
            cv::imshow("test", image);
            cv::waitKey(3);
            #endif
        }
    }

    return 0;
}
