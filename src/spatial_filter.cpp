// Copy from RealSense Filters Code
// License: Apache 2.0 - See: https://raw.githubusercontent.com/IntelRealSense/librealsense/master/LICENSE
//

#include "img_filters.hpp"

namespace img_filters {

namespace rs {

enum spatial_holes_filling_types : uint8_t
{
    sp_hf_disabled,
    sp_hf_2_pixel_radius,
    sp_hf_4_pixel_radius,
    sp_hf_8_pixel_radius,
    sp_hf_16_pixel_radius,
    sp_hf_unlimited_radius,
    sp_hf_max_value
};

uint8_t get_hole_filling_radius(uint8_t holes_filling_mode)
{
    uint8_t holes_filling_radius = 0;
    switch (holes_filling_mode)
    {
    case sp_hf_disabled:
        holes_filling_radius = 0; // disabled
        break;
    case sp_hf_unlimited_radius:
        holes_filling_radius = 0xff; // Unrealistic smearing; not particulary useful
        break;
    case sp_hf_2_pixel_radius:
    case sp_hf_4_pixel_radius:
    case sp_hf_8_pixel_radius:
    case sp_hf_16_pixel_radius:
        holes_filling_radius = 0x1 << holes_filling_mode; // 2's exponential radius
        break;
    default:
        holes_filling_radius = 0;
        break;
    }
    return holes_filling_radius;
}

template <typename T>
std::vector<T> spatial_filter(std::vector<T>& source, img_details& details, float spatial_alpha_param, float spatial_edge_threshold, int spatial_iterations, uint8_t holes_filling_mode)
{

    auto tgt = prepare_target_frame<T>(source, details);
    auto holes_filling_radius = get_hole_filling_radius(holes_filling_mode);

    static_assert((std::is_arithmetic<T>::value), "Spatial filter assumes numeric types");
    bool fp = (std::is_floating_point<T>::value);

    // Spatial domain transform edge-preserving filter
    if (fp)
        dxf_smooth<float>(const_cast<void*>(tgt.get_data()), spatial_alpha_param, spatial_edge_threshold, spatial_iterations, holes_filling_mode, holes_filling_radius, details);
    else
        dxf_smooth<uint16_t>(const_cast<void*>(tgt.get_data()), spatial_alpha_param, spatial_edge_threshold, spatial_iterations, holes_filling_mode, holes_filling_radius, details);

    return tgt;
}

// template <typename T>
std::vector<unsigned short> spatial_filter(std::vector<unsigned short>& source, img_details& details, float spatial_alpha_param, float spatial_edge_threshold, int spatial_iterations, uint8_t holes_filling_mode)
{

    auto tgt = prepare_target_frame<unsigned short>(source, details);
    auto holes_filling_radius = get_hole_filling_radius(holes_filling_mode);


    dxf_smooth<unsigned short>(static_cast<void*>(tgt.data()), spatial_alpha_param, spatial_edge_threshold, spatial_iterations, holes_filling_mode, holes_filling_radius, details);

    return tgt;
}

// void  spatial_filter::update_configuration(const rs2::frame& f)
// {
//     if (f.get_profile().get() != _source_stream_profile.get())
//     {
//         _source_stream_profile = f.get_profile();
//         _target_stream_profile = _source_stream_profile.clone(RS2_STREAM_DEPTH, 0, _source_stream_profile.format());

//         _extension_type = f.is<rs2::disparity_frame>() ? RS2_EXTENSION_DISPARITY_FRAME : RS2_EXTENSION_DEPTH_FRAME;
//         _bpp = (_extension_type == RS2_EXTENSION_DISPARITY_FRAME) ? sizeof(float) : sizeof(uint16_t);
//         auto vp = _target_stream_profile.as<rs2::video_stream_profile>();
//         _focal_lenght_mm = vp.get_intrinsics().fx;
//         _width = vp.width();
//         _height = vp.height();
//         _stride = _width * _bpp;
//         _current_frm_size_pixels = _width * _height;

//         // Check if the new frame originated from stereo-based depth sensor
//         // retrieve the stereo baseline parameter
//         // TODO refactor disparity parameters into the frame's metadata
//         auto snr = ((frame_interface*)f.get())->get_sensor().get();
//         librealsense::depth_stereo_sensor* dss;

//         // Playback sensor
//         if (auto a = As<librealsense::extendable_interface>(snr))
//         {
//             librealsense::depth_stereo_sensor* ptr;
//             if ((_stereoscopic_depth = a->extend_to(TypeToExtension<librealsense::depth_stereo_sensor>::value, (void**)&ptr)))
//             {
//                 dss = ptr;
//                 _stereo_baseline_mm = dss->get_stereo_baseline_mm();
//             }
//         }
//         else if (auto depth_emul = As<librealsense::software_sensor>(snr))
//         {
//             // Software device can obtain these options via Options interface
//             if (depth_emul->supports_option(RS2_OPTION_STEREO_BASELINE))
//             {
//                 _stereo_baseline_mm = depth_emul->get_option(RS2_OPTION_STEREO_BASELINE).query()*1000.f;
//                 _stereoscopic_depth = true;
//             }
//         }
//         else // Live sensor
//         {
//             _stereoscopic_depth = Is<librealsense::depth_stereo_sensor>(snr);
//             dss = As<librealsense::depth_stereo_sensor>(snr);
//             if (_stereoscopic_depth)
//                 _stereo_baseline_mm = dss->get_stereo_baseline_mm();
//         }

//         _spatial_edge_threshold = _spatial_delta_param;// (_extension_type == RS2_EXTENSION_DISPARITY_FRAME) ?
//                                                        // (_focal_lenght_mm * _stereo_baseline_mm) / float(_spatial_delta_param) : _spatial_delta_param;
//     }
// }
template <typename T>
std::vector<T> prepare_target_frame(std::vector<T>& source, img_details& details)
{
    // Allocate and copy the content of the original Depth data to the target
    std::vector<T> new_data(source);
    return new_data;
}

void recursive_filter_horizontal_fp(void* image_data, float alpha, float deltaZ, img_details& details)
{
    float* image = reinterpret_cast<float*>(image_data);
    auto _height = static_cast<int>(details.h);
    auto _width = static_cast<int>(details.w);
    int v, u;

    for (v = 0; v < _height;)
    {
        // left to right
        float* im = image + v * _width;
        float state = *im;
        float previousInnovation = state;

        im++;
        float innovation = *im;
        u = int(_width) - 1;
        if (!(*(int*)&previousInnovation > 0))
            goto CurrentlyInvalidLR;
        // else fall through

    CurrentlyValidLR:
        for (;;)
        {
            if (*(int*)&innovation > 0)
            {
                float delta = previousInnovation - innovation;
                bool smallDifference = delta < deltaZ && delta > -deltaZ;

                if (smallDifference)
                {
                    float filtered = innovation * alpha + state * (1.0f - alpha);
                    *im = state = filtered;
                }
                else
                {
                    state = innovation;
                }
                u--;
                if (u <= 0)
                    goto DoneLR;
                previousInnovation = innovation;
                im += 1;
                innovation = *im;
            }
            else
            { // switch to CurrentlyInvalid state
                u--;
                if (u <= 0)
                    goto DoneLR;
                previousInnovation = innovation;
                im += 1;
                innovation = *im;
                goto CurrentlyInvalidLR;
            }
        }

    CurrentlyInvalidLR:
        for (;;)
        {
            u--;
            if (u <= 0)
                goto DoneLR;
            if (*(int*)&innovation > 0)
            { // switch to CurrentlyValid state
                previousInnovation = state = innovation;
                im += 1;
                innovation = *im;
                goto CurrentlyValidLR;
            }
            else
            {
                im += 1;
                innovation = *im;
            }
        }
    DoneLR:

        // right to left
        im = image + (v + 1) * _width - 2; // end of row - two pixels
        previousInnovation = state = im[1];
        u = int(_width) - 1;
        innovation = *im;
        if (!(*(int*)&previousInnovation > 0))
            goto CurrentlyInvalidRL;
        // else fall through
    CurrentlyValidRL:
        for (;;)
        {
            if (*(int*)&innovation > 0)
            {
                float delta = previousInnovation - innovation;
                bool smallDifference = delta < deltaZ && delta > -deltaZ;

                if (smallDifference)
                {
                    float filtered = innovation * alpha + state * (1.0f - alpha);
                    *im = state = filtered;
                }
                else
                {
                    state = innovation;
                }
                u--;
                if (u <= 0)
                    goto DoneRL;
                previousInnovation = innovation;
                im -= 1;
                innovation = *im;
            }
            else
            { // switch to CurrentlyInvalid state
                u--;
                if (u <= 0)
                    goto DoneRL;
                previousInnovation = innovation;
                im -= 1;
                innovation = *im;
                goto CurrentlyInvalidRL;
            }
        }

    CurrentlyInvalidRL:
        for (;;)
        {
            u--;
            if (u <= 0)
                goto DoneRL;
            if (*(int*)&innovation > 0)
            { // switch to CurrentlyValid state
                previousInnovation = state = innovation;
                im -= 1;
                innovation = *im;
                goto CurrentlyValidRL;
            }
            else
            {
                im -= 1;
                innovation = *im;
            }
        }
    DoneRL:
        v++;
    }
}

void recursive_filter_vertical_fp(void* image_data, float alpha, float deltaZ, img_details& details)
{
    float* image = reinterpret_cast<float*>(image_data);

    auto _height = static_cast<int>(details.h);
    auto _width = static_cast<int>(details.w);

    int v, u;

    // we'll do one column at a time, top to bottom, bottom to top, left to right,

    for (u = 0; u < _width;)
    {

        float* im = image + u;
        float state = im[0];
        float previousInnovation = state;

        v = int(_height) - 1;
        im += _width;
        float innovation = *im;

        if (!(*(int*)&previousInnovation > 0))
            goto CurrentlyInvalidTB;
        // else fall through

    CurrentlyValidTB:
        for (;;)
        {
            if (*(int*)&innovation > 0)
            {
                float delta = previousInnovation - innovation;
                bool smallDifference = delta < deltaZ && delta > -deltaZ;

                if (smallDifference)
                {
                    float filtered = innovation * alpha + state * (1.0f - alpha);
                    *im = state = filtered;
                }
                else
                {
                    state = innovation;
                }
                v--;
                if (v <= 0)
                    goto DoneTB;
                previousInnovation = innovation;
                im += _width;
                innovation = *im;
            }
            else
            { // switch to CurrentlyInvalid state
                v--;
                if (v <= 0)
                    goto DoneTB;
                previousInnovation = innovation;
                im += _width;
                innovation = *im;
                goto CurrentlyInvalidTB;
            }
        }

    CurrentlyInvalidTB:
        for (;;)
        {
            v--;
            if (v <= 0)
                goto DoneTB;
            if (*(int*)&innovation > 0)
            { // switch to CurrentlyValid state
                previousInnovation = state = innovation;
                im += _width;
                innovation = *im;
                goto CurrentlyValidTB;
            }
            else
            {
                im += _width;
                innovation = *im;
            }
        }
    DoneTB:

        im = image + u + (_height - 2) * _width;
        state = im[_width];
        previousInnovation = state;
        innovation = *im;
        v = int(_height) - 1;
        if (!(*(int*)&previousInnovation > 0))
            goto CurrentlyInvalidBT;
        // else fall through
    CurrentlyValidBT:
        for (;;)
        {
            if (*(int*)&innovation > 0)
            {
                float delta = previousInnovation - innovation;
                bool smallDifference = delta < deltaZ && delta > -deltaZ;

                if (smallDifference)
                {
                    float filtered = innovation * alpha + state * (1.0f - alpha);
                    *im = state = filtered;
                }
                else
                {
                    state = innovation;
                }
                v--;
                if (v <= 0)
                    goto DoneBT;
                previousInnovation = innovation;
                im -= _width;
                innovation = *im;
            }
            else
            { // switch to CurrentlyInvalid state
                v--;
                if (v <= 0)
                    goto DoneBT;
                previousInnovation = innovation;
                im -= _width;
                innovation = *im;
                goto CurrentlyInvalidBT;
            }
        }

    CurrentlyInvalidBT:
        for (;;)
        {
            v--;
            if (v <= 0)
                goto DoneBT;
            if (*(int*)&innovation > 0)
            { // switch to CurrentlyValid state
                previousInnovation = state = innovation;
                im -= _width;
                innovation = *im;
                goto CurrentlyValidBT;
            }
            else
            {
                im -= _width;
                innovation = *im;
            }
        }
    DoneBT:
        u++;
    }
}

} // namespace rs

} // namespace img_filters