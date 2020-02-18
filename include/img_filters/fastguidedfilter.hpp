// Source Code Taken From: https://github.com/atilimcetin/guided-filter
// License: MIT

#ifndef GUIDED_FILTER_H
#define GUIDED_FILTER_H

#include <opencv2/opencv.hpp>

namespace img_filters {

class FastGuidedFilterImpl;

class FastGuidedFilter
{
public:
    FastGuidedFilter(const cv::Mat &I, int r, double eps,int s);
    ~FastGuidedFilter();

    cv::Mat filter(const cv::Mat &p, int depth = -1) const;

private:
    FastGuidedFilterImpl *impl_;
};

cv::Mat fastGuidedFilter(const cv::Mat &I, const cv::Mat &p, int r, double eps, int s = 1,int depth = -1);

}

#endif
