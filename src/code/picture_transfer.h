#pragma once
#include "../includeall.h"
namespace stream_project
{
    class CPictureTransfer
    {
        public:
            int init();
            // yuv/rgb to picture
            // src_fmt: 0-yuv, 1-rgb
            // dst_codec_id: AV_CODEC_ID_PNG, AV_CODEC_ID_MJPEG
            int transfer_raw_to_picture(const std::string& input_path, AVPixelFormat src_fmt, int width, int height, const std::string& output_path, AVCodecID dst_codec_id);
        private:
            bool is_init_ = false;
    };
}
