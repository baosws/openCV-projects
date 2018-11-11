#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
// #include "algos.h"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

#define CMD_ROTATE    "rotate"
#define CMD_ROTATE_N  "rotateN"
#define CMD_SCALE     "scale"
#define CMD_SHOW_HELP "help"    // mã lệnh hiện hướng dẫn

typedef const cv::CommandLineParser& Params;
// hàm main
// @nargs: số lượng tham số truyền vào khi chương trình được gọi
// @args: mảng các tham số
int main(int nargs, char* args[]) try {
    cv::Point a(1, 2);
    cv::Point b(2, 3);
    tell(a.x);
    tell(a<b);
}
catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
}
