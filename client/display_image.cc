/*
 * @Author: Clark
 * @Email: haixuanwoTxh@gmail.com
 * @Date: 2024-04-12 16:36:13
 * @LastEditors: Clark
 * @LastEditTime: 2024-07-13 21:31:38
 * @Description: file content
 */
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "display_image.h"
using namespace std;

#define MAX_TOTAL_IMAGE_SIZE 1024 * 1024 * 10 // 10MB

DisplayImage::DisplayImage()
{
    total_image_size_ = 0;
    image_queue_ = queue<shared_ptr<vector<uint8_t>>>();
}

bool DisplayImage::push(const uint8_t* image, uint32_t size)
{
    lock_guard<mutex> lock(mutex_);

    if (MAX_TOTAL_IMAGE_SIZE < total_image_size_ + size)
    {
        cout << "Image queue["<<image_queue_.size()<<"] full! total_image_size_:"<< total_image_size_ << endl;
        return false;
    }

    auto item = make_shared<vector<uint8_t>>(image, image + size);
    image_queue_.push(item);
    total_image_size_ += size;

    // cout << __FUNCTION__<<" Image queue size:" << image_queue_.size() << " total_image_size_:" << total_image_size_ << endl;
    return true;
}

shared_ptr<vector<uint8_t>> DisplayImage::pop()
{
    lock_guard<mutex> lock(mutex_);

    auto image = image_queue_.front();
    image_queue_.pop();

    total_image_size_ -= image->size();
    // cout << __FUNCTION__<<" Image queue size:" << image_queue_.size() << " total_image_size_:" << total_image_size_ << endl;
    return image;
}

void DisplayImage::threadFunction()
{
    cout << "DisplayImage thread --- start ---" << endl;

    while (true)
    {
        if (image_queue_.empty())
        {
            this_thread::yield();
            continue;
        }

        auto image = pop();
        cv::Mat matImage = cv::imdecode(cv::Mat(HEIGHT, WIDTH, CV_8UC3, image->data()), cv::IMREAD_COLOR);

        if (matImage.empty() || 0 == matImage.cols || 0 == matImage.rows)
        {
            cout << "Thread "<< " decode image failed!" << " pop image size:" << image->size() << "width:" << matImage.cols << "height:" << matImage.rows << endl;
            continue;
        }

        cv::imshow("DisplayThread", matImage);
        cv::waitKey(1);

        test_fps();
    }
}

void DisplayImage::startThread()
{
    displayThread = std::thread(&DisplayImage::threadFunction, this);
}

DisplayImage::~DisplayImage()
{

}
