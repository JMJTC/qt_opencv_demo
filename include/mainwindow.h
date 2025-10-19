#ifndef _QTOPENCV_DEMO_MAINWINDOW_H_
#define _QTOPENCV_DEMO_MAINWINDOW_H_

#include "imageprocessor.h"
#include "videoplayer.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

enum class MediaType
{
    None,
    Image,
    Video
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void openMedia();
    void playPause();
    void onFrameReady(const QImage &frame);
    void onPositionChanged(double pos);
    void onSliderReleased();
    void onFilterChanged(int index);

  private:
    void displayImage(const cv::Mat &img);

    VideoPlayer *m_pPlayer = nullptr;
    QLabel *m_pDisplayLabel = nullptr;
    QPushButton *m_pOpenButton = nullptr;
    QPushButton *m_pPlayButton = nullptr;
    QComboBox *m_pFilterCombo = nullptr;
    QSlider *m_pSlider = nullptr;

    MediaType m_currentMediaType{};
    cv::Mat m_currentImage{};
    FilterType m_currentFilter{};
    bool m_isPlaying = false;
};

#endif // _QTOPENCV_DEMO_MAINWINDOW_H_