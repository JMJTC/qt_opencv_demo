#include "videowidget.h"
#include <QImage>
#include <QPixmap>
#include <iostream>

VideoWidget::VideoWidget(QWidget *parent) : QLabel(parent), timer(nullptr)
{
    // 1. 尝试打开摄像头
    if (cap.open(0))
    {
        std::cout << "✅ 摄像头打开成功" << std::endl;
    }
    else
    {
        std::cout << "⚠️ 摄像头打开失败，尝试播放本地视频..." << std::endl;
        std::string videoPath = "../assets/demo.mp4";
        if (!cap.open(videoPath))
        {
            std::cerr << "❌ 无法打开摄像头或视频文件：" << videoPath << std::endl;
            return;
        }
        else
        {
            std::cout << "✅ 已切换到本地视频：" << videoPath << std::endl;
        }
    }

    // 固定窗口大小
    this->setFixedSize(640, 480);

    // 定时器刷新
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &VideoWidget::updateFrame);
    timer->start(30);
}

VideoWidget::~VideoWidget()
{
    cap.release();
}

void VideoWidget::updateFrame()
{
    cv::Mat frame;
    cap >> frame;
    if (frame.empty())
    {
        // 如果是视频播放完了，重头开始
        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
        return;
    }

    // BGR -> RGB
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img((uchar *)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    this->setPixmap(QPixmap::fromImage(img).scaled(this->size(), Qt::KeepAspectRatio));
}