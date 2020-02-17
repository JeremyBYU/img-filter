#include "img_filters.hpp"
#include <stdio.h>
#include <fstream>
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
        if (logger)
        {
            logger->info("Size is: {}; Expected: {}", results.st_size, num_bytes);
        }
    }
    else
    {
        if (logger)
            logger->error("Error occurred reading file", results.st_size, num_bytes);
    }
    std::ifstream myFile(fpath.c_str(), std::ios::in | std::ios::binary);
    // fread(img_data.data(), 2, num_pixels, myFile);
    myFile.read(reinterpret_cast<char*>(img_data.data()), num_bytes);
    return img_data;
}

} // namespace img_filters