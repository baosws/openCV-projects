#pragma once
#pragma once
#include <vector>
#include "Histogram.h"
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

class HistogramEqualizer {
    std::vector<double> T;
public:
    /**
     * hàm khởi tạo histogram equalizer từ histogram, nó sẽ tự động cân bằng histogram này mà không cần gọi hàm equalize
     * @h: histogram cần cân bằng
     */
    HistogramEqualizer(const Histogram& h) {
        equalize(h);
    }

    /**
     * phương thức cân bằng histogram
     * @h: histogram cần cân bằng
     * @return: *this
     */
    HistogramEqualizer& equalize(const Histogram& h) {
        T = h.getHist();
        for (int i = 1; i < h.getBins(); ++i) {
            T[i] += T[i - 1];
        }
        for (auto& t : T) {
            t *= (h.getUpperBound() - 1);
        }
        return *this;
    }

    HistogramEqualizer& apply(cv::Mat& res, int channel) {
        for (int i = 0; i < res.rows; ++i) {
            for (int j = 0; j < res.cols; ++j) {
                res.at<cv::Vec3b>(i, j)[channel] = cv::saturate_cast<uchar>(T[res.at<cv::Vec3b>(i, j)[channel]]);
            }
        }
        return *this;
    }
};
