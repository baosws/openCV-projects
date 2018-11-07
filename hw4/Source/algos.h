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
 * tính histogram xám của ảnh
 * @img: ảnh cần tính histogram
 * @num_bins: số bin trong histogram
 * @return: ảnh histogram
 */
cv::Mat get_gray_histogram(Img img, int num_bins) {
    // khai báo một ảnh trắng để vẽ
    cv::Mat res(HIS_HEIGHT, HIS_WIDTH, CV_8UC3, {255, 255, 255});

    // tính histogram của ảnh
    Histogram his(num_bins, 256, GRAY);
    his.calculate(convert_to_gray(img), 0);

    // vẽ histogram đó lên ảnh
    HistogramDrawer().insert(his).draw(res);
    
    return res;
}

/**
 * hàm cân bằng histogram ảnh xám
 * @img: ảnh cần câng bằng
 * @return: ảnh đã được cân bằng
 */
cv::Mat hqgray(Img img) {
    // nếu không phải ảnh xám thì báo lỗi
    if (!is_grayscale(img)) {
        throw std::runtime_error("Khong phai anh xam");
    }
    // tính histogram và cân bằng
    HistogramEqualizer he(Histogram().calculate(img, 0));

    // tạo ảnh kết quả
    auto res = img.clone();
    he.apply(res, 0)
      .apply(res, 1)
      .apply(res, 2);

    return res;
}

/**
 * hàm cân bằng 3 kênh độc lập của ảnh rgb
 * @img: ảnh cần cân bằng
 * @return: ảnh đã cân bằng
 */
cv::Mat hqrgb(Img img) {
    auto res = img.clone();

    for (int c = 0; c < img.channels(); ++c) {
        HistogramEqualizer(Histogram(256).calculate(img, c)).apply(res, c);
    }

    return res;
}

/**
 * hàm chuyển pixel từ không gian rgb sang hsv
 * tham khảo: https://docs.opencv.org/3.1.0/de/d25/imgproc_color_conversions.html
 * @rgb_pixel: giá trị pixel trong không gian rgb
 * @return: giá trị pixel tương ứng ở không gian hsv
 */
cv::Vec3b rgb_to_hsv(cv::Vec3b rgb_pixel) {
    double r = rgb_pixel[2] / 255.0, g = rgb_pixel[1] / 255.0, b = rgb_pixel[0] / 255.0;

    double h = 0, s = 0, v = std::max(r, std::max(g, b)), min_rgb = std::min(r, std::min(g, b));
    if (v != 0) {
        s = (v - min_rgb) / v;
    }

    if (v != min_rgb) {
        if (v == r) {
            h = 0 +  60 * (g - b) / (v - min_rgb); // -60 -> 60
        }
        else if (v == g) {
            h = 120 + 60 * (b - r) / (v - min_rgb); // 60 -> 180
        }
        else {
            h = 240 + 60 * (r - g) / (v - min_rgb); // 180 -> 300
        }
    }

    if (h < 0) {
        h += 360; // 0 -> 360
    }

    v *= 255;
    s *= 255;
    h /= 2;

    return cv::Vec3b(h, s, v);
}

/**
 * hàm chuyển pixel từ không gian hsv sang rgb
 * tham khảo: https://stackoverflow.com/questions/3018313/algorithm-to-convert-rgb-to-hsv-and-hsv-to-rgb-in-range-0-255-for-both
 * @hsv_pixel: giá trị pixel trong không gian hsv
 * @return: giá trị pixel tương ứng ở không gian rgb
 */
cv::Vec3b hsv_to_rgb(cv::Vec3b hsv_pixel) {
    double h = hsv_pixel[0] * 2, s = hsv_pixel[1] / 255.0, v = hsv_pixel[2] / 255.0;

    double hh = h / 60;
    int i = hh;
    double ff = hh - i;
    auto p = v * (1 - s);
    auto q = v * (1 - s * ff);
    auto t = v * (1 - (s * (1 - ff)));
    double r, g, b;
    switch (i) {
        case 0:
            std::tie(r, g, b) = std::make_tuple(v, t, p);
            break;
        case 1:
            std::tie(r, g, b) = std::make_tuple(q, v, p);
            break;
        case 2:
            std::tie(r, g, b) = std::make_tuple(p, v, t);
            break;
        case 3:
            std::tie(r, g, b) = std::make_tuple(p, q, v);
            break;
        case 4:
            std::tie(r, g, b) = std::make_tuple(t, p, v);
            break;
        case 5:
            std::tie(r, g, b) = std::make_tuple(v, p, q);
            break;
    }
    assert(((b<=1)&&(b>=0)));
    assert(((g<=1)&&(g>=0)));
    assert(((r<=1)&&(r>=0)));
    return cv::Vec3b(b * 255, g * 255, r * 255);
}

/**
 * hàm biến đổi ảnh bằng một ánh xạ trên mỗi pixel
 * @img: ảnh cần biến đổi
 * @func: ánh xạ biến đổi ảnh
 * @return: ảnh đã được biến đổi
 */
template <class Func>
cv::Mat map_pixels(Img img, Func func) {
    auto res = img.clone();

    // áp dụng hàm @func trên mọi pixel trong ảnh
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            res.at<cv::Vec3b>(i, j) = func(img.at<cv::Vec3b>(i, j));
        }
    }
    
    return res;
}

/**
 * hàm cân bằng kênh h của ảnh
 * @img: ảnh cần cân bằng
 * @return: ảnh đã được cân bằng kênh h
 */
cv::Mat hqhsv(Img rgb_img) {
    auto hsv_img = map_pixels(rgb_img, rgb_to_hsv);

    // cân bằng kênh h với 180 bin
    HistogramEqualizer(Histogram(180, 180).calculate(hsv_img, 0)).apply(hsv_img, 0);

    // chuyển từ hệ hsv sang rgb và trả ra kết quả
    return map_pixels(hsv_img, hsv_to_rgb);
}

/**
 * hàm cân bằng 3 kênh màu rgb sử dụng opencv
 * @img: ảnh cần cân bằng
 * @return: ảnh đã được cân bằng
 */
cv::Mat hqrgb_opencv(Img img) {
    cv::Mat bgr[3];
    // tách 3 kênh b, g, r vào mảng bgr
    cv::split(img, bgr);

    cv::Mat bgr_equalized[3];
    // cân bằng 3 kênh và lưu vào mảng bgr_equalized
    for (int i : {0, 1, 2}) {
        cv::equalizeHist(bgr[i], bgr_equalized[i]);
    }

    cv::Mat res;
    // gộp 3 kênh vào ảnh kết quả
    cv::merge(bgr_equalized, 3, res);

    return res;
}

/**
 * hàm cân bằng ảnh xám sử dụng opencv
 * @img: ảnh cần cân bằng
 * @return: ảnh xám đã được cân bằng
 */
cv::Mat hqgray_opencv(Img img) {
    if (!is_grayscale(img)) {
        throw std::runtime_error("Khong phai anh xam");
    }

    return hqrgb_opencv(img);
}

/**
 * hàm cân bằng kênh h của ảnh sử dụng opencv
 * @img: ảnh cần cân bằng
 * @return: ảnh đã được cân bằng
 */
cv::Mat hqhsv_opencv(Img img) {
    cv::Mat hsv_img;
    // đầu tiên chuyển hệ màu từ rgb sang hsv
    cv::cvtColor(img, hsv_img, cv::COLOR_BGR2HSV);

    cv::Mat hsv[3];
    // tách 3 kênh h, s, v lưu vào mảng hsv
    cv::split(hsv_img, hsv);

    // cân bằng kênh h
    cv::equalizeHist(hsv[0], hsv[0]);

    cv::Mat res;
    // gộp 3 kênh h, s, v vào ảnh kết quả
    cv::merge(hsv, 3, res);

    cv::Mat rgb_img;
    // chuyển ngược từ hệ màu hsv sang rgb
    cv::cvtColor(res, rgb_img, cv::COLOR_HSV2BGR);
    return rgb_img;
}
