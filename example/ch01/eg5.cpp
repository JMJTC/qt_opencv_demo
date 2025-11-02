#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat image = cv::imread("../../assets/111.png");

    if (image.empty())
    {
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }

    // 转为灰度
    cv::Mat grayImage;
    cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

    cv::imshow("Gray Image", grayImage);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}