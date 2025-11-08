#include "imageprocessor.h"
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QString>
#include <memory>

cv::Mat ImageProcessor::applyFilter(const cv::Mat &input, FilterType type)
{
    if (input.empty())
        return cv::Mat();

    cv::Mat output;
    switch (type)
    {
    case FilterType::Grayscale:
        cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);
        cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
        break;
    case FilterType::Edge:
        cv::Canny(input, output, 100, 200);
        cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
        break;
    case FilterType::Blur:
        cv::GaussianBlur(input, output, cv::Size(9, 9), 2);
        break;
    case FilterType::FaceDetection:
        {
            // 创建人脸检测器实例（懒初始化），在多个可能的资源路径中查找模型文件，兼容不同工具链/IDE的工作目录差异
            static std::unique_ptr<FaceDetector> faceDetector = []() -> std::unique_ptr<FaceDetector> {
                QString appDir = QCoreApplication::applicationDirPath();
                const QString filename = "haarcascade_frontalface_default.xml";
                QStringList candidates;
                // 常见的候选路径：可执行文件同级的 assets/resources、可执行文件上一级的 resources/assets、当前工作目录下的路径等
                candidates << QDir(appDir).filePath("assets/models/" + filename)
                           << QDir(appDir).filePath("resources/models/" + filename)
                           << QDir(appDir).filePath("models/" + filename)
                           << QDir(appDir).filePath(filename)
                           << QDir(appDir).filePath("../resources/models/" + filename)
                           << QDir(appDir).filePath("../assets/models/" + filename)
                           << QDir::current().filePath("resources/models/" + filename)
                           << QDir::current().filePath("assets/models/" + filename)
                           ;

                for (const QString &c : candidates) 
                {
                    if (QFile::exists(c)) 
                    {
                        qDebug() << "Found face model at:" << c;
                        return std::make_unique<FaceDetector>(c.toStdString());
                    }
                }

                // 回退：尝试相对项目路径（保持向后兼容原来写法）
                QString fallback = QDir(appDir).filePath("../resources/models/" + filename);
                qWarning() << "未在候选路径中找到人脸模型，尝试回退路径：" << fallback;
                return std::make_unique<FaceDetector>(fallback.toStdString());
            }();

            if (!faceDetector || !faceDetector->isLoaded())
            {
                qWarning() << "人脸检测模型加载失败";
                output = input.clone();
                break;
            }

            // 检测人脸
            std::vector<cv::Rect> faces = faceDetector->detect(input);
            
            // 复制原图像
            output = input.clone();
            
            // 在检测到的人脸周围绘制矩形
            for (const auto& face : faces)
            {
                cv::rectangle(output, face, cv::Scalar(0, 255, 0), 2);
                
                // 在矩形上方添加文本标签
                cv::Point textPos(face.x, face.y - 10);
                if (textPos.y < 0) textPos.y = face.y + face.height + 20;
                cv::putText(output, "Face", textPos, cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            }
            
            // 如果没有检测到人脸，显示提示信息
            if (faces.empty())
            {
                cv::putText(output, "No face detected", cv::Point(10, 30), 
                           cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
            }
            else
            {
                // 显示检测到的人脸数量
                std::string countText = "Faces detected: " + std::to_string(faces.size());
                cv::putText(output, countText, cv::Point(10, 30), 
                           cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            }
        }
        break;
    default:
        output = input.clone();
    }
    return output;
}
