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

    AVCodecContext* CFFmpegHelper::create_video_encodec_context(const AVCodecParameters* const codec_parameter, AVRational time_base, AVRational framerate)
    {
        if (!codec_parameter)
        {
            std::cerr << "codec_parameter is nullptr" << std::endl;
            return nullptr;
        }
        const AVCodec* video_encoder = avcodec_find_encoder(codec_parameter->codec_id);
        if (!video_encoder) 
        {
            std::cerr << "Unsupported video codec" << codec_parameter->codec_id << std::endl;
            return nullptr;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(video_encoder);
        if (!codec_ctx)
        {
            std::cerr << "Failed to allocate video codec context" << std::endl;
            return nullptr;
        }
        avcodec_parameters_to_context(codec_ctx, codec_parameter);
        if (time_base.num > 0 && time_base.den > 0)
        {
            codec_ctx->time_base = time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, 25}; // 默认25fps
        }
        if (framerate.num > 0 && framerate.den > 0)
        {
            codec_ctx->framerate = framerate;
        }
        else
        {
            codec_ctx->framerate = AVRational{25, 1}; // 默认25fps
        }
        avcodec_open2(codec_ctx, video_encoder, NULL);

        return codec_ctx;
    }

    AVCodecContext* CFFmpegHelper::create_video_decodec_context(const AVCodecParameters* const codec_parameter, AVRational time_base, AVRational framerate)
    {
        if (!codec_parameter)
        {
            std::cerr << term_color::red << "codec_parameter is nullptr" << term_color::reset << std::endl;
            return nullptr;
        }
        const AVCodec* video_decoder = avcodec_find_decoder(codec_parameter->codec_id);
        if (!video_decoder) 
        {
            std::cerr << term_color::red << "Unsupported video codec" << term_color::reset << codec_parameter->codec_id << std::endl;
            return nullptr;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(video_decoder);
        if (!codec_ctx)
        {
            std::cerr << term_color::red << "Failed to allocate video codec context" << term_color::reset << std::endl;
            return nullptr;
        }
        avcodec_parameters_to_context(codec_ctx, codec_parameter);

         if (time_base.num > 0 && time_base.den > 0)    
        {
            codec_ctx->time_base = time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, 25}; // 默认25fps
        }
        if (framerate.num > 0 && framerate.den > 0)
        {
            codec_ctx->framerate = framerate;
        }
        else
        {
            codec_ctx->framerate = AVRational{25, 1}; // 默认25fps
        }
        avcodec_open2(codec_ctx, video_decoder, NULL);

        return codec_ctx;
    }
}   
