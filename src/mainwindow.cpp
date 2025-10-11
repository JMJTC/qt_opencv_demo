#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), imageLabel(new QLabel(this)), filterCombo(new QComboBox(this)),
      openButton(new QPushButton("打开图片", this)), currentFilter(FilterType::None)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // 顶部工具栏
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    toolbarLayout->addWidget(openButton);
    toolbarLayout->addWidget(filterCombo);

    filterCombo->addItem("无滤镜");
    filterCombo->addItem("灰度");
    filterCombo->addItem("边缘检测");
    filterCombo->addItem("高斯模糊");

    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(imageLabel, 1);

    setCentralWidget(central);
    // setWindowTitle(QString::fromUtf8("Qt + OpenCV 基础图像处理 Demo"));
    resize(800, 600);

    connect(openButton, &QPushButton::clicked, this, &MainWindow::onOpenImage);
    connect(filterCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onFilterChanged);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onOpenImage()
{
    QString filePath = QFileDialog::getOpenFileName(this, "选择图片", "", "Images (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty())
    {
        originalImage = cv::imread(filePath.toStdString());
        cv::cvtColor(originalImage, originalImage, cv::COLOR_BGR2RGB);
        processedImage = originalImage.clone();
        currentFilter = FilterType::None;
        filterCombo->setCurrentIndex(0);
        updateDisplay();
    }
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
        break;
    }

    if (!originalImage.empty())
    {
        processedImage = ImageProcessor::applyFilter(originalImage, currentFilter);
        updateDisplay();
    }
}

void MainWindow::updateDisplay()
{
    if (processedImage.empty())
        return;

    QImage qimg(processedImage.data, processedImage.cols, processedImage.rows, processedImage.step,
                QImage::Format_RGB888);
    imageLabel->setPixmap(
        QPixmap::fromImage(qimg).scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
