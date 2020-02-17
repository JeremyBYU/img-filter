#include "img_filters.hpp"

const std::string DEFAULT_DEPTH_IMAGE = "./fixtures/realsense/test_depth.raw";

int main(int argc, char const *argv[])
{
    auto console = spdlog::stderr_color_st("console");
    console->info("Starting up");
    img_filters::get_depth_image(DEFAULT_DEPTH_IMAGE);
    return 0;
}
