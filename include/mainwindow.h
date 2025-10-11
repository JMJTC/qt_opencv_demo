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
    MainWindow(QWidget *parent = nullptr);
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

    VideoPlayer *player = nullptr;
    QLabel *displayLabel = nullptr;
    QPushButton *openButton = nullptr;
    QPushButton *playButton = nullptr;
    QComboBox *filterCombo = nullptr;
    QSlider *slider = nullptr;

    MediaType currentMediaType{};
    cv::Mat currentImage{};
    FilterType currentFilter{};
    bool isPlaying = false;
};

#endif // _QTOPENCV_DEMO_MAINWINDOW_H_