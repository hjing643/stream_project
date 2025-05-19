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

        static bool is_keyframe(AVPacket* packet);
        static int estimate_bitrate(int width, int height, int fps = 25);
        // clone input to output
        static AVStream* clone_new_video_stream(AVFormatContext* input_fmt_ctx, AVFormatContext* output_fmt_ctx);
        // only for encode
        static AVStream* create_new_video_stream(AVCodecContext* encoder_ctx, AVFormatContext* output_fmt_ctx, AVCodecID codec_id);
        static AVStream* create_new_audio_stream(AVCodecContext* encoder_ctx, AVFormatContext* output_fmt_ctx, AVCodecID codec_id);
        
        static AVCodecContext* create_video_encodec_context(const AVStream* const video_stream, AVCodecID codec_id, bool open_codec = true);
        static AVCodecContext* create_video_decodec_context(const AVStream* const video_stream, bool open_codec = true);

        static AVCodecContext* create_audio_encodec_context(const AVStream* const audio_stream, AVCodecID codec_id, bool open_codec = true);
        static AVCodecContext* create_audio_decodec_context(const AVStream* const audio_stream, bool open_codec = true);

        static std::string get_input_format_name(const AVFormatContext * const fmt_ctx);
    };  
}