#pragma once
#include "opencv2/core.hpp"

typedef const cv::Mat& Img;
namespace EdgeDetect {
    cv::Mat cmd_gra_sobel(Img);
    cv::Mat cmd_gra_prewitt(Img);
    cv::Mat cmd_gra_scharr(Img);
    cv::Mat cmd_gra_roberts(Img);
    cv::Mat cmd_laplacian(Img);
    cv::Mat cmd_log(Img, int, double);
}
