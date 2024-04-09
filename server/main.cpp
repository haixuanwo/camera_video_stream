/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-08 18:19:31
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 11:00:55
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

    int len;

    if(argc < 2)
    {
        printf("Please input port\n");
        return -1;
    }

    char name[128] = {0};
    auto udpServer = make_shared<UdpServer>(atoi(argv[1]));
    auto buf = vector<uint8_t>(WIDTH * HEIGHT*3/2, 0);
    // data->resize(WIDTH * HEIGHT*3/2);

    uint32_t index = 0;

    while(1)
    {
        len = udpServer->recv(buf.data(), buf.size());
        if (len > 0)
        {
            cout << "recv len: " << len << endl;
            for (size_t i = 0; i < 5; i++)
            {
                printf("%02x ", buf[i]);
            }
            printf("\n");
        }

        snprintf(name, sizeof(name), "frame_%u.jpg", index);
        save_data_to_file(buf.data(), len, name);
        index++;

        #if 0
        cv::Mat gray(HEIGHT, WIDTH, CV_8UC1, grayData);
        cv::Mat rgb;
        cv::cvtColor(gray, rgb, CV_GRAY2BGR);
        // face_func(rgb);
        cv::imshow("test", rgb);
        cv::waitKey(10);
        #endif
    }

    return 0;
}
