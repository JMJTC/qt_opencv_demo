#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat image = cv::imread("../../assets/111.png");
    if (image.empty())
    {
        std::cout << "Could not open or find the image!\n";
        return -1;
    }

    // 访问像素值（BGR格式）
    cv::Vec3b pixel = image.at<cv::Vec3b>(100, 100); // 获取(100, 100)像素的值
    std::cout << "Pixel value at (100, 100): B=" << (int)pixel[0] << ", G=" << (int)pixel[1] << ", R=" << (int)pixel[2]
              << std::endl;
    // 修改像素值
    image.at<cv::Vec3b>(100, 100) = cv::Vec3b(255, 0, 0); // 设置成蓝色
    cv::imshow("Image", image);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}