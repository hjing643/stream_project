#pragma once
#include "../includeall.h"

struct FileFormat
{

};

class CStreamTransfer
{
    public:

        // h264 raw data to others
        bool format_to_mp4(std::string& out, const std::string& video_path);
        bool format_to_avi(std::string& out, const std::string& video_path);
        bool format_to_yuv(std::string& out, const std::string& video_path);
        bool format_yuv_to_rgb(std::string& out, const std::string& video_path);

        // mp4 to h264
        bool format_mp4_to_raw(std::string& out, const std::string& video_path);

        bool analyze_file(FileFormat& file_format, const std::string& video_path);
    private:
};