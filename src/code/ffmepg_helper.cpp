#include "ffmepg_helper.h"

namespace stream_project
{
    int CFFmpegHelper::get_video_stream_index(const AVFormatContext * const fmt_ctx)
    {
        if (!fmt_ctx)
        {
            std::cerr << term_color::red << "fmt_ctx is nullptr" << term_color::reset << std::endl;
            return -1;
        }
        for (int i = 0; i < fmt_ctx->nb_streams; i++)
        {
            if (fmt_ctx->streams[i] == nullptr)
            {
                std::cerr << term_color::red << "fmt_ctx->streams[i] is nullptr" << term_color::reset << std::endl;
                continue;
            }
            if (fmt_ctx->streams[i]->codecpar == nullptr)
            {
                std::cerr << "fmt_ctx->streams[i]->codecpar is nullptr" << std::endl;
                continue;
            }
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
            {
                return i;
            }
        }
        return -1;
    }

    int CFFmpegHelper::get_audio_stream_index(const AVFormatContext * const fmt_ctx)
    {
        if (!fmt_ctx)
        {
            std::cerr << "fmt_ctx is nullptr" << std::endl;
            return -1;
        }
        for (int i = 0; i < fmt_ctx->nb_streams; i++)
        {
            if (fmt_ctx->streams[i] == nullptr)
            {
                std::cerr << "fmt_ctx->streams[i] is nullptr" << std::endl;
                continue;
            }
            if (fmt_ctx->streams[i]->codecpar == nullptr)
            {
                std::cerr << "fmt_ctx->streams[i]->codecpar is nullptr" << std::endl;
                continue;
            }
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
            {
                return i;
            }
        }
        return -1;
    }

    std::string CFFmpegHelper::get_input_format_name(const AVFormatContext * const input_fmt_ctx)
    {
        if (!input_fmt_ctx)
        {
            std::cerr << term_color::red << "input_fmt_ctx is nullptr" << term_color::reset << std::endl;
            return "";
        }
        std::string input_format_name = input_fmt_ctx->iformat->name;
        size_t comma_pos = input_format_name.find(',');
        if (comma_pos != std::string::npos) 
        {
            input_format_name = input_format_name.substr(0, comma_pos); 
        }
        return input_format_name;
    }


    AVCodecContext* CFFmpegHelper::create_video_encodec_context(const AVStream* const video_stream, bool open_codec)
    {
        if (!video_stream)
        {
            std::cerr << "video_stream is nullptr" << std::endl;
            return nullptr;
        }
        const AVCodec* video_encoder = avcodec_find_encoder(video_stream->codecpar->codec_id);
        if (!video_encoder) 
        {
            std::cerr << "Unsupported video codec" << video_stream->codecpar->codec_id << std::endl;
            return nullptr;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(video_encoder);
        if (!codec_ctx)
        {
            std::cerr << "Failed to allocate video codec context" << std::endl;
            return nullptr;
        }
        avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);
        
        if (video_stream->time_base.num > 0 && video_stream->time_base.den > 0)
        {
            codec_ctx->time_base = video_stream->time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, 25}; // 默认25fps
        }
        if (video_stream->r_frame_rate.num > 0 && video_stream->r_frame_rate.den > 0)
        {
            codec_ctx->framerate = video_stream->r_frame_rate;
        }
        else
        {
            codec_ctx->framerate = AVRational{25, 1}; // 默认25fps
        }
        if (open_codec)
        {
            avcodec_open2(codec_ctx, video_encoder, NULL);
        }

        return codec_ctx;
    }

    AVCodecContext* CFFmpegHelper::create_video_decodec_context(const AVStream* const video_stream, bool open_codec)
    {
        if (!video_stream)
        {
            std::cerr << term_color::red << "video_stream is nullptr" << term_color::reset << std::endl;
            return nullptr;
        }
        const AVCodec* video_decoder = avcodec_find_decoder(video_stream->codecpar->codec_id);
        if (!video_decoder) 
        {
            std::cerr << term_color::red << "Unsupported video codec" << term_color::reset << video_stream->codecpar->codec_id << std::endl;
            return nullptr;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(video_decoder);
        if (!codec_ctx)
        {
            std::cerr << term_color::red << "Failed to allocate video codec context" << term_color::reset << std::endl;
            return nullptr;
        }
        avcodec_parameters_to_context(codec_ctx, video_stream->codecpar);

        if (video_stream->time_base.num > 0 && video_stream->time_base.den > 0)    
        {
            codec_ctx->time_base = video_stream->time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, 25}; // 默认25fps
        }
        if (video_stream->r_frame_rate.num > 0 && video_stream->r_frame_rate.den > 0)
        {
            codec_ctx->framerate = video_stream->r_frame_rate;
        }
        else
        {
            codec_ctx->framerate = AVRational{25, 1}; // 默认25fps
        }
        if (open_codec)
        {
            avcodec_open2(codec_ctx, video_decoder, NULL);
        }

        return codec_ctx;
    }
}   
