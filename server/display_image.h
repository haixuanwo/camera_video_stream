/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-12 16:36:13
 * @LastEditors: Clark
 * @LastEditTime: 2024-07-13 21:31:47
 * @Description: file content
 */
#ifndef DISPLAY_IMAGE_H
#define DISPLAY_IMAGE_H

#include <queue>
#include <vector>
#include <mutex>
#include <thread>
#include "../common/common.h"

class DisplayImage {
public:
    DisplayImage();

    bool push(const uint8_t* image, uint32_t size);

    std::shared_ptr<std::vector<uint8_t>> pop();

    void threadFunction();

    void startThread();

    ~DisplayImage();
private:
    uint32_t total_image_size_;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> image_queue_;
    std::mutex mutex_;
    std::thread displayThread;
};

#endif
