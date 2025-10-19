#include "imageprocessor.h"
#include <QDebug>

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
            // 创建人脸检测器实例
            static FaceDetector faceDetector("resources/models/haarcascade_frontalface_default.xml");
            
            if (!faceDetector.isLoaded())
            {
                qWarning() << "人脸检测模型加载失败";
                output = input.clone();
                break;
            }
            
            // 检测人脸
            std::vector<cv::Rect> faces = faceDetector.detect(input);
            
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
