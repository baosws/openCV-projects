#include "Filters.hpp"
#include <cassert>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

namespace Filters {
    const double PI = 3.14159265359;

    typedef const cv::Mat& Img;

    /**
     * hàm kiểm tra ảnh có phải ảnh xám hay không
     * @img: ảnh cần kiểm tra
     * return: true nếu @img là ảnh xám, ngược lại false
     **/
    bool is_grayscale(Img img) {
        // duyệt từng pixel (i, j) trong ảnh
        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                // lấy thông tin 3 kênh màu Red, Blue, Green của ảnh
                cv::Vec3b pixel = img.at<cv::Vec3b>(i, j);
                auto R = pixel[2], G = pixel[1], B = pixel[0];

                // ảnh là ảnh xám nếu R = G = B, ngược lại là ảnh màu
                if (R != G || G != B) {
                    return false;
                }
            }
        }

        return true;
    }

    /**
     * hàm tạo kernel cho toán tử trung bình
     * @kern_size: kích thước kernel
     * @return: kernel cho toán tử trung bình, kích thước kernel x kernel, đơn kênh, kiểu double
     */
    cv::Mat get_mean_kernel(int kern_size) {
        return 1.0 / (kern_size * kern_size) * cv::Mat::ones(kern_size, kern_size, CV_64FC1);
    }

    /**
     * hàm tạo kernel cho toán tử Gaussian
     * @kern_size: kích thước kernel
     * @sd: độ lệch chuẩn trong phân phối Gaussian
     * @return: kernel toán tử Gaussian, kích thước kernel x kernel, đơn kênh, kiểu double
     */
    cv::Mat get_gaussian_kernel(int kern_size, double sd) {
        cv::Mat kern(kern_size, kern_size, CV_64FC1);

        for (int x = -kern_size / 2; x <= kern_size / 2; ++x) {
            for (int y = -kern_size / 2; y <= kern_size / 2; ++y) {
                // g(x, y) = 1 / (2 * pi * sd^2) * e ^ (-(x^2 + y^2) / (2 * sd^2)
                kern.at<double>(x + kern_size / 2, y + kern_size / 2) = std::exp(-(x * x + y * y) / (2 * sd * sd)) / (2 * PI * sd * sd);
            }
        }

        return kern;
    }

    /**
     * hàm áp dụng toán tử chập
     * @h: ảnh chính
     * @kern: ảnh kernel
     * @return: ảnh kết quả chập, cùng kích thước với @h, đơn kênh, kiểu uchar
     */
    cv::Mat convolution(Img h, Img kern) {
        assert(kern.type()==CV_64DC1);
        
        cv::Mat res(h.rows, h.cols, CV_8UC1);
        int kern_size = kern.rows;
        assert(kern_size==kern.cols);

        for (int x = 0; x < h.rows; ++x) {
            for (int y = 0; y < h.cols; ++y) {
                float gxy = 0;
                for (int i = -kern_size / 2; i <= kern_size / 2; ++i) {
                    for (int j = -kern_size / 2; j <= kern_size / 2; ++j) {
                        if (x - i >= 0 && x - i < h.rows && y - j >= 0 && y - j < h.cols) {
                            gxy += h.at<uchar>(x - i, y - j) * kern.at<double>(i + kern_size / 2, j + kern_size / 2);
                        }
                    }
                }

                res.at<uchar>(x, y) = cv::saturate_cast<uchar>(gxy);
            }
        }

        return res;
    }

    /**
     * hàm áp dụng phép lọc trung bình trên ảnh đơn kênh
     * @img: ảnh đầu vào
     * @kern_size: kích thước kernel cho toán tử trung bình
     */
    cv::Mat mean_sgc(Img img, int kern_size) {
        assert(img.channel()==1);
        auto kern = get_mean_kernel(kern_size);
        return convolution(img, kern);
    }

    cv::Mat median_sgc(Img img, int kern_size) {
        assert(img.channel()==1);
        cv::Mat res(img.rows, img.cols, CV_8UC1);
        
        for (int x = 0; x < img.rows; ++x) {
            for (int y = 0; y < img.cols; ++y) {
                
                std::vector<uchar> locals;
                for (int i = -kern_size / 2; i <= kern_size / 2; ++i) {
                    for (int j = -kern_size / 2; j <= kern_size / 2; ++j) {
                        if (x - i >= 0 && x - i < img.rows && y - j >= 0 && y - j < img.cols) {
                            locals.push_back(img.at<uchar>(x - i, y - j));
                        }
                    }
                }

                sort(locals.begin(), locals.end());
                res.at<uchar>(x, y) = locals[locals.size() / 2];
            }
        }

        return res;
    }

    /**
     * hàm áp dụng phép lọc Gaussian trên ảnh đơn kênh
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     * @sd: độ lệch chuẩn trong phân phối Gaussian
     */
    cv::Mat gaussian_sgc(Img img, int kern_size, double sd) {
        assert(img.channel()==1);
        auto kern = get_gaussian_kernel(kern_size, sd);
        return convolution(img, kern);
    }

    /**
     * hàm áp dụng phép lọc Trung bình trên ảnh đơn kênh
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     */
    cv::Mat mean_gray(Img img, int kern_size) {
        if (!is_grayscale(img)) {
            throw std::invalid_argument("mean_gray expected an grayscale image");
        }
        cv::Mat gray_img;
        cvtColor(img, gray_img, CV_BGR2GRAY);
        return mean_sgc(gray_img, kern_size);
    }

    /**
     * hàm áp dụng phép lọc Trung bình trên ảnh màu
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     */
    cv::Mat mean_color(Img img, int kern_size) {
        cv::Mat bgr[3];
        // tách 3 kênh màu ra
        cv::split(img, bgr);
        // áp dụng phép lọc trên từng kênh
        for (auto& chan: bgr) {
            chan = mean_sgc(chan, kern_size);
        }
        cv::Mat res;
        // rồi gộp 3 kênh lại
        cv::merge(bgr, 3, res);
        return res;
    }

    /**
     * hàm áp dụng phép lọc Trung vị trên ảnh xám
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     */
    cv::Mat median_gray(Img img, int kern_size) {
        if (!is_grayscale(img)) {
            throw std::invalid_argument("median_gray expected an grayscale image");
        }
        cv::Mat gray_img;
        cvtColor(img, gray_img, CV_BGR2GRAY);
        return median_sgc(gray_img, kern_size);
    }

    /**
     * hàm áp dụng phép lọc Trung vị trên ảnh màu
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     */
    cv::Mat median_color(Img img, int kern_size) {
        cv::Mat bgr[3];
        // tách 3 kênh màu ra
        cv::split(img, bgr);
        // áp dụng phép lọc trên từng kênh
        for (auto& chan: bgr) {
            chan = median_sgc(chan, kern_size);
        }
        cv::Mat res;
        //  rồi gộp lại
        cv::merge(bgr, 3, res);
        return res;
    }

    /**
     * hàm áp dụng phép lọc Gaussian trên ảnh xám
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     * @sd: độ lệch chuẩn trong phân phối Gaussian
     */
    cv::Mat gaussian_gray(Img img, int kern_size, double sd) {
        if (!is_grayscale(img)) {
            throw std::invalid_argument("gaussian_gray expected an grayscale image");
        }
        cv::Mat gray_img;
        cvtColor(img, gray_img, CV_BGR2GRAY);
        return gaussian_sgc(gray_img, kern_size, sd);
    }

    /**
     * hàm áp dụng phép lọc Gaussian trên ảnh màu
     * @img: ảnh đầu vào
     * @kern_size: kích thước nhân
     * @sd: độ lệch chuẩn trong phân phối Gaussian
     */
    cv::Mat gaussian_color(Img img, int kern_size, double sd) {
        cv::Mat bgr[3];
        // tách 3 kênh màu ra
        cv::split(img, bgr);
        // áp dụng phép lọc trên từng kênh
        for (auto& chan: bgr) {
            chan = gaussian_sgc(chan, kern_size, sd);
        }
        cv::Mat res;
        // rồi gộp 3 kênh lại
        cv::merge(bgr, 3, res);
        return res;
    }
}
