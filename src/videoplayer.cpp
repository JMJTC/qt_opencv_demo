#include "videoplayer.h"
#include <QDebug>

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent), m_pTimer(new QTimer(this)), m_currentFilter(FilterType::None), m_fps(30), m_isPlaying(false)
{
    connect(m_pTimer, &QTimer::timeout, this, &VideoPlayer::processFrame);
}

VideoPlayer::~VideoPlayer()
{
    stop();
}

bool VideoPlayer::loadVideo(const QString &filePath)
{
    QMutexLocker locker(&m_mutex);
    if (m_cap.isOpened())
        m_cap.release();

    m_cap.open(filePath.toStdString());
    if (!m_cap.isOpened())
    {
        qWarning() << "无法打开视频:" << filePath;
        return false;
    }

    m_fps = static_cast<int>(m_cap.get(cv::CAP_PROP_FPS));
    if (m_fps <= 0)
        m_fps = 30; // fallback

    setPosition(0.0);
    return true;
}

void VideoPlayer::play()
{
    if (!m_cap.isOpened())
        return;
    m_isPlaying = true;
    m_pTimer->start(1000 / m_fps);
}

void VideoPlayer::pause()
{
    m_isPlaying = false;
    m_pTimer->stop();
}

void VideoPlayer::stop()
{
    pause();
    QMutexLocker locker(&m_mutex);
    if (m_cap.isOpened())
        m_cap.release();
}

void VideoPlayer::setFilter(FilterType type)
{
    QMutexLocker locker(&m_mutex);
    m_currentFilter = type;
}

void VideoPlayer::setPosition(double pos)
{
    if (!m_cap.isOpened())
        return;
    long total = m_cap.get(cv::CAP_PROP_FRAME_COUNT);
    long frame = static_cast<long>(total * pos);
    m_cap.set(cv::CAP_PROP_POS_FRAMES, frame);
    // 立即读取并显示该帧
    cv::Mat frameMat;
    if (m_cap.read(frameMat))
    {
        frameMat = ImageProcessor::applyFilter(frameMat, m_currentFilter);
        cv::cvtColor(frameMat, frameMat, cv::COLOR_BGR2RGB);
        QImage img(frameMat.data, frameMat.cols, frameMat.rows, frameMat.step, QImage::Format_RGB888);
        emit frameReady(img.copy());
        double curPos = m_cap.get(cv::CAP_PROP_POS_FRAMES) / m_cap.get(cv::CAP_PROP_FRAME_COUNT);
        emit positionChanged(curPos);
        // 回到拖动后的位置
        m_cap.set(cv::CAP_PROP_POS_FRAMES, frame);
    }
}

void VideoPlayer::processFrame()
{
    if (!m_isPlaying || !m_cap.isOpened())
        return;

    cv::Mat frame;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_cap.read(frame))
        {
            // 播放完自动回到开头并显示第一帧
            m_cap.set(cv::CAP_PROP_POS_FRAMES, 0);
            if (m_cap.read(frame))
            {
                frame = ImageProcessor::applyFilter(frame, m_currentFilter);
                cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
                QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
                emit frameReady(img.copy());
                emit positionChanged(0.0);
            }
            emit finished();
            pause();
            return;
        }
        frame = ImageProcessor::applyFilter(frame, m_currentFilter);
    }

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    emit frameReady(img.copy());

    double pos = m_cap.get(cv::CAP_PROP_POS_FRAMES) / m_cap.get(cv::CAP_PROP_FRAME_COUNT);
    emit positionChanged(pos);
}