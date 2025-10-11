#ifndef _QTOPENCV_DEMO_MAINWINDOW_H_
#define _QTOPENCV_DEMO_MAINWINDOW_H_

#include "imageprocessor.h"
#include <QComboBox>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <opencv2/opencv.hpp>

class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

  private slots:
    void onOpenImage();
    void onFilterChanged(int index);

  private:
    void updateDisplay();

    QLabel *imageLabel = nullptr;
    QComboBox *filterCombo = nullptr;
    QPushButton *openButton = nullptr;

    cv::Mat originalImage{};
    cv::Mat processedImage{};
    FilterType currentFilter{};
};

#endif // _QTOPENCV_DEMO_MAINWINDOW_H_