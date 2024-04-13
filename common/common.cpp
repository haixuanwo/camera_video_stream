/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-10 17:53:30
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-13 10:28:08
 * @Description: file content
 */
#include "../common/common.h"
#include <chrono>
#include <ctime>

/**
 * @brief 以十六进制打印数据
 * @param info
 * @param data
 * @param len
 */
void print_hex_data(const char *info, uint8_t* data, int len)
{
    printf("%s len[%d]: ", info, len);
    for (int i = 0; i < (len>100?100:len); i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/**
 * @brief 设置uint32_t值到buf中
 */
void set_value(uint32_t value, uint8_t* buf)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
}

/**
 * @brief 从buf中获取uint32_t值
 */
uint32_t get_value(uint8_t* buf)
{
    uint32_t value = buf[0]<<24;
    value += buf[1]<<16;
    value += buf[2]<<8;
    value += buf[3];

    return value;
}

/**
 * @brief 保存一帧数据到文件
 * @param data
 * @param len
 * @param name
 * @return true
 * @return false
 */
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

/**
 * @brief 测试帧率
 */
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
