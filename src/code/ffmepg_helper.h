#pragma once
#include "../includeall.h"
namespace stream_project
{
    namespace term_color {
    constexpr const char* red    = "\033[31m";
    constexpr const char* green  = "\033[32m";
    constexpr const char* yellow = "\033[33m";
    constexpr const char* reset  = "\033[0m";
}

    class CFFmpegHelper
    {
        public:

        static int get_video_stream_index(const AVFormatContext * const fmt_ctx);
        static int get_audio_stream_index(const AVFormatContext * const fmt_ctx);

        static AVCodecContext* create_video_encodec_context(const AVStream* const video_stream, bool open_codec = true);
        static AVCodecContext* create_video_decodec_context(const AVStream* const video_stream, bool open_codec = true);

        static std::string get_input_format_name(const AVFormatContext * const fmt_ctx);
    };  
}