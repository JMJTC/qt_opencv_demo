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
    cv::VideoCapture m_cap{};
    QTimer *m_pTimer{};
};

#endif // _QTOPENCV_DEMO_VIDEOWIDGET_H_