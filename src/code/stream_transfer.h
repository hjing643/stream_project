#pragma once
#include "../includeall.h"

struct FileFormat
{

};

class CStreamTransfer
{
    public:
        int init();

        // h264 raw data to others
        int format_to_mp4(const std::string& out, const std::string& video_path);
        int format_to_avi(const std::string& out, const std::string& video_path);
        int format_to_yuv(const std::string& out, const std::string& video_path);
        int format_yuv_to_rgb(const std::string& out, const std::string& video_path);

        // get the frame
        int get_first_I_frame(const std::string& out, const std::string& video_path);
        int get_first_P_frame(const std::string& out, const std::string& video_path);
        int I_frame_to_yuv(const std::string& out, const std::string& video_path);
        int P_frame_to_yuv(const std::string& out, const std::string& video_path);

        // mp4 to h264
        int format_mp4_to_raw(const std::string& out, const std::string& video_path);

        // analyze the file
        int analyze_file(FileFormat& file_format, const std::string& video_path);
    private:
};