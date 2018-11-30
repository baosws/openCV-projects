#include "EdgeDetect.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#include <iostream>
#include "/home/duongbao/code-hub/logger.hpp"

namespace EdgeDetect {
    const double PI = 3.14159265359;
    /**
     * hàm áp dụng toán tử chập
     * @h: ảnh chính
     * @kern: ảnh kernel
     * @return: ảnh kết quả chập, cùng kích thước với @h, đơn kênh, kiểu uchar
     */
    cv::Mat convolution(Img h, Img kern) {
        assert(kern.type()==CV_64FC1);

        cv::Mat res(h.rows, h.cols, CV_64FC1);
        int kern_size = kern.rows;
        assert(kern_size==kern.cols);

        for (int x = 0; x + kern_size <= h.rows; ++x) {
            for (int y = 0; y + kern_size <= h.cols; ++y) {
                double gxy = 0;
                for (int i = 0; i < kern_size; ++i) {
                    for (int j = 0; j < kern_size; ++j) {
                        gxy += h.at<uchar>(x + i, y + j) * kern.at<double>(kern_size - 1 - i, kern_size - 1 - j);
                    }
                }

                res.at<double>(x, y) = gxy;
            }
        }

        return res;
    }

    void print_kern(Img img) {
        for (int i = 0; i < img.rows; ++i, std::cout << std::endl) {
            for (int j = 0; j < img.cols; ++j) {
                std::cout << img.at<double>(i, j) << " ";
            }
        }
    }
    
    cv::Mat get_grad(Img img, const std::pair<cv::Mat, cv::Mat>& mask) {
        auto gx = convolution(img, mask.first);
        auto gy = convolution(img, mask.second);
        auto res = img.clone();
        for (int i = 0; i < res.rows; ++i) {
            for (int j = 0; j < res.cols; ++j) {
                res.at<uchar>(i, j) = cv::saturate_cast<uchar>(std::sqrt(std::pow(gx.at<double>(i, j), 2) + std::pow(gy.at<double>(i, j), 2)));
            }
        }
        return res;
    }

    std::pair<cv::Mat, cv::Mat> get_grad_mask(double k, double a = 1) {
        cv::Mat gx(3, 3, CV_64FC1, new double[9] {a, 0, -a, k, 0, -k, a, 0, -a});
        cv::Mat gy(3, 3, CV_64FC1, new double[9] {a, k, a, 0, 0, 0, -a, -k, -a});
        
        return {1.0 / (k + 2) * gx, 1.0 / (k + 2) * gy};
    }
    
    std::pair<cv::Mat, cv::Mat> get_sobel_kern() {
        return get_grad_mask(2);
    }

    std::pair<cv::Mat, cv::Mat> get_prewitt_kern() {
        return get_grad_mask(1);
    }

    std::pair<cv::Mat, cv::Mat> get_scharr_kern() {
        return get_grad_mask(10, 3);
    }

    std::pair<cv::Mat, cv::Mat> get_robert_kern() {
        cv::Mat gx(2, 2, CV_64FC1, new double[4] {1, 0, 0, -1});
        cv::Mat gy(2, 2, CV_64FC1, new double[4] {0, 1, -1, 0});
        return {gx, gy};
    }

    cv::Mat cmd_gra_sobel(Img img) {
        return get_grad(img, get_sobel_kern());
    }

    cv::Mat cmd_gra_prewitt(Img img) {
        return get_grad(img, get_prewitt_kern());
    }

    cv::Mat cmd_gra_scharr(Img img) {
        return get_grad(img, get_scharr_kern());
    }

    cv::Mat cmd_gra_roberts(Img img) {
        return get_grad(img, get_robert_kern());
    }

    cv::Mat get_laplacian_mask() {
        cv::Mat mask(3, 3, CV_64FC1, new double[9] {1, 1, 1, 1, -8, 1, 1, 1, 1});
        return mask;
    }
    
    cv::Mat cmd_laplacian(Img img) {
        return convolution(img, get_laplacian_mask());
    }

    cv::Mat get_log_mask(int kern_size, double sd) {
        cv::Mat kern(kern_size, kern_size, CV_64FC1);

        for (int x = -kern_size / 2; x <= kern_size / 2; ++x) {
            for (int y = -kern_size / 2; y <= kern_size / 2; ++y) {
                // g(x, y) = 1 / (2 * pi * sd^2) * e ^ (-(x^2 + y^2) / (2 * sd^2)
                kern.at<double>(x + kern_size / 2, y + kern_size / 2) = ((x * x + y * y) / 2.0 / sd / sd - 1) * std::exp(-(x * x + y * y) / 2.0 / sd / sd) / PI / std::pow(sd, 4);
            }
        }

        return kern;
    }
    
    cv::Mat cmd_log(Img img, int kern_size, double sd) {
        return convolution(img, get_log_mask(kern_size, sd));
    }
}
