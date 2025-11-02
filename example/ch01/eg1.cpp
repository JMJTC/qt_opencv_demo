#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // 读取图像
    cv::Mat image = cv::imread("../../assets/111.png");

    if (image.empty())
    {
        std::cout << "Could not open or find the image!" << std::endl;
        return -1;
    }

    // 显示图像
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Display window", image);

    // 等待按键
    cv::waitKey(0);

    cv::destroyAllWindows();
    return 0;
}