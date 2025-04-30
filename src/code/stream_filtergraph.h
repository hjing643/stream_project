// based on ffmpeg
// the filter is suitable for processing static or regularly changing positions, 
// but not for image positions that change dynamically in each frame. we need to use the opencv for this.
#pragma once
#include "../includeall.h"
namespace stream_project
{
    class CStreamFilterGraph
    {
        public:
            int init();

        public:
            int scale_video(const std::string& out, const std::string& video_path);
            int crop_video(const std::string& out, const std::string& video_path);
            int overload_video(const std::string& out, const std::string& video_path);
            int drawbox_video(const std::string& out, const std::string& video_path);
            int drawtext_video(const std::string& out, const std::string& video_path);
            int boxblur_video(const std::string& out, const std::string& video_path);
        private:
            bool is_init_ = false;
    };
}