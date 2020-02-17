#include "img_filters.hpp"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>

using namespace std::chrono_literals;
using namespace std::string_literals;

const std::string DEFAULT_DEPTH_IMAGE = "./fixtures/realsense/test_depth.raw";


cv::Mat convert_to_color(img_filters::Z16_BUFFER &data, img_filters::img_details &details, float max_depth=4.0)
{
    // Convert to OpenCV for viewing
    cv::Mat depth_image(cv::Size(details.w, details.h) , CV_16UC1 , (void *)data.data() , cv::Mat::AUTO_STEP);
    cv::Mat depth_color = cv::Mat(details.h, details.w, CV_8UC3);   // container for false-color version
    double scale_factor = 255.0 / ((max_depth / 10.0) * 10000);
    depth_image.convertTo(depth_color, CV_8UC3, scale_factor);
    return depth_color;
}
int main(int argc, char const *argv[])
{
    img_filters::img_details details {480, 848, 2, 2};
    auto console = spdlog::stdout_logger_st("console");
    console->info("Starting Image Filter Demo");
    auto data = img_filters::get_depth_image(DEFAULT_DEPTH_IMAGE, details.w, details.h);
    float alpha = 0.3, delta=25.0;
    uint8_t hole_filling_mode = 1;
    int iterations = 3;
    auto t0 = std::chrono::high_resolution_clock::now();
    auto rs_filtered_image = img_filters::rs::spatial_filter(data, details, alpha, delta, iterations, hole_filling_mode);
    auto t1 = std::chrono::high_resolution_clock::now();
    float elapsed_d = static_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count();

    // Convert to OpenCV for viewing
    auto depth_color = convert_to_color(data, details);
    auto rs_filtered_color = convert_to_color(rs_filtered_image, details);

    std::string windowName = "Image";
    cv::namedWindow(windowName); // Create a window

    cv::Mat combined = cv::Mat(details.h, details.w, CV_8UC3);
    cv::hconcat(depth_color,rs_filtered_color, combined);
    console->info("Realsense Filter Before/After; Elapsed Time (ms): {:.2f}", elapsed_d);
    cv::imshow(windowName, combined); // Show our image inside the created window.
    cv::waitKey(0); // Wait for any keystroke in the window

    return 0;
}
