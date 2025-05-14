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
            ~CStreamFilterGraph();
        public:
            int filter_video(const std::string& out, const std::string& video_path);
        private:
            bool is_init_ = false;
            AVDictionary *opts_ = nullptr;
    };
}