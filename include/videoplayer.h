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
    bool loadCamera(int index = 0);
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
    cv::VideoCapture m_cap{};
    QTimer *m_pTimer{};
    QMutex m_mutex{};
    FilterType m_currentFilter{};
    int m_fps{};
    bool m_isPlaying{};
    // 无需额外标志，VideoCapture 在摄像头模式下可能没有总帧数
};

#endif // _QTOPENCV_DEMO_VIDEOPLAYER_H_