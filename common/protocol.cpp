/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-11 09:24:26
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-12 15:22:21
 * @Description: file content
 */
#include "protocol.h"
#include "../common/common.h"
#include <cstring>
#include <iostream>

using namespace std;

Protocol::Protocol()
{
    m_len = 0;
    m_data.resize(MAX_PACKET_SIZE*2);
}

Protocol::~Protocol()
{

}

/**
 * @brief 从数据中获取frame
 * @param data
 * @param dataLen
 * @param frame
 * @param frameLen
 * @return true
 * @return false
 */
bool Protocol::get_frame_from_data(uint8_t* data, uint32_t dataLen, uint8_t* frame, uint32_t& frameLen)
{
    if (m_len + dataLen > m_data.size())
    {
        return false;
    }

    // 将数据拷贝到缓冲区
    memcpy(m_data.data() + m_len, data, dataLen);
    m_len += dataLen;

    // print_hex_data("get_frame_from_data", m_data.data(), m_len);

    return parse_packet(frame, frameLen);
}

/**
 * @brief 构建数据帧
 * @param data
 * @param len    数据长度
 * @param packet 输出的帧
 * @param packetLen 帧长
 * @return true
 * @return false
 */
bool Protocol::build_packet(uint8_t* data, uint32_t len, uint8_t* packet, uint32_t& packetLen)
{
    uint32_t index = 0;

    packet[0] = HEAD1; // 帧头标志
    packet[1] = HEAD2; // 帧头标志
    index = 2;

    set_value(len, packet + index); // 帧长度
    index += 4;

    memcpy(packet+index, data, len); // 数据
    index += len;

    // 计算校验和，传输的是视频数据帧，数据量太大，因性能暂不计算校验和

    packet[index] = END1; // 帧尾标志
    index += 1;

    packet[index] = END2; // 帧尾标志
    index += 1;

    packetLen = index;

    return true;
}

/**
 * @brief 解析数据帧
 * @param frame
 * @param frameLen
 * @return true
 * @return false
 */
bool Protocol::parse_packet(uint8_t* frame, uint32_t &frameLen)
{
    if (m_len < MIN_PACKET_LEN)
    {
        std::cout<<"JH --- "<< __FUNCTION__ <<" data not enough MIN_PACKET_LEN m_len: "<< m_len << endl;
        return false;
    }

    uint32_t index = 0;

    while (1)
    {
        // print_hex_data("parse_packet", m_data.data(), m_len);
        // 找第一个帧头
        while (m_data[index] != HEAD1)
        {
            if (m_len - index < MIN_PACKET_LEN)
            {
                std::cout<<"JH --- "<< __FUNCTION__ <<" data not enough HEAD1 MIN_PACKET_LEN m_len: " << m_len << " index: " << index << endl;
                return false;
            }
            index++;
        }

        // 调整数据位置
        if (0 != index)
        {
            m_len -= index;
            memmove(m_data.data(), m_data.data() + index, m_len);

            index = 0;
        }

        index++;

        // 找第二个帧头
        if (m_data[index] != HEAD2)
        {
            m_len -= index;
            memmove(m_data.data(), m_data.data() + index, m_len);
            index = 0;
            continue;
        }
        index++;

        // 获取帧数据长度
        uint32_t len = get_value(m_data.data() + index);
        if (len > (m_len - MIN_PACKET_LEN))
        {
            std::cout<<"JH --- "<< __FUNCTION__ <<" data not enough frame len:" << len << endl;
            return false;
        }
        else if (len > MAX_PACKET_SIZE)
        {
            std::cout<<"JH --- "<< __FUNCTION__ <<" data too large" << endl;
            memmove(m_data.data(), m_data.data() + 1, m_len - 1); // 丢掉第一个头，重新找第一个头
            index = 0;
            continue;
        }
        index += 4;

        // 计算校验和，传输的是视频数据帧，数据量太大，因性能暂不计算校验和

        // 判断帧尾
        if (END1 != m_data[index+len] || END2 != m_data[index+len+1])
        {
            std::cout<< "JH --- " << __FUNCTION__ << " END fail" << endl;
            memmove(m_data.data(), m_data.data() + 1, m_len - 1); // 丢掉第一个头，重新找第一个头
            index = 0;
            continue;
        }

        // 获取帧数据
        memcpy(frame, m_data.data() + index, len);
        frameLen = len;
        std::cout<< "JH --- " << __FUNCTION__ << " frameLen: " << frameLen << endl;

        // 调整剩下数据位置
        m_len -= (len + MIN_PACKET_LEN);
        memmove(m_data.data(), m_data.data() + (len + MIN_PACKET_LEN), m_len);

        return true;
    }

    return false;
}
