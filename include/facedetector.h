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

  private:
    cv::CascadeClassifier faceCascade_; ///< OpenCV的人脸分类器对象
    bool loaded_;                       ///< 标记模型是否成功加载
};
#endif // _QTOPENCV_DEMO_FACEDETECTOR_H_