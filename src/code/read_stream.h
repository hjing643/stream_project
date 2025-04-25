#pragma once

#include "../includeall.h"

class CReadStream
{
    public:
        int init();

        // read the stream
        int read_mp4_box(const std::string& box_name, const std::string& video_path);
        
        // read the h264 nalu
        int read_h264_nalu(const std::string& naul_name, const std::string& video_path);

};