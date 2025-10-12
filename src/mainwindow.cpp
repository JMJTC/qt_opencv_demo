#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), player(new VideoPlayer(this)), displayLabel(new QLabel(this)),
      openButton(new QPushButton("打开", this)), playButton(new QPushButton("播放", this)),
      filterCombo(new QComboBox(this)), slider(new QSlider(Qt::Horizontal, this)), currentMediaType(MediaType::None),
      currentFilter(FilterType::None), isPlaying(false)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->addWidget(openButton);
    toolbar->addWidget(playButton);
    toolbar->addWidget(filterCombo);

    filterCombo->addItem("无滤镜");
    filterCombo->addItem("灰度");
    filterCombo->addItem("边缘检测");
    filterCombo->addItem("高斯模糊");

    mainLayout->addLayout(toolbar);
    mainLayout->addWidget(displayLabel);
    mainLayout->addWidget(slider);

    setCentralWidget(central);
    setWindowTitle("Qt + OpenCV 图像/视频滤镜 Demo");
    resize(800, 600);

    playButton->setEnabled(false);
    slider->setEnabled(false);
    slider->setRange(0, 1000); // 设置滑块的范围为0到1000

    connect(openButton, &QPushButton::clicked, this, &MainWindow::openMedia);
    connect(playButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(player, &VideoPlayer::frameReady, this, &MainWindow::onFrameReady);
    connect(player, &VideoPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(slider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterChanged);
}

MainWindow::~MainWindow()
{
}

void MainWindow::openMedia()
{
    QString path =
        QFileDialog::getOpenFileName(this, "选择文件", "", "Images and Videos (*.png *.jpg *.jpeg *.bmp *.mp4 *.avi)");
    if (path.isEmpty())
        return;

    // 停止视频播放
    player->stop();
    isPlaying = false;
    playButton->setText("播放");

    // 尝试先加载图片
    cv::Mat img = cv::imread(path.toStdString());
    if (!img.empty())
    {
        currentMediaType = MediaType::Image;
        currentImage = img;
        displayImage(ImageProcessor::applyFilter(img, currentFilter));

        playButton->setEnabled(false);
        slider->setEnabled(false);
        return;
    }

    currentMediaType = MediaType::Video;
    // 否则尝试作为视频
    if (player->loadVideo(path))
    {
        playButton->setEnabled(true);
        slider->setEnabled(true);
    }
    else
    {
        currentMediaType = MediaType::None;
        qWarning() << "无法加载媒体文件";
    }
}

void MainWindow::playPause()
{
    if (currentMediaType != MediaType::Video)
        return;
    if (isPlaying)
    {
        player->pause();
        playButton->setText("播放");
    }
    else
    {
        player->play();
        playButton->setText("暂停");
    }
    isPlaying = !isPlaying;
}

void MainWindow::onFrameReady(const QImage &frame)
{
    if (currentMediaType != MediaType::Video)
        return;
    displayLabel->setPixmap(
        QPixmap::fromImage(frame).scaled(displayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onPositionChanged(double pos)
{
    if (currentMediaType != MediaType::Video)
        return;
    slider->blockSignals(true);
    slider->setValue(static_cast<int>(pos * 1000));
    slider->blockSignals(false);
}

void MainWindow::onSliderReleased()
{
    if (currentMediaType != MediaType::Video)
        return;
    double pos = slider->value() / 1000.0;
    player->setPosition(pos);
}

void MainWindow::onFilterChanged(int index)
{
    switch (index)
    {
    case 1:
        currentFilter = FilterType::Grayscale;
        break;
    case 2:
        currentFilter = FilterType::Edge;
        break;
    case 3:
        currentFilter = FilterType::Blur;
        break;
    default:
        currentFilter = FilterType::None;
    }

    if (currentMediaType == MediaType::Image && !currentImage.empty())
    {
        displayImage(ImageProcessor::applyFilter(currentImage, currentFilter));
    }
    else if (currentMediaType == MediaType::Video)
    {
        player->setFilter(currentFilter);
    }
}

void MainWindow::displayImage(const cv::Mat &img)
{
    if (img.empty())
        return;
    cv::Mat rgb;
    cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
    QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    displayLabel->setPixmap(
        QPixmap::fromImage(qimg).scaled(displayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
