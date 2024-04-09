/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 18:19:31
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 14:47:58
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
#include "udp_server.h"

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

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        printf("Please input port\n");
        return -1;
    }

    uint32_t len = 0;
    auto udpServer = make_shared<UdpServer>(atoi(argv[1]));
    auto buf = vector<uint8_t>(WIDTH * HEIGHT*3/2, 0);

    while(1)
    {
        len = udpServer->recv(buf.data(), buf.size());
        if (len <= 0)
        {
            continue;
        }
        printf("recv len: %d\n", len);

        // static uint32_t index = 0;
        // char name[128] = {0};
        // snprintf(name, sizeof(name), "frame_%u.jpg", index);
        // save_data_to_file(buf.data(), len, name);
        // index++;

        cv::Mat image = cv::imdecode(cv::Mat(1, len, CV_8UC3, buf.data()), cv::IMREAD_COLOR);
        cv::imshow("test", image);
        cv::waitKey(10);
    }

    return 0;
}
