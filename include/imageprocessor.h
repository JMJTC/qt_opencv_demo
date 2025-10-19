#ifndef _QTOPENCV_DEMO_IMAGEPROCESSOR_H_
#define _QTOPENCV_DEMO_IMAGEPROCESSOR_H_

#include <opencv2/opencv.hpp>
#include <string>
#include "facedetector.h"

enum class FilterType
{
    None,
    Grayscale,
    Edge,
    Blur,
    FaceDetection
};

class ImageProcessor
{
  public:
    static cv::Mat applyFilter(const cv::Mat &input, FilterType type);
};

#endif // _QTOPENCV_DEMO_IMAGEPROCESSOR_H_