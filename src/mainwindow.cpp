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
    m_pCameraButton = new QPushButton("打开摄像头", this);
    toolbar->addWidget(m_pCameraButton);
    toolbar->addWidget(m_pPlayButton);
    toolbar->addWidget(m_pFilterCombo);

    m_pFilterCombo->addItem("无滤镜");
    m_pFilterCombo->addItem("灰度");
    m_pFilterCombo->addItem("边缘检测");
    m_pFilterCombo->addItem("高斯模糊");
    m_pFilterCombo->addItem("人脸检测");

    // 设置人脸检测参数控件
    setupFaceDetectionControls();

    mainLayout->addLayout(toolbar);
    mainLayout->addWidget(m_pDisplayLabel);
    mainLayout->addWidget(m_pSlider);
    mainLayout->addWidget(m_pFaceDetectionGroup);

    setCentralWidget(central);
    setWindowTitle("Qt + OpenCV 图像/视频滤镜 Demo");
    resize(800, 700);

    m_pPlayButton->setEnabled(false);
    m_pSlider->setEnabled(false);
    m_pSlider->setRange(0, 1000); // 设置滑块的范围为0到1000

    connect(m_pOpenButton, &QPushButton::clicked, this, &MainWindow::openMedia);
    connect(m_pCameraButton, &QPushButton::clicked, this, &MainWindow::openCamera);
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

void MainWindow::openCamera()
{
    // 停止之前的媒体
    m_pPlayer->stop();
    m_isPlaying = false;
    m_pPlayButton->setText("播放");

    if (m_pPlayer->loadCamera(0))
    {
        m_currentMediaType = MediaType::Camera;
        m_pPlayButton->setEnabled(true);
        // 摄像头没有总帧数，不显示/使能滑块
        m_pSlider->setEnabled(false);
        // 自动开始播放摄像头
        m_pPlayer->setFilter(m_currentFilter);
        m_pPlayer->play();
        m_isPlaying = true;
        m_pPlayButton->setText("暂停");
    }
    else
    {
        m_currentMediaType = MediaType::None;
        qWarning() << "无法打开摄像头";
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
    case 4:
        m_currentFilter = FilterType::FaceDetection;
        break;
    default:
        m_currentFilter = FilterType::None;
    }

    // 显示/隐藏人脸检测参数控件
    m_pFaceDetectionGroup->setVisible(m_currentFilter == FilterType::FaceDetection);

    if (m_currentMediaType == MediaType::Image && !m_currentImage.empty())
    {
        displayImage(ImageProcessor::applyFilter(m_currentImage, m_currentFilter));
    }
    else if (m_currentMediaType == MediaType::Video)
    {
        m_pPlayer->setFilter(m_currentFilter);
    }
}

void MainWindow::setupFaceDetectionControls()
{
    m_pFaceDetectionGroup = new QGroupBox("人脸检测参数", this);
    QVBoxLayout *faceLayout = new QVBoxLayout(m_pFaceDetectionGroup);

    // 缩放因子
    QHBoxLayout *scaleLayout = new QHBoxLayout();
    scaleLayout->addWidget(new QLabel("缩放因子:", this));
    m_pScaleFactorSpin = new QDoubleSpinBox(this);
    m_pScaleFactorSpin->setRange(1.01, 2.0);
    m_pScaleFactorSpin->setSingleStep(0.01);
    m_pScaleFactorSpin->setValue(1.1);
    m_pScaleFactorSpin->setDecimals(2);
    scaleLayout->addWidget(m_pScaleFactorSpin);
    scaleLayout->addStretch();
    faceLayout->addLayout(scaleLayout);

    // 最小邻居数
    QHBoxLayout *neighborsLayout = new QHBoxLayout();
    neighborsLayout->addWidget(new QLabel("最小邻居数:", this));
    m_pMinNeighborsSpin = new QSpinBox(this);
    m_pMinNeighborsSpin->setRange(1, 10);
    m_pMinNeighborsSpin->setValue(3);
    neighborsLayout->addWidget(m_pMinNeighborsSpin);
    neighborsLayout->addStretch();
    faceLayout->addLayout(neighborsLayout);

    // 最小尺寸
    QHBoxLayout *minSizeLayout = new QHBoxLayout();
    minSizeLayout->addWidget(new QLabel("最小尺寸:", this));
    m_pMinSizeSpin = new QSpinBox(this);
    m_pMinSizeSpin->setRange(10, 2000);
    m_pMinSizeSpin->setValue(300);
    minSizeLayout->addWidget(m_pMinSizeSpin);
    minSizeLayout->addStretch();
    faceLayout->addLayout(minSizeLayout);

    // 最大尺寸
    QHBoxLayout *maxSizeLayout = new QHBoxLayout();
    maxSizeLayout->addWidget(new QLabel("最大尺寸:", this));
    m_pMaxSizeSpin = new QSpinBox(this);
    m_pMaxSizeSpin->setRange(50, 5000);
    m_pMaxSizeSpin->setValue(600);
    maxSizeLayout->addWidget(m_pMaxSizeSpin);
    maxSizeLayout->addStretch();
    faceLayout->addLayout(maxSizeLayout);

    // 连接信号
    connect(m_pScaleFactorSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this,
            &MainWindow::onFaceDetectionParamsChanged);
    connect(m_pMinNeighborsSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onFaceDetectionParamsChanged);
    connect(m_pMinSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onFaceDetectionParamsChanged);
    connect(m_pMaxSizeSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onFaceDetectionParamsChanged);

    // 初始时隐藏人脸检测参数组
    m_pFaceDetectionGroup->setVisible(false);
}

void MainWindow::updateFaceDetectionParams()
{
    if (m_currentFilter != FilterType::FaceDetection)
        return;

    // 这里可以更新全局的人脸检测器参数
    // 由于我们使用的是静态实例，参数会在检测时自动应用
}

void MainWindow::onFaceDetectionParamsChanged()
{
    if (m_currentFilter == FilterType::FaceDetection)
    {
        updateFaceDetectionParams();

        // 如果当前是图像模式，重新处理图像
        if (m_currentMediaType == MediaType::Image && !m_currentImage.empty())
        {
            displayImage(ImageProcessor::applyFilter(m_currentImage, m_currentFilter));
        }
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
