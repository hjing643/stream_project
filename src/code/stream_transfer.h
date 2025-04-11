#pragma once
#include "includeall.h"
class CStreamTransfer
{
    public:

        // h264 raw data to others
        bool format_to_mp4(std::stirng& out, const std::string& video_path);
        bool format_to_avi(std::stirng& out, const std::string& video_path);
        bool format_to_yuv(std::stirng& out, const std::string& video_path);
        bool format_yuv_to_rgb(std::stirng& out, const std::string& video_path);

        // mp4 to h264
        bool format_mp4_to_raw(std::stirng& out, const std::string& video_path);


    private:
};