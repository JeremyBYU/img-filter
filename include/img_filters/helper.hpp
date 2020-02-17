#ifndef IMGFILTERS_HELPER
#define IMGFILTERS_HELPER

#include <string>
#include <vector>
#include <tuple>


namespace img_filters {

struct img_details {
    size_t h;
    size_t w;
    size_t bpp;
    size_t stride;
};

struct depth_info {
    float stereo_baseline_mm;
    float focal_length_x_mm;
    float depth_units_m;
    float d2d_convert_factor;
};

using Z16_BUFFER = std::vector< unsigned  short>;

std::tuple<img_details, depth_info> get_depth_metadata(std::string fpath);
Z16_BUFFER get_depth_image(std::string fpath, int w=848, int h=480);

}

#endif