#pragma once
#include <vector>
#include "Histogram.h"
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

// lớp hỗ trợ vẽ histogram lên ảnh
class HistogramDrawer {
    // danh sách các histogram sẽ vẽ lên ảnh
    std::vector<Histogram> hists;
public:
    
    // chèn môt histogram mới vào
    HistogramDrawer& insert(Histogram hist) {
        hists.emplace_back(hist);
        return *this;
    }

    /**
     * phương thức vẽ các histogram lên ảnh
     * @res: ảnh được dùng để vẽ các histogram lên
     * @return: chính mình
     */
    HistogramDrawer& draw(cv::Mat& res) {
        // tính xác suất xuất hiện lớn nhất trong tất cả các histogram
        // bin màu có xác suất lớn nhất sẽ được vẽ với chiều cao là chiều cao của ảnh
        double max_height = 0;
        for (auto& hist : hists) {
            for (auto& fr : hist.getHist()) {
                max_height = std::max(max_height, fr);
            }
        }

        // lấy số bin
        int num_bins = hists[0].getBins();

        // tính khoảng cách rộng giữa điểm histogram của 2 bin kề nhau
        // bin đầu tiên sẽ được vẽ trên biên trái của ảnh, bin cuối vẽ ở biên phải ảnh
        // vậy nếu có @num_bins bin thì cần chia ảnh thành @num_bins - 1 cột
        auto bin_width = res.cols / (num_bins - 1.0);

        // vẽ từng histogram trong danh sách lên ảnh
        for (auto& hist : hists) {
            auto frq = hist.getHist();
            auto mask = hist.getColorMask();
            double last_x = 0, last_y = res.rows;

            // với mỗi bin màu
            for (int bin = 0; bin < num_bins; ++bin) {

                // tính tọa độ cần vẽ của điểm histogram
                // vì khoảng cách rộng giữa các điểm là @bin_width nên hoành độ của bin @i là @i * @bin_width
                auto x = bin_width * bin;
                
                // tung độ đánh số từ trên xuống dưới nên phải đảo ngược lại bằng 'res.rows -'
                // frq[i] / max_height là tỉ lệ của histogram của bin i với bin cao nhất
                // bin cao nhất có chiều cao trên ảnh là @res.rows nên lấy tỉ lệ đó nhân với @res.rows là ra chiều cao của bin hiện tại trên ảnh
                auto y = res.rows - 1.0 * frq[bin] / max_height * res.rows;

                // mỗi điểm histogram sẽ có màu tượng trưng cho bin tương ứng
                // màu tăng dần từ 0 tới 255, bước nhảy là 256 / số bin
                int color = bin * 256.0 / num_bins;

                // vẽ điểm histogram lên ảnh
                // màu của điểm này sẽ được and với mặt nạ để vẽ đúng kênh màu
                // ví dụ muốn vẽ kênh màu đỏ = (0, 0, 255) thì khi and với mask thì 2 màu xanh lúc nào cũng là 0, màu đỏ là color
                cv::circle(res, cv::Point(x, y), 5, cv::Vec3b(mask[0] & color, mask[1] & color, mask[2] & color), -1);

                // nếu không phải là bin đầu tiên thì vẽ đoạn thẳng nối giữa điểm histogram của bin này với bin trước
                if (bin != 0) {
                    cv::line(res, cv::Point(x, y), cv::Point(last_x, last_y), mask);
                }

                // lưu lại tọa độ điểm histogram hiện tại để lần sau có thể vẽ đoạn thẳng nối tới nó
                last_x = x, last_y = y;
            }
        }

        return *this;
    }
};
