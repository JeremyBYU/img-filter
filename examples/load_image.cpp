#include "img_filters.hpp"

const std::string DEFAULT_DEPTH_IMAGE = "./fixtures/realsense/test_depth.raw";

int main(int argc, char const *argv[])
{
    auto console = spdlog::stdout_logger_st("console");
    console->info("Starting up");
    auto data = img_filters::get_depth_image(DEFAULT_DEPTH_IMAGE);
    // std::cout << "After call" <<std::endl;
    return 0;
}
