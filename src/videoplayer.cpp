#include "videoplayer.h"
#include <QDebug>

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent), timer(new QTimer(this)), currentFilter(FilterType::None), fps(30), isPlaying(false)
{
    connect(timer, &QTimer::timeout, this, &VideoPlayer::processFrame);
}

VideoPlayer::~VideoPlayer()
{
    stop();
}

bool VideoPlayer::loadVideo(const QString &filePath)
{
    QMutexLocker locker(&mutex);
    if (cap.isOpened())
        cap.release();

    cap.open(filePath.toStdString());
    if (!cap.isOpened())
    {
        qWarning() << "无法打开视频:" << filePath;
        return false;
    }

    fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    if (fps <= 0)
        fps = 30; // fallback

    setPosition(0.0);
    return true;
}

void VideoPlayer::play()
{
    if (!cap.isOpened())
        return;
    isPlaying = true;
    timer->start(1000 / fps);
}

void VideoPlayer::pause()
{
    isPlaying = false;
    timer->stop();
}

void VideoPlayer::stop()
{
    pause();
    QMutexLocker locker(&mutex);
    if (cap.isOpened())
        cap.release();
}

void VideoPlayer::setFilter(FilterType type)
{
    QMutexLocker locker(&mutex);
    currentFilter = type;
}

void VideoPlayer::setPosition(double pos)
{
    if (!cap.isOpened())
        return;
    long total = cap.get(cv::CAP_PROP_FRAME_COUNT);
    long frame = static_cast<long>(total * pos);
    cap.set(cv::CAP_PROP_POS_FRAMES, frame);
    // 立即读取并显示该帧
    cv::Mat frameMat;
    if (cap.read(frameMat))
    {
        frameMat = ImageProcessor::applyFilter(frameMat, currentFilter);
        cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2RGB);
        QImage img(frameMat.data, frameMat.cols, frameMat.rows, frameMat.step, QImage::Format_RGB888);
        emit frameReady(img.copy());
        double curPos = cap.get(cv::CAP_PROP_POS_FRAMES) / cap.get(cv::CAP_PROP_FRAME_COUNT);
        emit positionChanged(curPos);
        // 回到拖动后的位置
        cap.set(cv::CAP_PROP_POS_FRAMES, frame);
    }
}

void VideoPlayer::processFrame()
{
    if (!isPlaying || !cap.isOpened())
        return;

    cv::Mat frame;
    {
        QMutexLocker locker(&mutex);
        if (!cap.read(frame))
        {
            // 播放完自动回到开头并显示第一帧
            cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            if (cap.read(frame))
            {
                frame = ImageProcessor::applyFilter(frame, currentFilter);
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                emit frameReady(img.copy());
                emit positionChanged(0.0);
            }
            emit finished();
            pause();
            return;
        }
        frame = ImageProcessor::applyFilter(frame, currentFilter);
    }

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    emit frameReady(img.copy());

    double pos = cap.get(cv::CAP_PROP_POS_FRAMES) / cap.get(cv::CAP_PROP_FRAME_COUNT);
    emit positionChanged(pos);
}