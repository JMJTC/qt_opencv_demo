#ifndef _QTOPENCV_DEMO_FACEDETECTOR_H_
#define _QTOPENCV_DEMO_FACEDETECTOR_H_

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

/**
 * @class FaceDetector
 * @brief 人脸检测器类，用于检测图像中的人脸
 */
class FaceDetector
{
  public:
    /**
     * @brief 构造函数，加载人脸检测模型
     * @param modelPath 模型文件的路径
     */
    FaceDetector(const std::string &modelPath);
    /**
     * @brief 检查模型是否成功加载
     * @return 如果模型加载成功返回true，否则返回false
     */
    bool isLoaded() const;
    /**
     * @brief 在图像中检测人脸
     * @param frame 输入的图像帧
     * @return 返回检测到的人脸矩形区域列表
     */
    std::vector<cv::Rect> detect(const cv::Mat &frame);

    /**
     * @brief 设置检测参数
     * @param scaleFactor 图像缩放因子
     * @param minNeighbors 最小邻居数
     * @param minSize 最小人脸尺寸
     * @param maxSize 最大人脸尺寸
     */
    void setDetectionParams(double scaleFactor, int minNeighbors, const cv::Size &minSize, const cv::Size &maxSize);

    /**
     * @brief 获取当前检测参数
     */
    double getScaleFactor() const
    {
        return m_scaleFactor;
    }
    int getMinNeighbors() const
    {
        return m_minNeighbors;
    }
    cv::Size getMinSize() const
    {
        return m_minSize;
    }
    cv::Size getMaxSize() const
    {
        return m_maxSize;
    }

  private:
    cv::CascadeClassifier m_faceCascade{}; ///< OpenCV的人脸分类器对象
    bool m_loaded{false};                  ///< 标记模型是否成功加载

    // 检测参数
    double m_scaleFactor{1.1};    ///< 图像缩放因子
    int m_minNeighbors{3};        ///< 最小邻居数
    cv::Size m_minSize{20, 20};   ///< 最小人脸尺寸
    cv::Size m_maxSize{700, 700}; ///< 最大人脸尺寸
};
#endif // _QTOPENCV_DEMO_FACEDETECTOR_H_