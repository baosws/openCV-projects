#include "stdafx.h"
#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "algos.h"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

#define CMD_ROTATE    "rotate"  // mã lệnh xoay ảnh bảo toàn
#define CMD_ROTATE_N  "rotateN" // mã lệnh xoay ảnh không bảo toàn
#define CMD_SCALE     "scale"   // mã lệnh thay đổi kích thước ảnh
#define CMD_SHOW_HELP "help"    // mã lệnh hiện hướng dẫn

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

/**
 * hàm xoay ảnh bảo toàn từ param parser
 * @param: param parser
 */
void rotate(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy tham số góc xoay phi, chuyển từ đơn vị độ sang radian
    auto phi = param.get<double>("@arg1") * PI / 180;

    // tính ảnh kết quả
    auto res = rotate(img, phi);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");
}

/**
 * hàm xoay ảnh không bảo toàn lấy từ param parser
 * @param: param parser
 */
void rotateN(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy tham số góc xoay phi, chuyển từ đơn vị độ sang radian
    auto phi = param.get<double>("@arg1") * PI / 180;

    // tính ảnh kết quả
    auto res = rotateN(img, phi);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");
}

/**
 * hàm thay đổi kích thước ảnh lấy từ param parser
 * @param: param parser
 */
void scale(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // lấy tham số tỉ lệ thay đổi theo trục x và y
    auto xscale = param.get<double>("@arg1");
    auto yscale = param.get<double>("@arg2");

    // tính ảnh kết quả
    auto res = scale(img, xscale, yscale);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");
}
typedef void (*cmd_func)(const cv::CommandLineParser&);

// bảng ánh xạ từ chuỗi mã lệnh tới hàm xử lý tương ứng dựa vào param parser
std::map<std::string, cmd_func> commands =
{
    {CMD_ROTATE, rotate},
    {CMD_ROTATE_N, rotateN},
    {CMD_SCALE, scale},
    {CMD_SHOW_HELP, show_help}
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
        "{@path                          |   | path to main image file}"
        "{" CMD_ROTATE                  "|   | rotate image with preservation}"
        "{" CMD_ROTATE_N                "|   | rotate image with out preservation}"
        "{" CMD_SCALE                   "|   | scale image}"
        "{@arg1                          |   | 'phi' for rotate command, or 'xscale' for scale command}"
        "{@arg2                          |0  | 'yscale' for scale command}"
        "{" CMD_SHOW_HELP               "|   | show help}"
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
