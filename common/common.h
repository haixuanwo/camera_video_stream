/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-10 17:28:16
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-12 16:55:21
 * @Description: file content
 */

#ifndef COMMON_H
#define COMMON_H

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <memory>

#define WIDTH 1920
#define HEIGHT 1080

/**
 * @brief 以十六进制打印数据
 * @param info
 * @param data
 * @param len
 */
void print_hex_data(const char *info, uint8_t* data, int len);

/**
 * @brief 设置uint32_t值到buf中
 */
void set_value(uint32_t value, uint8_t* buf);

/**
 * @brief 从buf中获取uint32_t值
 */
uint32_t get_value(uint8_t* buf);

/**
 * @brief 保存一帧数据到文件
 * @param data
 * @param len
 * @param name
 * @return true
 * @return false
 */
bool save_data_to_file(unsigned char *data, uint32_t len, const char *name);

/**
 * @brief 测试帧率
 */
void test_fps(void);

#endif
