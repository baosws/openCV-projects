#pragma once
#include <iostream>
#include <tuple>
#include <cassert>
#include "Histogram.h"
#include "HistogramDrawer.h"
#include "HistogramComparator.h"
#include "HistogramEqualizer.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

const int HIS_WIDTH = 640;
const int HIS_HEIGHT = 480;
const auto GRAY = cv::Vec3b(128, 128, 128);
const auto RED = cv::Vec3b(0, 0, 255);
const auto BLUE = cv::Vec3b(255, 0, 0);
const auto GREEN = cv::Vec3b(0, 255, 0);

typedef const cv::Mat& Img;


