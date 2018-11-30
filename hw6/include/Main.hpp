#include <string>                      // cần kiểu std::string
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

namespace Main {
    typedef const cv::CommandLineParser& Params;

    auto read_img(Params);

    void show_image(const cv::Mat&, const std::string&);

    std::string get_result_info(Params);
    void show_help(Params);
    void cmd_mg(Params);
    void cmd_mc(Params);
    void cmd_meg(Params);
    void cmd_mec(Params);
    void cmd_gg(Params);
    void cmd_gc(Params);
    
    typedef void (*cmd_func)(const cv::CommandLineParser&);

    auto parseParams(int, char**);
    cmd_func getCommand(const cv::CommandLineParser&);    
    void main(int, char**); 
};
