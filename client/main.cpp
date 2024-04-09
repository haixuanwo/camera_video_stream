/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2021-12-18 10:02:46
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-09 11:30:14
 * @Description: udp通信客户端
 */
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <vector>

extern "C" {
#include <sys/time.h>
}

#include "cameraapp.h"
#include "udp_client.h"

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

    if (argc < 4)
    {
        printf("Please input camear index, ip and port\n");
        return -1;
    }

    int index = atoi(argv[1]);
    int ret = cam_init(index, WIDTH, HEIGHT);
    if(ret < 0)
    {
        printf("open camera failed\n");
        return -1;
    }

    camera_streamon();

    int frameLen = 0;
    uint64_t frameCount = 0;

    char name[128] = {0};

    auto udpClient = make_shared<UdpClient>(argv[2], atoi(argv[3]));
    vector<uint8_t> buf(WIDTH*HEIGHT*3/2+2);
    uint8_t *data = buf.data() + 2; // udp在应用层分包sendto，buf[0] is cmd, buf[1] is 帧数量或帧序号

    // for (size_t i = 0; i < 20; i++)
    while (1)
    {
        camera_capture(data, &frameLen);

        // snprintf(name, sizeof(name), "frame_%lu.jpg", i);
        // save_data_to_file(data, frameLen, name);

        frameCount++;
        printf("JH ---frameLen:%d frameCount:%lu\n", frameLen, frameCount);

        printf("send len:%d\n", udpClient->send(data, frameLen));
    }

    return 0;
}
