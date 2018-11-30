#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "EdgeDetect.hpp"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include "ScopedTimer.hpp"
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

namespace Main {
    typedef const cv::CommandLineParser& Params;

    std::string get_result_info(Params);
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
//         cv::imshow(win_name, img);
        cv::imwrite(win_name + ".png", img);
    }

        /** hàm hiện hướng dẫn sử dụng
     * @params: param parser
     */
    void show_help(Params params) {
        params.printMessage();
    }

    void cmd_gra_sobel(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto res = EdgeDetect::cmd_gra_sobel(img);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }

    void cmd_gra_prewitt(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto res = EdgeDetect::cmd_gra_prewitt(img);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }

    void cmd_gra_scharr(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto res = EdgeDetect::cmd_gra_scharr(img);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }

    void cmd_gra_roberts(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto res = EdgeDetect::cmd_gra_roberts(img);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }

    void cmd_laplacian(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto res = EdgeDetect::cmd_laplacian(img);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }

    void cmd_log(Params param) {
        auto img = read_img(param);
        cvtColor(img, img, ::CV_BGR2GRAY);

        auto kern = param.get<double>("kern");
        auto sd = param.get<double>("sd");
        if (sd == 0) {
            sd = 0.3 * ((kern - 1) * 0.5 - 1) + 0.8;
        }
        auto res = EdgeDetect::cmd_log(img, kern, sd);

        show_image(img, "input");

        show_image(res, "result_" + get_result_info(param));
    }
    
    typedef void (*cmd_func)(const cv::CommandLineParser&);

    // bảng ánh xạ từ chuỗi mã lệnh tới hàm xử lý tương ứng dựa vào param parser
    std::map<std::string, cmd_func> commands =
    {
        {"gra-sobel", cmd_gra_sobel},
        {"gra-prewitt", cmd_gra_prewitt},
        {"gra-scharr", cmd_gra_scharr},
        {"gra-roberts", cmd_gra_roberts},
        {"laplacian", cmd_laplacian},
        {"log", cmd_log},
    };
    
    std::string get_result_info(Params param) {
        std::string res = param.get<std::string>("@path") + "_cmd-" + std::find_if(commands.begin(), commands.end(), [&] (const std::pair<std::string, cmd_func>& cmd) {
                    return param.has(cmd.first);
                    })->first;
        std::replace(res.begin(), res.end(), '.', '_');
        dtell(res);
        return res;
//             + "_kern-" + std::to_string(param.get<int>("kern"))
//             + "_sd-" + std::to_string(param.get<double>("sd"));
    }

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
            "{gra-sobel    || gradient with Sobel mask}"
            "{gra-prewitt    || gradient with Prewitt mask}"
            "{gra-scharr   || gradient with Scharr mask}"
            "{gra-roberts   || gradient with Roberts mask}"
            "{laplacian    || Laplacian mask}"
            "{log    || Log mask}"
            "{canny  || Edge detection with Canny}"
            "{kern |5| kernel size for LoG filter}"
            "{sd |0.8| Standard deviation for LoG filter}"
            "{thresh |0| Threshold for Canny edge detect}"
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
    void main(int nargs, char* args[]) try {
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
};

int main(int nargs, char* args[]) {
    Main::main(nargs, args);
}
