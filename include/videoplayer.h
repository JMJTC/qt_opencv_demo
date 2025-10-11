#ifndef _QTOPENCV_DEMO_VIDEOPLAYER_H_
#define _QTOPENCV_DEMO_VIDEOPLAYER_H_

#include "imageprocessor.h"
#include <QImage>
#include <QMutex>
#include <QString>
#include <QTimer>
#include <opencv2/opencv.hpp>
class VideoPlayer : public QObject
{
    Q_OBJECT
  public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    bool loadVideo(const QString &filePath);
    void play();
    void pause();
    void stop();
    void setPosition(double pos); // 0~1 范围
    void setFilter(FilterType type);

  signals:
    void frameReady(const QImage &frame);
    void positionChanged(double pos);
    void finished();

  private slots:
    void processFrame();

  private:
    cv::VideoCapture cap{};
    QTimer *timer{};
    QMutex mutex{};
    FilterType currentFilter{};
    int fps{};
    bool isPlaying{};
};

#endif // _QTOPENCV_DEMO_VIDEOPLAYER_H_