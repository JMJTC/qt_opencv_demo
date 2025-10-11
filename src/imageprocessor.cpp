#include "imageprocessor.h"

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
    default:
        output = input.clone();
    }
    return output;
}
