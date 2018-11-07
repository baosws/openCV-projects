#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

const char* CMD_RV_COLOR = "--rv";       // mã lệnh đổi màu ảnh
const char* CMD_CHG_CONTRAST = "--cc";   // mã lệnh đổi độ tương phản
const char* CMD_CHG_BRIGHTNESS = "--cb"; // mã lệnh đổi độ sáng

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

// hàm chuyển ảnh thành ảnh đỏ
// @img: ảnh cần chuyển 
// return: ảnh màu đỏ tương ứng
cv::Mat convert_to_red(const cv::Mat& img) {
    // tạo ảnh kết quả bằng cách copy từ ảnh ban đầu
    auto result = img.clone();

    // duyệt mỗi pixel (i, j) trong ảnh
    for (int i = 0; i < img.rows; ++i) {
        for (int j = 0; j < img.cols; ++j) {
            
            // duyệt mỗi kênh màu c trong ảnh
            // c = 0: blue
            // c = 1: green
            // c = 2: red
            for (int c = 0; c < img.channels(); ++c) {
                // nếu đang ở kênh màu khác đỏ
                if (c != 2) {
                    // thì chuyển nó về 0
                    result.at<cv::Vec3b>(i, j)[c] = 0;
                }
            }
        }
    }
    
    return result;
}

/**
 * hàm đổi màu ảnh xám thành ảnh màu (đỏ) và ngược lại
 * @img: ảnh cần đổi màu
 * return: nếu @img là ảnh xám thì trả về một ảnh màu đỏ tương ứng, ngược lại trả về ảnh xám tương ứng
 **/
cv::Mat revert_color(const cv::Mat& img) {
    if (is_grayscale(img)) { // nếu @img là ảnh xám
        // thì chuyển nó thành ảnh đỏ
        return convert_to_red(img);
    }
    else { // nếu @img là ảnh màu
        // biến sẽ lưu ảnh kết quả
        cv::Mat result;
        
        // chuyển @img thành ảnh xám và lưu vào result
        cvtColor(img, result, ::CV_RGB2GRAY);
        
        return result;
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

// hàm main
// @nargs: số lượng tham số truyền vào khi chương trình được gọi
// @args: mảng các tham số
int main(int nargs, char* args[]) try {

    // kiểm tra số lượng tham số
    // chương trình chỉ xử lý 3 hoặc 4 tham số
    if (nargs < 3 || nargs > 4) {
        throw std::runtime_error("Sai so luong tham so: " + std::to_string(nargs) + " (can 3 hoac 4)");
    }
    else {
        // tham số thứ 1 là đường dẫn file (tham số 0 là tên chương trình)
        auto fname = std::string(args[1]);
        // tham số thứ 2 là mã lệnh
        auto cmd = std::string(args[2]);
        
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
        
        // ảnh sẽ lưu kết quả
        cv::Mat result;
        
        // dựa vào các tham số để xử lý
        if (cmd == CMD_RV_COLOR) { // nếu là lệnh đổi màu thì gọi hàm đổi màu
            result = revert_color(img);
        }
        else { // nếu là lệnh đổi độ sáng hoặc đổi độ tương phản
            // tham số thứ 3 là tham số của lệnh - đây là alpha nếu lệnh là đổi độ tương phản, và beta nếu lệnh là đổi độ sáng
            double param = std::stod(args[3]);
            
            if (cmd == CMD_CHG_CONTRAST) { // nếu là lệnh đổi độ tương phản
                // thì alpha = param, beta = 0, tức là chỉ thay đổi độ tương phản, không thay đổi độ sáng
                result = change_contrast_and_brightness(img, param, 0); 
            }
            else { // nếu là lệnh đổi độ sáng
                // thì alpha = 1, beta = param, tức là giữ nguyên độ tơng phản, chỉ thay đổi độ sáng
                result = change_contrast_and_brightness(img, 1, param);
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
    std::cerr << ex.what() << std::endl;
}
