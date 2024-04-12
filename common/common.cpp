/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-10 17:53:30
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-12 14:39:38
 * @Description: file content
 */

#include "../common/common.h"

void print_hex_data(const char *info, uint8_t* data, int len)
{
    printf("%s len[%d]: ", info, len);
    for (int i = 0; i < (len>100?100:len); i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

void set_value(uint32_t value, uint8_t* buf)
{
    buf[0] = (value >> 24) & 0xFF;
    buf[1] = (value >> 16) & 0xFF;
    buf[2] = (value >> 8) & 0xFF;
    buf[3] = value & 0xFF;
}


uint32_t get_value(uint8_t* buf)
{
    uint32_t value = buf[0]<<24;
    value += buf[1]<<16;
    value += buf[2]<<8;
    value += buf[3];

    return value;
}
