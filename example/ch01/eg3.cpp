#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat image = cv::imread("../../assets/111.png");
    if (image.empty())
    {
        std::cout << "Could not read the image" << std::endl;
        return 1;
    }

    int width = image.cols;
    int height = image.rows;
    int channels = image.channels();

    std::cout << "width: " << width << ", height: " << height << ", channels: " << channels << std::endl;
}