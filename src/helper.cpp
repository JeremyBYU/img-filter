#include "img_filters.hpp"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>

namespace img_filters {
// CONSOLE = spdlog::stdout_logger_st("console");
Z16_BUFFER get_depth_image(std::string fpath, int w, int h)
{
    auto logger = spdlog::get("console");
    Z16_BUFFER img_data(w * h);
    struct stat results;
    size_t num_pixels = w * h;
    size_t num_bytes = num_pixels * 2;
    if (stat(fpath.c_str(), &results) == 0)
    {
        if(logger)
        {
            logger->info("Size is: {}; Expected: {}",results.st_size ,num_bytes);
        }
        
    }
    else
    {
        if (logger)
            logger->error("Error occurred reading file",results.st_size ,num_bytes);
    }
    return img_data;
    // fread (img_data.data(), 2, num_pixels, FILE * stream );
}

} // namespace img_filters