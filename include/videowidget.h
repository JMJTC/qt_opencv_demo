#ifndef _QTOPENCV_DEMO_VIDEOWIDGET_H_
#define _QTOPENCV_DEMO_VIDEOWIDGET_H_

#include <QLabel>
#include <QTimer>
#include <opencv2/opencv.hpp>

class VideoWidget : public QLabel
{
    Q_OBJECT
  public:
    explicit VideoWidget(QWidget *parent = nullptr);
    ~VideoWidget() override;

  private slots:
    void updateFrame();

  private:
    cv::VideoCapture cap{};
    QTimer *timer{};
};

#endif // _QTOPENCV_DEMO_VIDEOWIDGET_H_