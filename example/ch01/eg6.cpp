#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // 读取图像文件
    cv::Mat image = cv::imread("../../assets/111.png");
    // 检查图像是否成功加载
    if (image.empty())
    {
        std::cout << "Could not read the image" << std::endl;
        return -1;
    }

    // 裁剪
    // 定义一个矩形感兴趣区域(Region of Interest, ROI)
    // 参数分别为：左上角x坐标、左上角y坐标、矩形的宽度、矩形的高度
    cv::Rect roi(100, 100, 200, 200);
    cv::Mat croppedImage = image(roi);
    // 缩放
    cv::Mat resizedImage;
    cv::resize(image, resizedImage, cv::Size(300, 300));
    cv::imshow("Original Image", image);
    cv::imshow("Cropped Image", croppedImage);
    cv::imshow("Resized Image", resizedImage);
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}