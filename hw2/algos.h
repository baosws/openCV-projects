#pragma once
#include "Histogram.h"
#include "HistogramDrawer.h"
#include "HistogramComparator.h"
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

// hằng số cho công thức chuyển từ ảnh màu sang ảnh xám:
// y = b * 0.114 + g * 0.587 + r * 0.114
// với y là mức xám kết quả, r, g, b là mức đỏ, xanh lá và xanh dương
// https://docs.opencv.org/3.1.0/de/d25/imgproc_color_conversions.html
const double coeff[] = {0.114, 0.587, 0.299};

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

// hàm chuyển ảnh thành ảnh xám
// @img: ảnh cần chuyển
// return: ảnh màu xám tương ứng
cv::Mat convert_to_gray(Img img) {
    // tạo ảnh kết quả bằng cách copy từ ảnh ban đầu
    auto result = img.clone();

    // duyệt mỗi pixel (i, j) trong ảnh
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            auto& pix = result.at<cv::Vec3b>(i, j);

            // c = 0: blue
            // c = 1: green
            // c = 2: red
            double r = pix[2], g = pix[1], b = pix[0];
            double y = r * coeff[2] + g * coeff[1] + b * coeff[0];
            pix = cv::Vec3b(y, y, y);
        }
    }

    return result;
}

/**
 * hàm đổi màu ảnh xám thành ảnh màu (đỏ) và ngược lại
 * @img: ảnh cần đổi màu
 * return: nếu @img là ảnh xám thì trả về một ảnh màu đỏ tương ứng, ngược lại trả về ảnh xám tương ứng
 **/
cv::Mat revert_color(Img img) {
    if (is_grayscale(img)) { // nếu @img là ảnh xám
        // thì báo chỉ thực hiện với ảnh màu
        throw std::runtime_error("Lenh nay chi thuc hien voi anh mau");
    }
    else { // nếu @img là ảnh màu
        return convert_to_gray(img);
    }
}

/**
 * hàm thay đổi độ tương phản và độ sáng của ảnh
 * @img: ảnh đầu vào
 * @alpha: tỷ lệ thay đổi độ tương phản
 * @beta: độ tăng độ sáng
 * return: một ảnh mới đã có độ tương phản tăng lên @alpha lần và độ sáng tăng @beta đơn vị so với ảnh đầu vào
 **/
cv::Mat change_contrast_and_brightness(Img img, double alpha, double beta) {
    // tạo ảnh kết quả bằng cách copy từ ảnh ban đầu
    auto result = img.clone();

    // duyệt mỗi pixel (i, j) của ảnh
    for (int i = 0; i < result.rows; ++i) {
        for (int j = 0; j < result.cols; ++j) {

            // duyệt mỗi kênh màu của ảnh
            for (int c = 0; c < result.channels(); ++c) {
                // thay đổi độ tương phản và độ sáng bằng công thức:
                // g(i, j) = alpha * f(i, j) + beta
                // với g(i, j) là giá trị pixel (i, j) mới,
                // f(i, j) là giá trị pixel (i, j) cũ
                result.at<cv::Vec3b>(i, j)[c] =                   // đây là g(i, j) ở kênh màu thứ c
                    cv::saturate_cast<uchar>(                     // phải dùng saturate_cast để đưa g(i, j) về lại khoảng [0, 255]
                            alpha * img.at<cv::Vec3b>(i, j)[c] + beta // img.at<cv::Vec3b>(i, j)[c] là f(i, j) ở kênh màu c
                            );
            }
        }
    }
    return result;
}

/**
 * hàm biến đổi ảnh theo từng pixel sử dụng một hàm biến đổi cụ thể
 * @img: ảnh cần biến đổi
 * @func: hàm biến đổi trên từng pixel - giá trị mới của pixel có giá trị x là func(x)
 * @return: ảnh đã được biến đổi
 */
template <class Func>
cv::Mat map(Img img, Func func) {
    cv::Mat res = img.clone();
    
    // với mỗi pixcel (i, j) và kênh màu c, áp dụng hàm @func lên nó
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            auto& pix = res.at<cv::Vec3b>(i, j);
            for (int c = 0; c < img.channels(); ++c) {
                pix[c] = cv::saturate_cast<uchar>(func(pix[c]));
            }
        }
    }

    return res;
}

/**
 * hàm tạo ảnh âm bản từ 1 ảnh
 * @img: ảnh cần tạo ảnh âm bản
 * @return: ảnh âm bản được tạo ra
 */
cv::Mat invert(Img img) {
    // áp dụng hàm x -> 255 - x lên từng pixel và kênh màu của ảnh
    return map(img, [] (double x) {return 255 - x;});
}

/**
 * hàm biến đổi log transform
 * @img: ảnh cần biến đổi
 * @c: tham số trong biến đổi log
 * @return: ảnh đã được biến đổi
 */
cv::Mat log_transform(Img img, double c) {
    // áp dụng hàm x -> c * log(x + 1) lên từng pixel và kênh màu của ảnh
    return map(img, [c] (double x) {return c * std::log(x + 1);});
}

/**
 * hàm biến đổi gamma transform
 * @img: ảnh cần biến đổi
 * @gamma: tham số trong biến đổi log
 * @return: ảnh đã được biến đổi
 */
cv::Mat gamma_transform(Img img, double gamma) {
    // áp dụng hàm x -> gamma ^ x lên từng pixel và kênh màu của ảnh
    return map(img, [gamma] (double x) {return std::pow(x, gamma);});
}

/**
 * tính histogram xám của ảnh
 * @img: ảnh cần tính histogram
 * @num_bins: số bin trong histogram
 * @return: ảnh histogram
 */
cv::Mat get_gray_histogram(Img img, int num_bins) {
    // khai báo một ảnh trắng để vẽ
    cv::Mat res(HIS_WIDTH, HIS_HEIGHT, CV_8UC3, {255, 255, 255});

    // tính histogram của ảnh
    Histogram his(num_bins, GRAY);
    his.calculate(convert_to_gray(img), 0);

    // vẽ histogram đó lên ảnh
    HistogramDrawer().insert(his).draw(res);
    
    return res;
}

/**
 * tính histogram màu của ảnh
 * @img: ảnh cần tính histogram
 * @num_bins: số bin trong histogram
 * @return: ảnh histogram
 */
cv::Mat get_color_histogram(Img img, int num_bins) {
    // khai báo một ảnh trắng để vẽ
    cv::Mat res(HIS_HEIGHT, HIS_WIDTH, CV_8UC3, cv::Vec3b(255, 255, 255));

    // tính histogram trên mỗi kênh màu của ảnh
    Histogram his_r(num_bins, RED);
    his_r.calculate(img, 2);
    Histogram his_g(num_bins, GREEN);
    his_g.calculate(img, 1);
    Histogram his_b(num_bins, BLUE);
    his_b.calculate(img, 0);

    // vẽ histogram mỗi kênh màu lên ảnh
    HistogramDrawer()
        .insert(his_r)
        .insert(his_g)
        .insert(his_b)
        .draw(res);
    
    return res;
}

/**
 * hàm tính histogram của ảnh
 * @img: ảnh cần tính histogram
 * @num_bins: số bin trong histogram
 * @return: nếu ảnh là ảnh xám thì trả về histogram xám, ngược lại trả về histogram màu
 */
cv::Mat get_histogram(Img img, int num_bins) {
    // kiểm tra ảnh xám
    if (is_grayscale(img)) {
        return get_gray_histogram(img, num_bins);
    }
    
    return get_color_histogram(img, num_bins);
}

/**
 * hàm so sánh 2 histogram xám của 2 ảnh
 * @img1: ảnh thứ nhất
 * @img2: ảnh thứ 2
 * @num_bins: số bin trong histogram
 * @cmp: phương thức so sánh 2 histogram
 * @return: chênh lệch giữa 2 histogram xám
 */
double compare_gray_histogram(Img img1, Img img2, int num_bins, const HistogramComparator& cmp) {
    // chuyển mỗi ảnh sang ảnh xám
    // tính histogram xám mỗi ảnh
    // rồi so sánh histogram của 2 ảnh xám
    return cmp(Histogram(num_bins).calculate(convert_to_gray(img1), 0),
               Histogram(num_bins).calculate(convert_to_gray(img2), 0));
}

/**
 * hàm so sánh histogram màu của 2 ảnh
 * @img1: ảnh thứ nhất
 * @img2: ảnh thứ 2
 * @num_bins: số bin trong histogram
 * @cmp: phương thức so sánh histogram
 * @return: chênh lệch giữa 2 histogram màu
 */
double compare_color_histogram(Img img1, Img img2, int num_bins, const HistogramComparator& cmp) {
    // tính chênh lệch histogram mỗi kênh màu
    double diff_r = cmp(Histogram(num_bins).calculate(img1, 2),
                        Histogram(num_bins).calculate(img2, 2));
    double diff_g = cmp(Histogram(num_bins).calculate(img1, 1),
                        Histogram(num_bins).calculate(img2, 1));
    double diff_b = cmp(Histogram(num_bins).calculate(img1, 0),
                        Histogram(num_bins).calculate(img2, 0));

    // chênh lệch chung là tổng hợp chênh lệch của 3 kênh màu
    return std::sqrt(diff_b * diff_b + diff_g * diff_g + diff_r * diff_r);
}

/**
 * hàm so sánh histogram của 2 ảnh
 * @img1: ảnh thứ nhất
 * @img2: ảnh thứ 2
 * @num_bins: số bin trong histogram
 * @cmp: phương thức so sánh histogram
 * @return: chênh lệch giữa 2 histogram
 */
double compare_histogram(Img img1, Img img2, int num_bins, const HistogramComparator& cmp) {
    // nếu cả 2 là ảnh xám thì so sánh histogram xám
    if (is_grayscale(img1) && is_grayscale(img2)) {
        return compare_gray_histogram(img1, img2, num_bins, cmp);
    }
    
    return compare_color_histogram(img1, img2, num_bins, cmp);
}
