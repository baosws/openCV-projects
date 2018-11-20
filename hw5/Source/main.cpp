#include <string>                      // cần kiểu std::string
#include <iostream>                    // cần std::cerr, std::endl
#include "Filters.hpp"                     // định nghĩa các hàm chức năng xử lý trên ảnh
#include "ScopedTimer.hpp"
#include "Main.hpp"
#include <map>
#include "opencv2/highgui/highgui.hpp" // cần các hàm cv::imread, cv::imwrite, cv::imshow, cv::waitKey, cv::namedWindow
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor
#ifdef BDBG
#include "/home/duongbao/code-hub/logger.hpp"
#endif

namespace Main {
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
     * hàm xử lý lọc trung bình ảnh xám từ param parser
     * @param: param parser
     */
    void cmd_mg(Params param) {
        ScopedTimer timer(__func__);
        // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::mean_gray(img, kern);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
    }
    
    /**
     * hàm xử lý lọc trung bình ảnh màu từ param parser
     * @param: param parser
     */
    void cmd_mc(Params param) {
        ScopedTimer timer(__func__);
        // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::mean_color(img, kern);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
    }
    
    /**
     * hàm xử lý lọc trung vị ảnh xám từ param parser
     * @param: param parser
     */
    void cmd_meg(Params param) {
        ScopedTimer timer(__func__);
        // // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::median_gray(img, kern);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
    }

    /**
     * hàm xử lý lọc trung vị ảnh màu từ param parser
     * @param: param parser
     */
    void cmd_mec(Params param) {
        ScopedTimer timer(__func__);
        // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::median_color(img, kern);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
    }

    /**
     * hàm xử lý lọc Gaussian ảnh xám từ param parser
     * @param: param parser
     */
    void cmd_gg(Params param) {
        ScopedTimer timer(__func__);
        // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");

        // lấy thông số độ lệch chuẩn
        double sd = param.get<double>("sd");
        if (sd == 0) {
            sd = 0.3 * ((kern - 1) * 0.5 - 1) + 0.8;
        }
        
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::gaussian_gray(img, kern, sd);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
    }

    /**
     * hàm xử lý lọc Gaussian ảnh màu từ param parser
     * @param: param parser
     */
    void cmd_gc(Params param) {
        ScopedTimer timer(__func__);
        // lấy ảnh từ param parser
        auto img = read_img(param);

        // lấy thông số kernel
        int kern = param.get<int>("kern");

        // lấy thông số độ lệch chuẩn
        double sd = param.get<double>("sd");
        if (sd == 0) {
            sd = 0.3 * ((kern - 1) * 0.5 - 1) + 0.8;
        }
        
        if (kern <= 0 || kern % 2 == 0) {
            throw std::invalid_argument("Kernel size expected to be an odd natural number, recieved: " + std::to_string(kern));
        }

        // tính ảnh kết quả
        auto res = Filters::gaussian_color(img, kern, sd);

        // xuất ảnh đầu vào
        show_image(img, "input");

        // xuất ảnh kết quả
        show_image(res, "result_" + get_result_info(param));
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
    
    std::string get_result_info(Params param) {
        return "cmd-" + std::find_if(commands.begin(), commands.end(), [&] (const std::pair<std::string, cmd_func>& cmd) {
                    return param.has(cmd.first);
                    })->first
            + "_kern-" + std::to_string(param.get<int>("kern"))
            + "_sd-" + std::to_string(param.get<double>("sd"));
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
            "{mg    || mean filter on grayscale image}"
            "{mc    || mean filter on color image}"
            "{meg   || median filter on grayscale image}"
            "{mec   || median filter on color image}"
            "{gg    || Gaussian filter on grayscale image}"
            "{gc    || Gaussian filter on color image}"
            "{kern  |3| kernel size (must be an odd natural number}"
            "{sd    |0| standard deviation of the Gaussian distribution in Gaussian filter}"
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
