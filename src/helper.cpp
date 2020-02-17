#include "img_filters.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>

namespace img_filters {

Z16_BUFFER get_depth_image(std::string fpath, int w, int h)
{
    Z16_BUFFER img_data(w * h);
    struct stat results;
    size_t num_pixels = w * h;
    size_t num_bytes = num_pixels * 2;
    if (stat(fpath.c_str(), &results) == 0)
    {
        spdlog::get("console")->debug("Size is: " << results.st_size << std::endl;
        std::cout << "Expected: " << num_bytes << std::endl;
    }
    else
    {
        std::cout<< "Error occurred" << std::endl;
    }
    return img_data;
    // fread (img_data.data(), 2, num_pixels, FILE * stream );
}

} // namespace img_filters