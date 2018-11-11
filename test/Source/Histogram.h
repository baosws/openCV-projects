#pragma once
#include <vector>
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

typedef const cv::Mat& Img;

// lớp bao bọc Histogram
class Histogram {
    // vector histogram
    std::vector<double> hist;
    int upper_bound = 256;

    // mặt nạ màu (dùng trong việc vẽ histogram lên ảnh)
    cv::Vec3b mask;
public:
    /**
     * hàm khởi tạo từ số bin màu số mức xám và mặt nạ màu
     * @bins: số bin
     * @upper_bound: cận trên/số mức xám
     * @mask: mặt nạ màu để vẽ (nếu cần)
     */
    Histogram(int bins = 256, int upper_bound = 256, const cv::Vec3b& mask = 0) : hist(bins), upper_bound(upper_bound), mask(mask) {}

    // phương thức lấy vector histogram
    std::vector<double> getHist() const {
        return hist;
    }

    // phương thức lấy mặt nạ màu
    cv::Vec3b getColorMask() const {
        return mask;
    }

    // phương thức lấy số bin
    int getBins() const {
        return hist.size();
    }

    // phương thức lấy số mức xám
    int getUpperBound() const {
        return upper_bound;
    }
    
    /**
     * phương thức tính histogram của một ảnh
     * @img: ảnh mà mình sẽ tính histogram
     * @channel: kênh màu dùng để tính histogram
     * @return: chính mình
     */
    Histogram& calculate(Img img, int channel) {
        /**
         * hàm lượng hóa màu
         * @i: mức sáng
         * @return: bin của mức sáng @i
         */
        auto get_bin = [&] (double i) -> int {
            return i * hist.size() / upper_bound;
        };

        // thống kê số pixel mỗi bin
        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                ++hist[get_bin(img.at<cv::Vec3b>(i, j)[channel])];
            }
        }

        // chuẩn hóa histogram, đưa histogram từ dạng tần số về dạng phân bố xác suất
        for (auto& bin : hist) {
            bin /= img.rows * img.cols;
        }

        return *this;
    }
};
