/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-11 09:24:26
 * @LastEditors: Clark
 * @LastEditTime: 2024-04-11 17:09:50
 * @Description: file content
 */
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <cstdint>
#include <vector>

#define HEAD1 0xFF
#define HEAD2 0xFF
#define END1  0x00
#define END2  0x00

#define MIN_PACKET_LEN 8    // 最小包长6 bytes, head1 + head2 + len + end1 + end2
#define MAX_DATA_SIZE 1920*1080*3/2
#define MAX_PACKET_SIZE (MIN_PACKET_LEN + MAX_DATA_SIZE)

// 使用 __attribute__((packed)) 属性来声明紧凑排列
typedef struct __attribute__((packed)) {
    uint8_t head1;
    uint8_t head2;
    uint32_t len;
    uint8_t data[0];    // 变长数组，不占数组空间，指向数据区
} protocol_header_t;

class Protocol
{
public:
    Protocol();
    ~Protocol();

    /**
     * @brief 从数据中获取frame
     * @param data
     * @param dataLen
     * @param frame
     * @param frameLen
     * @return true
     * @return false
     */
    bool get_frame_from_data(uint8_t* data, uint32_t dataLen, uint8_t* frame, uint32_t& frameLen);

    /**
     * @brief 构建数据帧
     * @param data
     * @param len    数据长度
     * @param packet 输出的帧
     * @param packetLen 帧长
     * @return true
     * @return false
     */
    bool build_packet(uint8_t* data, uint32_t len, uint8_t* packet, uint32_t& packetLen);    // 编码

    /**
     * @brief 解析数据帧
     * @param frame
     * @param frameLen
     * @return true
     * @return false
     */
    bool parse_packet(uint8_t* frame, uint32_t &frameLen);    // 解码

private:
    uint32_t m_len;                // 存放接收数据长度
    std::vector<uint8_t> m_data;   // 存放用于解析数据帧收的数据
};

#endif // PROTOCOL_H
