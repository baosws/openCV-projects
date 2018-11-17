#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "algos.h"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

typedef const cv::CommandLineParser& Params;

/**
 * hàm đọc ảnh từ param parser
 * @params: param parser
 * @return: ảnh đọc được từ thông tin của @params
 */
auto read_img(Params params) {
    // lấy đường dẫn file từ @params
    auto fname = params.get<std::string>("@path");

    // đọc ảnh từ đường dẫn đó
    return cv::imread(fname);
}

/**
 * hàm xuất ảnh ra màn hình
 * @img: ảnh cần xuất
 * @win_name: tiêu đề cửa sổ xuất hình
 */
void show_image(const cv::Mat& img, const std::string& win_name = "result") {
    cv::imshow(win_name, img);
    cv::imwrite(win_name + ".png", img);
}

/** hàm hiện hướng dẫn sử dụng
 * @params: param parser
 */
void show_help(Params params) {
    params.printMessage();
}

void cmd_mg(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");
    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = mean_gray(img, kern);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

void cmd_mc(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");
    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = mean_color(img, kern);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

void cmd_meg(Params param) {
    // // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");
    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = median_gray(img, kern);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

void cmd_mec(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");
    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = median_color(img, kern);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

void cmd_gg(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");

    // lấy thông số độ lệch chuẩn
    double sd = param.get<double>("sd");
    
    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = gaussian_gray(img, kern, sd);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

void cmd_gc(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy thông số kernel
    int kern = param.get<int>("kern");

    // lấy thông số độ lệch chuẩn
    double sd = param.get<double>("sd");

    if (kern <= 0 || kern % 2 == 0) {
        throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
    }

    // tính ảnh kết quả
    auto res = gaussian_color(img, kern, sd);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // xuất ảnh kết quả
    show_image(res, "result");
}

typedef void (*cmd_func)(const cv::CommandLineParser&);

// bảng ánh xạ từ chuỗi mã lệnh tới hàm xử lý tương ứng dựa vào param parser
std::map<std::string, cmd_func> commands =
{
    {"mg", cmd_mg},
    {"mc", cmd_mc},
    {"meg", cmd_meg},
    {"mec", cmd_mec},
    {"gg", cmd_gg},
    {"gc", cmd_gc}
};

/**
 * hàm chuyển tham số dòng lệnh vào param parser
 * @nargs: số lượng tham số
 * @args: mảng các tham số
 * @return: param parser
 */
auto parseParams(int nargs, char* args[]) {
    // bảng định nghĩa các tham số mà chương trình có thể/cần xử lý
    const cv::String keys =
        "{@path || path to main image file}"
        "{mg    || mean filter on grayscale image}"
        "{mc    || mean filter on color image}"
        "{meg   || median filter on grayscale image}"
        "{mec   || median filter on color image}"
        "{gg    || Gaussian filter on grayscale image}"
        "{gc    || Gaussian filter on color image}"
        "{kern  |3| kernel size (must be an odd natural number}"
        "{sd    | | standard deviation of the Gaussian distribution in Gaussian filter}"
        "{help  || show help}"
    ;

    // tạo param parser dựa trên tham số đầu vào và bảng định nghĩa các hàm chức năng
    cv::CommandLineParser parser(nargs, args, keys);
    return parser;
}

/**
 * hàm lấy lệnh cần thực hiện từ param parser
 * @params: param parser
 * @return: con trỏ tới hàm cần thực hiện
 */
cmd_func getCommand(const cv::CommandLineParser& parser) {
    // với mỗi lệnh trong bảng lệnh
    for (auto& cmd : commands) {
        // nếu mã lệnh đó xuất hiện trong param parser thì trả về hàm tương ứng
        if (parser.has(cmd.first)) {
            return cmd.second;
        }
    }

    // nếu không tìm thấy lệnh đã khai báo trước thì báo lỗi
    throw std::runtime_error("Khong tim thay lenh hop le");
}

// hàm main
// @nargs: số lượng tham số truyền vào khi chương trình được gọi
// @args: mảng các tham số
int main(int nargs, char* args[]) try {
    // tạo param parser từ tham số dòng lệnh đầu vào
    auto params = parseParams(nargs, args);

    //  trích lệnh từ trong tham số ra và thực hiện nó
    getCommand(params)(params);

    // đợi cho người dùng bấm phím bất kì rồi thoát chương trình
    cv::waitKey();
}
catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
}
