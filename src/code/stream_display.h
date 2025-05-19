// based on opencv
#pragma once
#include "../includeall.h"
namespace stream_project
{
    class CStreamDisplay
    {
        public:
            int init();
            int compress_png(const std::string &dst_path, const std::string &src_path);
            int png_to_jpg(const std::string &dst_path, const std::string &src_path);
            int display_video();
            int display_audio();
            int display_text();
            int display_pic();
            int display_hls();
            int display_yuv();
        private:
            bool is_init_ = false;
    };
}