/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-10 17:28:16
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-12 14:29:49
 * @Description: file content
 */

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <cstdio>

void print_hex_data(const char *info, uint8_t* data, int len);

void set_value(uint32_t value, uint8_t* buf);

uint32_t get_value(uint8_t* buf);

#endif
