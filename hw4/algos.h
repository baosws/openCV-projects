#pragma once
#include <iostream>
#include <tuple>
#include <cassert>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#include <cmath>
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

const double PI = 3.14159265359;

typedef const cv::Mat& Img;

/**
 * hàm lấy vùng không gian ảnh sau khi đã biến đổi hình học
 * việc này được thực hiện bằng cách áp dụng phép biến đổi lên 4 đỉnh góc của ảnh
 * 
 * @width: chiều rộng ban đầu
 * @height: chiều cao ban đầu
 * @transform_mat: ma trận biến đổi
 * @return: hình chữ nhật mô tả vùng không gian sau khi biến đổi, gồm có tọa độ góc trên trái, chiều rộng và chiều cao
 */
template <class Matx>
cv::Rect transformedRect(int width, int height, Matx transform_mat) {
    int xmin = INT_MAX, ymin = INT_MAX, xmax = INT_MIN, ymax = INT_MIN;
    for (int x = 0; x < 2; ++x) {
        for (int y = 0; y < 2; ++y) {
            auto newPos = transform_mat * cv::Matx21d(x * height, y * width);
            auto xnew = std::round(newPos(0, 0));
            auto ynew = std::round(newPos(1, 0));
            xmax = std::max(xmax, (int)xnew);
            xmin = std::min(xmin, (int)xnew);
            ymax = std::max(ymax, (int)ynew);
            ymin = std::min(ymin, (int)ynew);
        }
    }
    return {ymin, xmin, ymax - ymin + 1, xmax - xmin + 1};
}

/**
 * hàm áp dụng phép biến đổi hình học lên ảnh
 * thuật toán:
 *     - B1: tính kích thước ảnh mới
 *     - B2: với mỗi pixel trên ảnh mới:
 *         + Tìm pixel tương ứng trên ảnh cũ bằng phép biến đổi nghịch đảo
 *         + Tính giá trị nội suy của pixel mới bằng 4 pixel lân cận trên ảnh cũ
 * @img: ảnh đầu vào
 * @transform_mat: ma trận biến đổi
 * @return: ảnh sau khi biến đổi
 */
template <class Matx>
cv::Mat applyTransform(Img img, Matx transform_mat) {
    // tính vùng không gian ảnh mới
    cv::Rect transformed_region = transformedRect(img.cols, img.rows, transform_mat);
    
    // ảnh kết quả
    cv::Mat res(transformed_region.size(), CV_8UC3);
    
    auto f = [&] (int x, int y) -> cv::Vec3b {
        if (x >= 0 && x < img.rows && y >= 0 && y < img.cols) {
            return img.at<cv::Vec3b>(x, y);
        }
        return cv::Vec3b(0, 0, 0);
    };
    auto g = [&] (int x, int y) -> cv::Vec3b& {
        return res.at<cv::Vec3b>(x, y);
    };
    
    // ma trận biến đổi nghịch đảo là ma trận nghịch đảo của ma trận biến đổi
    auto inv_mat = transform_mat.inv();

    // với mỗi pixel trên ảnh kết quả
    for (int xnew = transformed_region.y; xnew < transformed_region.y + transformed_region.height; ++xnew) {
        for (int ynew = transformed_region.x; ynew < transformed_region.x + transformed_region.width; ++ynew) {
            // biến đổi ngược tọa độ để tìm tọa độ tương ứng trên ảnh cũ
            auto oldPos = inv_mat * cv::Matx21d(xnew, ynew);
            auto xold = oldPos(0, 0);
            auto yold = oldPos(1, 0);

            int x = std::round(xold);
            int y = std::round(yold);
            
            double a = xold - x;
            double b = yold - y;

            // trên mỗi kênh màu, nội suy giá trị pixel mới bằng 4 pixel cũ
            for (int c = 0; c < img.channels(); ++c) {
                g(xnew - transformed_region.y, ynew - transformed_region.x)[c] = cv::saturate_cast<uchar>(
                                              (1 - a) * (1 - b) * f(x, y)[c]
                                            + a * (1 - b) * f(x + 1, y)[c]
                                            + (1 - a) * b * f(x, y + 1)[c]
                                            + a * b * f(x + 1, y + 1)[c]);
            }
        }
    }

    return res;
}

/**
 * hàm cắt ảnh: cắt lấy 1 vùng hình chữ nhật của ảnh
 * @img: ảnh đầu vào
 * @gx: tọa độ x của tâm hình chữ nhật sẽ cắt ra
 * @gy: tọa độ y của tâm hình chữ nhật sẽ cắt ra
 * @height: chiều cao hình chữ nhật
 * @width: chiều rộng hình chữ nhật
 * @return: ảnh đã được cắt
 */
cv::Mat crop(Img img, int gx, int gy, int height, int width) {
    cv::Mat res(height, width, CV_8UC3);

    for (int i = 0; i < res.rows; ++i) {
        for (int j = 0; j < res.cols; ++j) {
            res.at<cv::Vec3b>(i, j) = img.at<cv::Vec3b>(gx - height / 2 + i, gy - width / 2 + j);
        }
    }
    return res;
}

/**
 * hàm xoay ảnh bảo toàn kích thước
 * @img: ảnh đầu vào
 * @phi: góc xoay
 * @return: ảnh đã xoay
 */
cv::Mat rotate(Img img, double phi) {
    cv::Mat res;
    
    // áp dụng ma trận phép xoay lên ảnh
    cv::Matx22d rotate_mat(std::cos(phi), std::sin(phi), -std::sin(phi), std::cos(phi)); 
    return applyTransform(img, rotate_mat);
}

/**
 * hàm xoay ảnh không bảo toàn kích thước
 * @img: ảnh đầu vào
 * @phi: góc xoay
 * @return: ảnh đã xoay
 */
cv::Mat rotateN(Img img, double phi) {
    // xoay ảnh có bảo toàn
    auto rotated = rotate(img, phi);
    // sau đó cắt phần ảnh ở giữa theo kích thước ban đầu ra
    return crop(rotated, rotated.rows / 2, rotated.cols / 2, img.rows, img.cols);
}

/**
 * hàm thay đổi kích thước ảnh
 * @img: ảnh đầu vào
 * @xscale: tỉ lệ thay đổi theo trục x
 * @yscale: tỉ lệ thay đổi theo trục y
 * @return: ảnh đã thay đổi kích thước
 */
cv::Mat scale(Img img, double xscale, double yscale) {
    // áp dụng ma trận phép co giãn lên ảnh
    cv::Matx22d scale_mat(xscale, 0, 0, yscale);
    return applyTransform(img, scale_mat);
}
