#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // 读取
    cv::Mat image = cv::imread("../../assets/111.png");

    if (image.empty())
    {
        std::cout << "image is empty" << std::endl;
        return -1;
    }

    // 保存
    bool isSaved = cv::imwrite("../../assets/111_copy.png", image);
    if (isSaved)
    {
        std::cout << "image saved" << std::endl;
    }
    else
    {
        std::cout << "image not saved" << std::endl;
    }
    return 0;
}