#include "ScopedTimer.hpp"
#include <iostream>

/**
 * hàm khởi tạo
 * khi một biến ScopedTimer được khởi tạo, thời gian sẽ bắt đầu được tính
 * @optname:  tên operations đang tính thời gian
 */
ScopedTimer::ScopedTimer(const std::string& optname = "<ano>"): optname(optname) {
    std::cout << optname << " started\n";
    start = std::chrono::system_clock::now();
}

/**
 * hàm hủy
 * khi biến ScopedTimer hủy, thời gian ngừng tính và xuất ra thời gian tồn tại của biến này
 */
ScopedTimer::~ScopedTimer() {
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> dur = end - start;
    std::cout << optname << " ended, time elapsed: " << dur.count() << "s\n";
}
