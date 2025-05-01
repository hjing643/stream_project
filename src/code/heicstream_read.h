#pragma once
#include  "../includeall.h"

namespace stream_project
{
    class CHeicStreamRead
    {
        public:
        int init();

        // read the heic stream
        int transfer_heic_to_picture(const std::string& output_path, const std::string& heic_path, AVCodecID dst_codec_id);
    };
}