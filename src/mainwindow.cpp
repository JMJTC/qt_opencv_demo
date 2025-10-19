#include "mainwindow.h"
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_pPlayer(new VideoPlayer(this)), m_pDisplayLabel(new QLabel(this)),
      m_pOpenButton(new QPushButton("打开", this)), m_pPlayButton(new QPushButton("播放", this)),
      m_pFilterCombo(new QComboBox(this)), m_pSlider(new QSlider(Qt::Horizontal, this)),
      m_currentMediaType(MediaType::None), m_currentFilter(FilterType::None), m_isPlaying(false)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    QHBoxLayout *toolbar = new QHBoxLayout();
    toolbar->addWidget(m_pOpenButton);
    toolbar->addWidget(m_pPlayButton);
    toolbar->addWidget(m_pFilterCombo);

    m_pFilterCombo->addItem("无滤镜");
    m_pFilterCombo->addItem("灰度");
    m_pFilterCombo->addItem("边缘检测");
    m_pFilterCombo->addItem("高斯模糊");

    mainLayout->addLayout(toolbar);
    mainLayout->addWidget(m_pDisplayLabel);
    mainLayout->addWidget(m_pSlider);

    setCentralWidget(central);
    setWindowTitle("Qt + OpenCV 图像/视频滤镜 Demo");
    resize(800, 600);

    m_pPlayButton->setEnabled(false);
    m_pSlider->setEnabled(false);
    m_pSlider->setRange(0, 1000); // 设置滑块的范围为0到1000

    connect(m_pOpenButton, &QPushButton::clicked, this, &MainWindow::openMedia);
    connect(m_pPlayButton, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(m_pPlayer, &VideoPlayer::frameReady, this, &MainWindow::onFrameReady);
    connect(m_pPlayer, &VideoPlayer::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_pSlider, &QSlider::sliderReleased, this, &MainWindow::onSliderReleased);
    connect(m_pFilterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterChanged);
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
    m_pPlayer->stop();
    m_isPlaying = false;
    m_pPlayButton->setText("播放");

    // 尝试先加载图片
    cv::Mat img = cv::imread(path.toStdString());
    if (!img.empty())
    {
        m_currentMediaType = MediaType::Image;
        m_currentImage = img;
        displayImage(ImageProcessor::applyFilter(img, m_currentFilter));

        m_pPlayButton->setEnabled(false);
        m_pSlider->setEnabled(false);
        return;
    }

    m_currentMediaType = MediaType::Video;
    // 否则尝试作为视频
    if (m_pPlayer->loadVideo(path))
    {
        m_pPlayButton->setEnabled(true);
        m_pSlider->setEnabled(true);
    }
    else
    {
        m_currentMediaType = MediaType::None;
        qWarning() << "无法加载媒体文件";
    }
}

void MainWindow::playPause()
{
    if (m_currentMediaType != MediaType::Video)
        return;
    if (m_isPlaying)
    {
        m_pPlayer->pause();
        m_pPlayButton->setText("播放");
    }
    else
    {
        m_pPlayer->play();
        m_pPlayButton->setText("暂停");
    }
    m_isPlaying = !m_isPlaying;
}

void MainWindow::onFrameReady(const QImage &frame)
{
    if (m_currentMediaType != MediaType::Video)
        return;
    m_pDisplayLabel->setPixmap(
        QPixmap::fromImage(frame).scaled(m_pDisplayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void MainWindow::onPositionChanged(double pos)
{
    if (m_currentMediaType != MediaType::Video)
        return;
    m_pSlider->blockSignals(true);
    m_pSlider->setValue(static_cast<int>(pos * 1000));
    m_pSlider->blockSignals(false);
}

void MainWindow::onSliderReleased()
{
    if (m_currentMediaType != MediaType::Video)
        return;
    double pos = m_pSlider->value() / 1000.0;
    m_pPlayer->setPosition(pos);
}

void MainWindow::onFilterChanged(int index)
{
    switch (index)
    {
    case 1:
        m_currentFilter = FilterType::Grayscale;
        break;
    case 2:
        m_currentFilter = FilterType::Edge;
        break;
    case 3:
        m_currentFilter = FilterType::Blur;
        break;
    default:
        m_currentFilter = FilterType::None;
    }

    if (m_currentMediaType == MediaType::Image && !m_currentImage.empty())
    {
        displayImage(ImageProcessor::applyFilter(m_currentImage, m_currentFilter));
    }
    else if (m_currentMediaType == MediaType::Video)
    {
        m_pPlayer->setFilter(m_currentFilter);
    }
}

void MainWindow::displayImage(const cv::Mat &img)
{
    if (img.empty())
        return;
    cv::Mat rgb;
    cv::cvtColor(img, rgb, cv::COLOR_BGR2RGB);
    QImage qimg(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888);
    m_pDisplayLabel->setPixmap(
        QPixmap::fromImage(qimg).scaled(m_pDisplayLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
