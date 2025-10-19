#ifndef _QTOPENCV_DEMO_MAINWINDOW_H_
#define _QTOPENCV_DEMO_MAINWINDOW_H_

#include "imageprocessor.h"
#include "videoplayer.h"
#include "facedetector.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
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
    void onFaceDetectionParamsChanged();

  private:
    void displayImage(const cv::Mat &img);
    void setupFaceDetectionControls();
    void updateFaceDetectionParams();

    VideoPlayer *m_pPlayer = nullptr;
    QLabel *m_pDisplayLabel = nullptr;
    QPushButton *m_pOpenButton = nullptr;
    QPushButton *m_pPlayButton = nullptr;
    QComboBox *m_pFilterCombo = nullptr;
    QSlider *m_pSlider = nullptr;
    
    // 人脸检测参数控件
    QGroupBox *m_pFaceDetectionGroup = nullptr;
    QDoubleSpinBox *m_pScaleFactorSpin = nullptr;
    QSpinBox *m_pMinNeighborsSpin = nullptr;
    QSpinBox *m_pMinSizeSpin = nullptr;
    QSpinBox *m_pMaxSizeSpin = nullptr;

    MediaType m_currentMediaType{};
    cv::Mat m_currentImage{};
    FilterType m_currentFilter{};
    bool m_isPlaying = false;
};

#endif // _QTOPENCV_DEMO_MAINWINDOW_H_