#ifndef _QTOPENCV_DEMO_VIDEOWIDGET_H_
#define _QTOPENCV_DEMO_VIDEOWIDGET_H_

#include <QLabel>
#include <QTimer>
#include <iostream>
#include <opencv2/opencv.hpp>

class VideoWidget : public QLabel
{
    Q_OBJECT
  public:
    VideoWidget(QWidget *parent = nullptr);

    ~VideoWidget() override;

  private slots:
    void updateFrame()
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

  private:
    cv::VideoCapture cap{};
    QTimer *timer{};
};

#endif // _QTOPENCV_DEMO_VIDEOWIDGET_H_