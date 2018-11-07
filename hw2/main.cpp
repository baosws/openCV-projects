#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "algos.h"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

#define CMD_REVERT_COLOR            "g"      // mã lệnh đổi ảnh màu thành ảnh xám
#define CMD_CHANGE_BIRGHTNESS       "b"      // mã lệnh đổi độ sáng
#define CMD_CHANGE_CONTRAST         "c"      // mã lệnh đổi độ tương phản
#define CMD_INVERT                  "n"      // mã lệnh tạo ảnh âm bản
#define CMD_LOG_TRANSFORM           "lt"     // mã lệnh biến đổi ảnh log transform
#define CMD_GAMMA_TRANSFORM         "gt"     // mã lệnh biến đổi ảnh gamma transform
#define CMD_HISTOGRAM               "hi"     // mã lệnh tính histogram
#define CMD_COMPARE_HISTOGRAM       "cmphi"  // mã lệnh so sánh histogram
#define CMD_HISTOGRAM_COLOR         "hiqc"   // mã lệnh tính histogram màu
#define CMD_HISTOGRAM_GRAY          "hiqg"   // mã lệnh tính histogram xám
#define CMD_COMPARE_HISTOGRAM_COLOR "cphiqc" // mã lệnh so sánh histogram màu
#define CMD_COMPARE_HISTOGRAM_GRAY  "cphiqg" // mã lệnh so sánh histogram xám
#define CMD_SHOW_HELP               "help"   // mã lệnh hiện hướng dẫn

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
 * hàm đổi ảnh màu thành ảnh xám từ param parser
 * @params: param parser
 */
void revert_color(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // tính ảnh xám tương ứng
    auto res = revert_color(img);
    
    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * hàm đổi độ sáng của ảnh từ param parser
 * @params: param parser
 */ 
void change_brightness(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy thông số beta (đơn vị độ sáng thay đổi): f(x) = alpha * x + beta
    auto beta = params.get<double>("beta");
    
    // tính ảnh đã thay đổi độ sáng
    auto res = change_contrast_and_brightness(img, 1, beta);

    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * hàm đổi độ tương phản của ảnh từ param parser
 * @params: param parser
 */
void change_contrast(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // đọc tham số alpha (tỉ lệ thay đổi độ tương phản): f(x) = alpha * x + beta
    auto alpha = params.get<double>("alpha");
    
    // tính ảnh đã thay đổi độ tương phản
    auto res = change_contrast_and_brightness(img, alpha, 0);

    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * hàm tạo ảnh âm bản từ param parser
 * @params: param parser
 */
void invert(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // tính ảnh âm bản
    auto res = invert(img);

    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * hàm biến đổi ảnh log transform từ params
 * @params: param parser
 */
void log_transform(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy tham số c trong biến đổi log: x -> c * log(x + 1)
    auto c = params.get<double>("c");

    // tính ảnh đã biến đổi log
    auto res = log_transform(img, c);

    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * hàm biến đổi ảnh gamma transform từ params
 * @params: param parser
 */
void gamma_transform(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy tham số gamma trong biến đổi gamma: x -> gamma ^ x
    auto gamma = params.get<double>("gamma");

    // tính ảnh đã biến đổi gamma
    auto res = gamma_transform(img, gamma);

    // xuất ảnh ra màn hình
    show_image(res);
}

/**
 * tính histogram của ảnh từ param parser
 * @params: param parser
 */
void get_histogram(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");
    
    // tính histogram
    auto his = get_histogram(img, num_bins);

    // xuất ảnh ra màn hình
    show_image(his);
}

/**
 * so sánh histogram của 2 ảnh từ param parser
 * @params: param parser
 */
void compare_historam(Params params) {
    // đọc ảnh thứ nhất từ @params
    auto img1 = read_img(params);

    // đọc ảnh thứ 2 từ @params
    auto img2 = read_img2(params);

    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");

    // lấy phương thức so sánh histogram 2 ảnh
    auto cmp_mode = params.get<std::string>("cmp_mode");

    // tính khoảng cách giữa histogram 2 ảnh
    auto res = compare_histogram(img1, img2, num_bins, HistogramComparator(cmp_mode));

    // xuất ảnh thứ nhất ra màn hình
    show_image(img1, "first image");

    // xuất ảnh thứ 2 ra màn hình
    show_image(img2, "result 2");

    // xuất khoảng cách histogram ra màn hình
    std::cout << "Histogram distance using " + cmp_mode + " method: " << res << std::endl;
}

/**
 * tính histogram màu của ảnh lấy từ param parser
 * @params: param parser
 */
void get_color_histogram(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");

    // lấy ảnh histogram
    auto res = get_color_histogram(img, num_bins);

    // xuất ảnh histogram ra màn hình
    show_image(res);
}

/**
 * tính histogram xám của ảnh lấy từ param parser
 * @params: param parser
 */
void get_gray_histogram(Params params) {
    // đọc ảnh từ @params
    auto img = read_img(params);

    // xuất ảnh đầu vào
    show_image(img, "input");

    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");

    // lấy ảnh histogram
    auto res = get_gray_histogram(img, num_bins);

    // xuất ảnh histogram ra màn hình
    show_image(res);
}

/**
 * so sánh histogram màu của 2 ảnh lấy từ param parser
 * @params: param parser
 */
void compare_color_histogram(Params params) {
    // đọc ảnh thứ nhất từ @params
    auto img1 = read_img(params);

    // đọc ảnh thứ 2 từ @params
    auto img2 = read_img2(params);

    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");

    // lấy tham số phương thức so sánh histogram
    auto cmp_mode = params.get<std::string>("cmp_mode");

    // tính khoảng cách 2 histogram
    auto res = compare_color_histogram(img1, img2, num_bins, HistogramComparator(cmp_mode));

    // xuất khoảng cách 2 histogram ra màn hình
    std::cout << "Histogram distance using " + cmp_mode + " method: " << res << std::endl;

    // xuất ảnh thứ nhất ra màn hình
    show_image(img1, "first image");

    // xuất ảnh thứ 2 ra màn hình
    show_image(img2, "second image");
}

/**
 * so sánh histogram xám của 2 ảnh lấy từ param parser
 * @params: param parser
 */
void compare_gray_histogram(Params params) {
    // đọc ảnh thứ nhất từ @params
    auto img1 = read_img(params);

    // đọc ảnh thứ 2 từ @params
    auto img2 = read_img2(params);
    
    // lấy tham số số bin
    auto num_bins = params.get<int>("bin");

    // lấy phương thức so sánh 2 histogram
    auto cmp_mode = params.get<std::string>("cmp_mode");

    // chuyển 2 ảnh sang ảnh xám rồi tính khoảng cách 2 histogram
    auto res = compare_gray_histogram(img1, img2, num_bins, HistogramComparator(cmp_mode));

    // xuất khoảng cách 2 histogram ra màn hình
    std::cout << "Histogram distance using " + cmp_mode + " method: " << res << std::endl;

    // xuất ảnh thứ nhất ra màn hình
    show_image(img1, "first image");

    // xuất ảnh thứ 2 ra màn hình
    show_image(img2, "second image");
}

typedef void (*cmd_func)(const cv::CommandLineParser&);

// bảng ánh xạ từ chuỗi mã lệnh tới hàm xử lý tương ứng dựa vào param parser
std::map<std::string, cmd_func> commands =
{
    {CMD_REVERT_COLOR, revert_color},                       // lệnh đổi ảnh màu thành xám
    {CMD_CHANGE_BIRGHTNESS, change_brightness},             // lệnh đổi độ sáng
    {CMD_CHANGE_CONTRAST, change_contrast},                 // lệnh đổi độ tương phản
    {CMD_INVERT, invert},                                   // lệnh tạo ảnh âm bản
    {CMD_LOG_TRANSFORM, log_transform},                     // lệnh biến đổi ảnh log transform
    {CMD_GAMMA_TRANSFORM, gamma_transform},                 // lệnh biến đổi ảnh gamma transform
    {CMD_HISTOGRAM, get_histogram},                         // lệnh tính histogram
    {CMD_COMPARE_HISTOGRAM, compare_historam},              // lệnh so sánh 2 histogram
    {CMD_HISTOGRAM_COLOR, get_color_histogram},             // lệnh tính histogram màu
    {CMD_HISTOGRAM_GRAY, get_gray_histogram},               // lệnh tính histogram xám
    {CMD_COMPARE_HISTOGRAM_COLOR, compare_color_histogram}, // lệnh so sánh 2 histogram màu
    {CMD_COMPARE_HISTOGRAM_GRAY, compare_gray_histogram},    // lệnh so sánh 2 histogram xám
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
        "{@path                          |          | path to main image file}"
        "{path2                          |          | path to second image file - used for histogram comparasions}"
        "{" CMD_REVERT_COLOR            "|          | convert color image to greyscale image}"
        "{" CMD_CHANGE_BIRGHTNESS       "|          | change image brightness}"
        "{" CMD_CHANGE_CONTRAST         "|          | change image contrast}"
        "{" CMD_INVERT                  "|          | create invert image}"
        "{" CMD_LOG_TRANSFORM           "|          | image log transform}"
        "{" CMD_GAMMA_TRANSFORM         "|          | image gamma transform}"
        "{" CMD_HISTOGRAM               "|          | get image histogram}"
        "{" CMD_COMPARE_HISTOGRAM       "|          | compare histogram of 2 images}"
        "{" CMD_HISTOGRAM_COLOR         "|          | get image color histogram}"
        "{" CMD_HISTOGRAM_GRAY          "|          | get image gray histogram}"
        "{" CMD_COMPARE_HISTOGRAM_COLOR "|          | compare color histogram of 2 images}"
        "{" CMD_COMPARE_HISTOGRAM_GRAY  "|          | compare gray histogram of 2 images}"
        "{" CMD_SHOW_HELP               "|          | show help}"
        "{alpha                          |1         | alpha value for contrast changing}"
        "{beta                           |0         | beta value for brightness changing}"
        "{bin                            |16        | number of bin for a histogram computation}"
        "{c                              |1         | c value for log transformation}"
        "{gamma                          |1         | gamma value for gamma transformation}"
        "{cmp_mode                       |corelation| compare method for histograms of 2 images (value = corelation / intersect / chisq)}"
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
