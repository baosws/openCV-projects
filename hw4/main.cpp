#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "algos.h"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

#define CMD_HQGRAY    "hqgray" // mã lệnh cân bằng histogram ảnh xám
#define CMD_HQRGB     "hqrgb"  // mã lệnh cân bằng histogram ảnh màu
#define CMD_HQHSV     "hqhsv"  // mã lệnh cân bằng histogram kênh H của ảnh HSV
#define CMD_SHOW_HELP "help"   // mã lệnh hiện hướng dẫn

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
 * hàm đọc ảnh thứ 2 (cho các thao tác cần 2 ảnh) từ param parser
 * @params: param parser
 * @return: ảnh đọc được từ thông tin của @params
 */
auto read_img2(Params params) {
    // lấy đường dẫn file từ @params
    auto fname = params.get<std::string>("path2");

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
}

/** hàm hiện hướng dẫn sử dụng
 * @params: param parser
 */
void show_help(Params params) {
    params.printMessage();
}

/**
 * hàm cân bằng histogram ảnh xám lấy từ param parser
 * @param: param parser
 */
void hqgray(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // cân bằng histogram và lưu vào ảnh res
    auto res = hqgray(img);
    auto res_opencv = hqgray_opencv(img);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");

    // xuất ảnh kết quả sử dụng hàm của opencv
    show_image(res_opencv, "opencv's result");
}

/**
 * hàm cân bằng histogram ảnh rgb lấy từ param parser
 * @param: param parser
 */
void hqrgb(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // cân bằng histogram và lưu lại vào ảnh res
    auto res = hqrgb(img);

    // cân bằng histogram bằng opencv
    auto res_opencv = hqrgb_opencv(img);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");

    // xuất ảnh kết quả sử dụng hàm của opencv
    show_image(res_opencv, "opencv's result");
}

/**
 * hàm cân bằng kênh H của ảnh HSV lấy từ param parser
 * @param: param parser
 */
void hqhsv(Params param) {
    // lấy ảnh từ param parser
    auto img = read_img(param);

    // cân bằng kênh H rồi lưu lại vào ảnh res
    auto res = hqhsv(img);
    auto res_opencv = hqhsv_opencv(img);

    // xuất ảnh đầu vào ra màn hình
    show_image(img, "input");
    // xuất ảnh kết quả ra màn hình
    show_image(res, "student's result");

    // xuất ảnh kết quả sử dụng hàm của opencv
    show_image(res_opencv, "opencv's result");
}

typedef void (*cmd_func)(const cv::CommandLineParser&);

// bảng ánh xạ từ chuỗi mã lệnh tới hàm xử lý tương ứng dựa vào param parser
std::map<std::string, cmd_func> commands =
{
    {CMD_HQGRAY, hqgray},      // lệnh cân bằng histogram ảnh xám
    {CMD_HQRGB, hqrgb},        // lệnh cân bằng histogram ảnh màu trên cả 3 kênh R, G, B
    {CMD_HQHSV, hqhsv},        // chuyển đổi ảnh màu từ hệ RBG sang hệ HSV, cân bằng trên kênh H, sau đó chuyển đổi ảnh ngược về hệ RGB để hiển thị
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
        "{" CMD_HQGRAY                  "|   | equalize greyscale histogram}"
        "{" CMD_HQRGB                   "|   | equalize RGB histogram}"
        "{" CMD_HQHSV                   "|   | equalize HSV histogram}"
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
