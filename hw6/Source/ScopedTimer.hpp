#pragma once
#include <string>
#include <chrono>

/**
 * lớp dùng để tính thời gian thực thi của một block lệnh
 * khi khởi tạo, thời gian được tính, khi bị hủy, thời gian ngừng tính
 */
class ScopedTimer {
    std::string optname;
    std::chrono::time_point<std::chrono::system_clock> start, end;
public:
    ScopedTimer(const std::string&);
    ~ScopedTimer();
};
