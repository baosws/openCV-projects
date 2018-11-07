#include <bits/stdc++.h>
#include "opencv2/core.hpp"
#include "helper.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

const char* CMD_RV_COLOR = "rv"; // mã lệnh đổi màu ảnh
const char* CMD_CHG_CONTRAST = "cc"; // mã lệnh đổi độ tương phản
const char* CMD_CHG_BRIGHTNESS = "cb"; // mã lệnh đổi độ sáng

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
bool is_grayscale(const cv::Mat& img) {
    // duyệt từng pixel trong ảnh
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

cv::Vec3d get_random_color(const cv::Vec3d& cur, int diff) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    while (1) {
        int eps = 200;
        std::uniform_real_distribution<double> dis_r(std::max(0.0, cur[2] - eps), std::min(255.0, cur[2] + eps));
        std::uniform_real_distribution<double> dis_g(std::max(0.0, cur[1] - eps), std::min(255.0, cur[1] + eps));
        double r = dis_r(gen), g = dis_g(gen);
        double dr = r - cur[2], dg = g - cur[1];
        double db = (diff - dr * coeff[2] - dg * coeff[1]) / coeff[0];
        if (0 <= cur[0] + db && cur[0] + db <= 255) {
            tell(r, g, cur[0] + db);
            return cv::Vec3d(cur[0] + db, g, r);
        }
    }
}

cv::Vec3d get_next_color(const cv::Vec3d& cur, int diff) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dis(1, 255);
    std::uniform_int_distribution<int> dis_sign(0, 1);
    cv::Vec3d res;
    for (int step = 0; step < 10; ++step) {
        double alpha = dis(gen), beta = dis(gen), gamma = dis(gen);
        double tmp = coeff[0] * coeff[0] / alpha / alpha + coeff[1] * coeff[1] / beta / beta + coeff[2] * coeff[2] / gamma / gamma;
        double dr = coeff[2] * diff / tmp / gamma / gamma;
        double dg = coeff[1] * diff / tmp / beta / beta;
        double db = coeff[0] * diff / tmp / alpha / alpha;
        res = cv::Vec3d(cur[0] + db, cur[1] + dg, cur[2] + dr);
        tell(res[0], res[1], res[2]);
        if (res[0] >= 0 && res[0] <= 255 && res[1] >= 0 && res[1] <= 255 && res[2] >= 0 && res[2] <= 255) {
            break;
        }
    }
    return res;
}

cv::Mat cvt2Color(const cv::Mat& src) {
    std::map<int, cv::Vec3d> levels;
    levels[0] = cv::Vec3d(0, 0, 0);
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            levels[src.at<cv::Vec3b>(i, j)[0]];
        }
    }
    
    for (auto it = std::next(levels.begin()); it != levels.end(); ++it) {
        auto diff = it->first - std::prev(it)->first;
        it->second = get_next_color(std::prev(it)->second, diff);
    }

    for (auto& x : levels) {
        int y = x.first;
        double r = x.second[2], g = x.second[1], b = x.second[0];
        tell(y, r, g, b);
    }

    auto res = src.clone();
    for (int i = 0; i < src.rows; ++i) {
        for (int j = 0; j < src.cols; ++j) {
            for (int c = 0; c < src.channels(); ++c) {
                res.at<cv::Vec3b>(i, j)[c] = cv::saturate_cast<uchar>(levels[src.at<cv::Vec3b>(i, j)[0]][c]);
            }
        }
    }
    
    return res;
}

/**
 * hàm đổi màu ảnh xám thành ảnh màu và ngược lại
 * @img: ảnh cần đổi màu
 * return: nếu @img là ảnh xám thì trả về một ảnh màu tương ứng, ngược lại trả về ảnh xám tương ứng
 **/
cv::Mat revert_color(const cv::Mat& img) {
    if (is_grayscale(img)) { // nếu @img là ảnh xám
        return cvt2Color(img);
    }
    else { // nếu @img là ảnh màu
        cv::Mat res;
        cvtColor(img, res, ::CV_RGB2GRAY);
        return res;
    }
}

/**
 * hàm thay đổi độ tương phản và độ sáng của ảnh
 * @img: ảnh đầu vào
 * @alpha: tỷ lệ thay đổi độ tương phản
 * @beta: độ tăng độ sáng
 * return: một ảnh mới đã có độ tương phản tăng lên @alpha lần và độ sáng tăng @beta đơn vị so với ảnh đầu vào
 **/
cv::Mat change_contrast_and_brightness(const cv::Mat& img, double alpha, double beta) {
    // tạo ảnh kết quả bằng cách copy từ ảnh ban đầu
    auto res = img.clone();

    // duyệt mỗi pixel (i, j) của ảnh
    for (int i = 0; i < res.rows; ++i) {
        for (int j = 0; j < res.cols; ++j) {

            // duyệt mỗi kênh màu của ảnh
            for (int c = 0; c < res.channels(); ++c) {
                // thay đổi độ tương phản và độ sáng bằng công thức:
                // g(i, j) = alpha * f(i, j) + beta
                // với g(i, j) là giá trị pixel (i, j) mới, f(i, j) là giá trị pixel (i, j) cũ
                res.at<cv::Vec3b>(i, j)[c] = // đây là g(i, j) ở kênh màu thứ c
                    cv::saturate_cast<uchar>( // phải dùng saturate_cast để đưa g(i, j) về lại khoảng [0, 255]
                        alpha * img.at<cv::Vec3b>(i, j)[c] + beta // img.at<cv::Vec3b>(i, j)[c] là f(i, j) ở kênh màu c
                        );
            }
        }
    }
    return res;
}

int main(int nargs, char* args[]) try {

    // kiểm tra đúng số lượng tham số
    // chương trình chỉ xử lý 3 hoặc 4 tham số
    if (nargs < 3 || nargs > 4) {
        throw std::runtime_error("Sai so luong tham so: " + std::to_string(nargs) + " (can 3 hoac 4)");
    }
    else {
        auto fname = std::string(args[1]); // tham số thứ 1 là đường dẫn file (tham số 0 là tên chương trình)
        auto cmd = std::string(args[2]);   // tham số thứ 2 là mã lệnh
        
        // kiểm tra số tham số tương ứng với từng lệnh
        if ((nargs == 3 && cmd != CMD_RV_COLOR) // nếu có 3 tham số thì lệnh phải là đảo màu
         || (nargs == 4 && cmd != CMD_CHG_CONTRAST && cmd != CMD_CHG_BRIGHTNESS)) { // nếu có 4 tham số thì lệnh phải là đổi độ sáng hoặc đổi độ tương phản
            throw std::runtime_error("Khong nhan ra lenh " + cmd + " voi so luong tham so = " + std::to_string(nargs));
        }
        
        // đọc ảnh từ file
        cv::Mat img = cv::imread(fname, cv::IMREAD_COLOR);

        // kiểm tra có đọc được ảnh không
        if (!img.data) {
            throw std::runtime_error("Khong mo duoc file " + fname);
        }
        
        cv::Mat result;
        
        // dựa vào các tham số để xử lý
        if (cmd == CMD_RV_COLOR) { // nếu là lệnh đổi màu thì gọi hàm đổi màu
            result = revert_color(img);
        }
        else { // nếu là lệnh đổi độ sáng hoặc đổi độ tương phản
            auto param = std::stod(args[3]); // tham số lệnh - đây là alpha nếu lệnh là đổi độ tương phản, và beta nếu lệnh là đổi độ sáng
            if (cmd == CMD_CHG_CONTRAST) {
                result = change_contrast_and_brightness(img, param, 0); // nếu là lệnh đổi độ tương phản thì alpha = param, beta = 0, tức là chỉ thay đổi độ tương phản, không thay đổi độ sáng
            }
            else {
                result = change_contrast_and_brightness(img, 1, param); // nếu là lệnh đổi độ sáng thì alpha = 1, beta = param, tức là giữ nguyên độ tơng phản, chỉ thay đổi độ sáng
            }
        }

        // xuất ảnh kết quả ra cửa sổ có tiêu đề là "Result image"
        cv::imshow("Result image", result);
        // đồng thời lưu ảnh lại với tên là '[result ' + tên cũ
        cv::imwrite("[result] " + fname, result);
        // đợi cho người dùng ấn phím bất kì thì đóng cửa sổ xuất hình và thoát chương trình
        cv::waitKey();
    }
}
catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
}
