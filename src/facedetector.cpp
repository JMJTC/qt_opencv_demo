#include "facedetector.h"

FaceDetector::FaceDetector(const std::string &modelPath)
{
    m_loaded = m_faceCascade.load(modelPath);
}

bool FaceDetector::isLoaded() const
{
    return m_loaded;
}

/**
 * @brief 检测输入图像中的人脸
 * @param frame 输入的图像帧，类型为cv::Mat
 * @return 返回检测到的人脸矩形区域向量，如果没有检测到则返回空向量
 */
std::vector<cv::Rect> FaceDetector::detect(const cv::Mat &frame)
{
    std::vector<cv::Rect> faces; // 存储检测到的人脸矩形区域
    // 检查模型是否加载以及图像是否为空
    if (!m_loaded || frame.empty())
        return faces;

    cv::Mat gray; // 用于存储灰度图像
    // 将彩色图像转换为灰度图像，因为人脸检测通常在灰度图像上进行
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    // 直方图均衡化，增强图像对比度，提高检测效果
    cv::equalizeHist(gray, gray);
    // 使用Haar级联分类器检测人脸
    // 参数：输入图像、检测到的矩形向量、缩放因子、最小邻居数、标志、最小人脸尺寸、最大人脸尺寸
    m_faceCascade.detectMultiScale(gray, faces, m_scaleFactor, m_minNeighbors, 0, m_minSize, m_maxSize);
    return faces; // 返回检测到的人脸矩形区域
}

/**
 * @brief 设置检测参数
 * @param scaleFactor 图像缩放因子
 * @param minNeighbors 最小邻居数
 * @param minSize 最小人脸尺寸
 * @param maxSize 最大人脸尺寸
 */
void FaceDetector::setDetectionParams(double scaleFactor, int minNeighbors, 
                                     const cv::Size& minSize, const cv::Size& maxSize)
{
    m_scaleFactor = scaleFactor;
    m_minNeighbors = minNeighbors;
    m_minSize = minSize;
    m_maxSize = maxSize;
}