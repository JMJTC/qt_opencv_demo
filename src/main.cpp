#include <QApplication>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <opencv2/opencv.hpp>

// OpenCV -> QImage 转换函数
QImage cvMatToQImage(const cv::Mat &mat)
{
    if (mat.type() == CV_8UC3)
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped(); // BGR 转 RGB
    }
    else if (mat.type() == CV_8UC1)
    {
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        return image;
    }
    else
    {
        return QImage();
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 1. 读取图像
    cv::Mat img = cv::imread("../images/test.jpg");
    if (img.empty())
    {
        qWarning("图像加载失败！");
        return -1;
    }

    // 2. 转换为QImage
    QImage qimg = cvMatToQImage(img);

    // 3. QLabel 显示
    QLabel label;
    label.setPixmap(QPixmap::fromImage(qimg));
    label.setWindowTitle("Qt + OpenCV Demo");
    label.show();

    return app.exec();
}
