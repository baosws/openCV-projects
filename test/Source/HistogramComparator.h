#pragma once
#include <vector>
#include <algorithm>
#include <numeric>
#include "Histogram.h"
#include "opencv2/imgproc/imgproc.hpp" // cần hàm cvtColor

#define CMP_MD_CORELATION "corelation" // hàm tính khoảng cách corelation
#define CMP_MD_CHISQ      "chisq"      // hàm tính khoảng cách Chi-square
#define CMP_MD_INTERSECT  "intersect"  // hàm tính khoảng cách intersect

// lớp hỗ trợ so sánh 2 histogram
class HistogramComparator {
public:
    typedef double (*comparator_type)(const Histogram&, const Histogram&);
    
    HistogramComparator(comparator_type cmp) : cmp(cmp) {}

    /**
     * hàm khởi tạo dựa vào mã chế độ so sánh
     * @mode: chế độ so sánh, nên bằng CMP_MD_CORELATION, CMP_MD_CHISQ hoặc CMP_MD_INTERSECT
     */
    HistogramComparator(const std::string& mode) {
        // nếu mã chế độ so sánh là corelation thì dùng hàm so sánh corelation
        if (mode == CMP_MD_CORELATION) {
            cmp = HistogramComparator::corelation;
            return;
        }

        // nếu mã chế độ so sánh là intersect thì dùng hàm so sánh intesect
        if (mode == CMP_MD_INTERSECT) {
            cmp = HistogramComparator::intersect;
            return;
        }

        // nếu mã chế độ so sánh là chisq thì dùng hàm so sánh chisq
        if (mode == CMP_MD_CHISQ) {
            cmp = HistogramComparator::chisq;
            return;
        }
    }
    
    /**
     * hàm so sánh bằng corelation
     * công thức: https://docs.opencv.org/3.4/d8/dc8/tutorial_histogram_comparison.html
     * 
     * @H1: histogram thứ nhất
     * @H2: histogram thứ 2
     * @return: độ chênh lệch giữa 2 histogram
     */
    static double corelation(const Histogram& H1, const Histogram& H2) {
        // trích 2 vector histogram
        auto h1 = H1.getHist(), h2 = H2.getHist();

        // tính số bin
        int num_bins = H1.getBins();

        // tính trung bình của histogram thứ nhất
        auto h1_bar = std::accumulate(h1.begin(), h1.end(), 0) / num_bins;

        // tính trung bình của histogram thứ 2
        auto h2_bar = std::accumulate(h2.begin(), h2.end(), 0) / num_bins;

        double numerator = 0, var1 = 0, var2 = 0;
        for (int i = 0; i < num_bins; ++i) {
            numerator += (h1[i] - h1_bar) * (h2[i] - h2_bar);
            var1 += std::pow(h1[i] - h1_bar, 2);
            var2 += std::pow(h2[i] - h2_bar, 2);
        }

        return numerator / std::sqrt(var1 * var2);
    }
    
    /**
     * hàm so sánh bằng Chi-square
     * công thức: dist(H1, H2) = sum((H1[i] - H2[i]) ^ 2 / H1[i]) for every intensity i
     * 
     * @H1: histogram thứ nhất
     * @H2: histogram thứ 2
     * @return: độ chênh lệch giữa 2 histogram
     */
    static double chisq(const Histogram& H1, const Histogram& H2) {
        // trích 2 vector histogram
        auto h1 = H1.getHist(), h2 = H2.getHist();

        // tính số bin
        int num_bins = H1.getBins();

        double dist = 0;
        for (int i = 0; i < num_bins; ++i) {
            if (h1[i] != 0) {
                dist += std::pow(h1[i] - h2[i], 2) / h1[i];
            }
        }

        return dist;
    }
    
    /**
     * hàm so sánh bằng intersect
     * công thức: dist(H1, H2) = min(H1[i], H2[i]) for every intensity i
     * 
     * @H1: histogram thứ nhất
     * @H2: histogram thứ 2
     * @return: độ chênh lệch giữa 2 histogram
     */
    static double intersect(const Histogram& H1, const Histogram& H2) {
        // trích 2 vector histogram
        auto h1 = H1.getHist(), h2 = H2.getHist();

        // tính số bin
        int num_bins = H1.getBins();

        double dist = 0;
        for (int i = 0; i < num_bins; ++i) {
            dist += std::min(h1[i], h2[i]);
        }

        return dist;
    }
    
    /**
     * phương thức so sánh 2 histogram
     * @H1: histogram thứ nhất
     * @H2: histogram thứ 2
     * @return: độ chênh lệch giữa 2 histogram theo phương thức so sánh đã chọn
     */
    double operator()(const Histogram& H1, const Histogram& H2) const {
        return cmp(H1, H2);
    }
    
private:
    // con trỏ hàm tới hàm dùng để so sánh
    comparator_type cmp;
};
