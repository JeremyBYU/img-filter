#ifndef IMGFILTERS_HELPER
#define IMGFILTERS_HELPER

#include <string>
#include <vector>



namespace img_filters {
using Z16_BUFFER = std::vector< unsigned  short>;

Z16_BUFFER get_depth_image(std::string fpath, int w=848, int h=480);

}

#endif