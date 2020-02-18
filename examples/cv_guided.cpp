#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>
#include <string>
#include <chrono>
#include "img_filters/fastguidedfilter.hpp"

using namespace cv;

int main(int argc, char **argv)
{
    cv::CommandLineParser parser(
        argc, argv,
        "{help h ? |     | help message}"
        "{@image   |     | Image filename to process }");
    if (parser.has("help") || !parser.has("@image"))
    {
        parser.printMessage();
        return 0;
    }
    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();
    float elapsed_time;

    // Load image from first parameter
    std::string filename = parser.get<std::string>("@image");
    Mat image = imread(filename, 1), res;

    if (!image.data)
    {
        std::cerr << "No image data at " << filename << std::endl;
        throw;
    }

    // Before filtering
    imshow("Original image", image);
    waitKey(0);

    // Initialize filter. Kernel size 5x5, threshold 20
    int radius = 2;
    double eps=0.0001;
    t0 = std::chrono::high_resolution_clock::now();
    ximgproc::guidedFilter(image, image, res, radius, eps);
    t1 = std::chrono::high_resolution_clock::now();
    elapsed_time = static_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count();
    std::cout << "Guided Filter Took (ms): " << elapsed_time << std::endl;

    // After filtering
    imshow("Filtered image", res);
    waitKey(0);

    // Initialize filter. Kernel size 5x5, threshold 20
    int stride = 1;
    Mat I;
    I = image;
    t0 = std::chrono::high_resolution_clock::now();
    res = img_filters::fastGuidedFilter(image, I, radius, eps, stride); // (image, image, res, radius, eps);
    t1 = std::chrono::high_resolution_clock::now();
    elapsed_time = static_cast<std::chrono::duration<float, std::milli>>(t1 - t0).count();
    std::cout << "Fast Guided Filter stride=1 (ms): " << elapsed_time << std::endl;

    // After filtering
    imshow("Filtered image", res);
    waitKey(0);




    return 0;
}