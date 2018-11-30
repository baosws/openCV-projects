#pragma once
#include "opencv2/core/core.hpp"

namespace Filters {
    typedef const cv::Mat& Img;
    cv::Mat mean_gray(Img, int);
    cv::Mat mean_color(Img, int);
    cv::Mat median_gray(Img, int);
    cv::Mat median_color(Img, int);
    cv::Mat gaussian_gray(Img, int, double);
    cv::Mat gaussian_color(Img, int, double);
}
