#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    // 创建VideoCapture对象，参数0表示默认摄像头
    cv::VideoCapture cap(0);

    // 检查摄像头是否成功打开
    if (!cap.isOpened())
    {
        std::cerr << "无法打开摄像头" << std::endl;
        return -1;
    }

    // 创建窗口用于显示
    cv::namedWindow("Camera", cv::WINDOW_AUTOSIZE);

    // 主循环
    while (true)
    {
        cv::Mat frame;
        // 从摄像头读取一帧
        cap >> frame;

        // 检查是否成功读取到帧
        if (frame.empty())
        {
            std::cerr << "无法获取帧" << std::endl;
            break;
        }

        // 显示帧
        cv::imshow("Camera", frame);

        // 等待用户按键，如果按下ESC键(ASCII码27)则退出
        if (cv::waitKey(30) == 27)
        {
            break;
        }
    }

    // 释放资源
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
