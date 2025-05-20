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

    bool CFFmpegHelper::is_keyframe(AVPacket* packet)
    {
        if (!packet)
        {
            return false;
        }
        return packet->flags & AV_PKT_FLAG_KEY;
    }

    int CFFmpegHelper::estimate_bitrate(int width, int height, int fps)
    {
        int pixels = width * height;
        if (pixels <= 850*480) 
        {
            // 480P
            return 1*1024*1024;
        }
        else if (pixels <= 1280*720) 
        {
            // 720P
            return 2.5*1024*1024;
        }
        else if (pixels <= 1920*1080) 
        {
            // 1080P
            return 4*1024*1024;
        }
        else if (pixels <= 2560*1440)
        {
            // 2K
            return 8*1024*1024;
        }
        else if (pixels <= 3840*2160)
        {
            // 4K
            return 16*1024*1024;
        }
        // 默认更高分辨率
        return 8*1024*1024; 
    }
    std::string CFFmpegHelper::get_input_format_name(const AVFormatContext * const input_fmt_ctx)
    {
        if (!input_fmt_ctx)
        {
            std::cerr << term_color::red << "input_fmt_ctx is nullptr" << term_color::reset << std::endl;
            return "";
        }
        std::string input_format_name = input_fmt_ctx->iformat->name;

        if (input_format_name.find("mp4") != std::string::npos) 
        {
            return "mp4";
        }

        size_t comma_pos = input_format_name.find(',');
        if (comma_pos != std::string::npos) 
        {
            input_format_name = input_format_name.substr(0, comma_pos); 
        }
        return input_format_name;
    }
    AVStream* CFFmpegHelper::create_new_audio_stream(AVCodecContext* encoder_ctx, AVFormatContext* output_fmt_ctx, AVCodecID codec_id)
    {
        const AVCodec* encoder = avcodec_find_encoder(codec_id);
        if (!encoder) 
        {
            std::cerr << "Unsupported encoder" << std::endl;
            return nullptr;
        }
        AVStream* out_stream = avformat_new_stream(output_fmt_ctx, encoder);
        if (!out_stream) 
        {
            std::cerr << term_color::red << "Failed allocating output stream" << term_color::reset << std::endl;
            return nullptr;
        }
        avcodec_parameters_from_context(out_stream->codecpar, encoder_ctx);
        out_stream->time_base = (AVRational){1, 48000};
        out_stream->codecpar->sample_rate = 48000;
        // 设置编码器参数（48kHz, 立体声, FLTP）
        return out_stream;
    }

    AVStream* CFFmpegHelper::create_new_video_stream(AVCodecContext* encoder_ctx, AVFormatContext* output_fmt_ctx, AVCodecID codec_id)
    {
        const AVCodec* encoder = avcodec_find_encoder(codec_id);
        if (!encoder) 
        {
            std::cerr << "Unsupported encoder" << std::endl;
            return nullptr;
        }

        AVStream* out_stream = avformat_new_stream(output_fmt_ctx, encoder);
        if (!out_stream) 
        {
            std::cerr << term_color::red << "Failed allocating output stream" << term_color::reset << std::endl;
            return nullptr;
        }
    
        avcodec_parameters_from_context(out_stream->codecpar, encoder_ctx);
        std::cout <<"create_new_video_stream" << std::endl;
        std::cout << "out_stream->codecpar->codec_type:" << out_stream->codecpar->codec_type << std::endl;
        std::cout << "encoder_ctx->codec_type:" << encoder_ctx->codec_type << std::endl;
        std::cout << "encoder_ctx->time_base:" << encoder_ctx->time_base.num << "/" << encoder_ctx->time_base.den << std::endl;
        out_stream->time_base = encoder_ctx->time_base; //should be correct, otherwise the video and audio will not be synchronized
        out_stream->r_frame_rate = encoder_ctx->framerate;
        out_stream->avg_frame_rate = encoder_ctx->framerate;
        return out_stream;
    }

    AVStream* CFFmpegHelper::clone_new_video_stream(AVFormatContext* input_fmt_ctx, AVFormatContext* output_fmt_ctx)
    {
        if (!input_fmt_ctx)
        {
            std::cerr << term_color::red << "input_fmt_ctx is nullptr" << term_color::reset << std::endl;
            return nullptr;
        }
        for (unsigned int i = 0; i < input_fmt_ctx->nb_streams; i++) 
        {
            AVStream* in_stream = input_fmt_ctx->streams[i];
           
            if (in_stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) 
            {
                AVStream* out_stream = avformat_new_stream(output_fmt_ctx, nullptr);
                if (!out_stream) 
                {
                    std::cerr << term_color::red << "Failed allocating output stream" << term_color::reset << std::endl;
                    return nullptr;
                }
                
                // 复制流参数
                int ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
                if (ret < 0) 
                {
                    std::cerr << term_color::red << "Failed to copy codec parameters" << term_color::reset << std::endl;
                    return nullptr;
                }
                out_stream->codecpar->codec_tag = 0;

                // if src_fmt pts, dst is not valid(raw data), set to 25fps
                if(in_stream->time_base.num <= 0 || in_stream->time_base.den <= 0)
                {
                    std::cout << term_color::yellow << "src_fmt pts, dst is not valid(raw data), set to 25fps" << term_color::reset << std::endl;
                    AVRational mp4_time_base = (AVRational){1, 90000};
                    out_stream->time_base = mp4_time_base;
                }
                else
                {
                    out_stream->time_base = in_stream->time_base;
                }

                if(in_stream->r_frame_rate.num <= 0 || in_stream->r_frame_rate.den <= 0)
                {
                    std::cout << term_color::yellow << "src_fmt frame_rate, dst is not valid(raw data), set to 25fps" << term_color::reset << std::endl;
                    out_stream->r_frame_rate = (AVRational){25, 1};
                    out_stream->avg_frame_rate = (AVRational){25, 1};
                }
                else
                {
                    out_stream->r_frame_rate = in_stream->r_frame_rate;
                    out_stream->avg_frame_rate = in_stream->avg_frame_rate;
                }
                {
                    out_stream->codecpar->codec_tag = av_codec_get_tag(output_fmt_ctx->oformat->codec_tag, out_stream->codecpar->codec_id);
                    if (out_stream->codecpar->codec_tag == 0) 
                    {
                        // webm not support codec tag, it's only support in mp4, avi, mov, etc.
                        std::cerr << term_color::red << "Could not find codec tag for codec id " << out_stream->codecpar->codec_id << term_color::reset << "\n";
                    }
                    else
                    {
                        std::cout << term_color::green << "find codec tag from id " << out_stream->codecpar->codec_id
                         << " to " << out_stream->codecpar->codec_tag << term_color::reset << "\n";
                    }
                }
                return out_stream;
            }
        }
        return nullptr;
    }

    AVCodecContext* CFFmpegHelper::create_video_encodec_context(const AVStream* const video_stream, AVCodecID codec_id, bool open_codec)
    {
        if (!video_stream)
        {
            std::cerr << "video_stream is nullptr" << std::endl;
            return nullptr;
        }
        const AVCodec* video_encoder = avcodec_find_encoder(codec_id);
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

        // video_stream->codecpar is vp9, so set codec_id to h264
        codec_ctx->codec_id = video_encoder->id;
        codec_ctx->codec_type = video_encoder->type;
        codec_ctx->thread_count = 0;
        codec_ctx->thread_type = FF_THREAD_FRAME;
        
        if (video_stream->r_frame_rate.num > 0 && video_stream->r_frame_rate.den > 0)
        {
            codec_ctx->framerate = video_stream->r_frame_rate;
        }
        else
        {
            codec_ctx->framerate = AVRational{25, 1}; // 默认25fps
        }
        if (video_stream->time_base.num > 0 && video_stream->time_base.den > 0)
        {
            codec_ctx->time_base = video_stream->time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, codec_ctx->framerate.den};
        }
        if (video_stream->codecpar->bit_rate <= 0)
        {
            // if avcodec_parameters_to_context, bit_rate is 0, so set to 4M
            codec_ctx->bit_rate = estimate_bitrate(video_stream->codecpar->width, video_stream->codecpar->height, video_stream->r_frame_rate.num);  // 高清（1080p）较常用
            //std::cout << "estimate_bitrate: " << codec_ctx->bit_rate << std::endl;
            codec_ctx->bit_rate = 4*1024*1024;
        }
        codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P; // some fmt not support in h264
        codec_ctx->gop_size = 25.; // 25
        codec_ctx->level = 51;  // Level 5.1
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
        codec_ctx->thread_count = 0;
        codec_ctx->thread_type = FF_THREAD_FRAME;
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

    AVCodecContext* CFFmpegHelper::create_audio_encodec_context(const AVStream* const audio_stream, AVCodecID codec_id, bool open_codec)
    {
        std::cout<< "create_audio_encodec_context" << std::endl;
        if (!audio_stream)
        {
            std::cerr << term_color::red << "audio_stream is nullptr" << term_color::reset << std::endl;
            return nullptr;
        }
        const AVCodec *enc_a = avcodec_find_encoder(codec_id); //AV_CODEC_ID_AAC);
        if (!enc_a)
        {
            std::cerr << term_color::red << "Unsupported audio codec" << term_color::reset << AV_CODEC_ID_AAC << std::endl;
            return nullptr;
        }
        else
        {
            std::cout << "enc_a: " << enc_a->name << std::endl;
        }

        AVCodecParameters *in_par = audio_stream->codecpar;

        AVCodecContext* codec_ctx = avcodec_alloc_context3(enc_a);
        avcodec_parameters_to_context(codec_ctx, in_par);
        codec_ctx->codec_id = enc_a->id;
        codec_ctx->codec_type = enc_a->type;
        if(enc_a->id == AV_CODEC_ID_AAC)
        {
            codec_ctx->frame_size = 1024;
        }
        
        std::cout<< "audio_stream->time_base.den: " << audio_stream->time_base.den << std::endl;
        std::cout<< "audio_stream->time_base.num: " << audio_stream->time_base.num << std::endl;
        if (audio_stream->time_base.num > 0 && audio_stream->time_base.den > 0)
        {
            codec_ctx->time_base = audio_stream->time_base;
        }
        else
        {
            codec_ctx->time_base = AVRational{1, 48000}; // 默认25fps
        }
                    
        codec_ctx->time_base = AVRational{1, 48000}; // 默认25fps
        if(codec_ctx->sample_rate <= 0)
        {
            codec_ctx->sample_rate = 48000;
        }
        
        codec_ctx->sample_rate = 48000;

        if (open_codec)
        {
            avcodec_open2(codec_ctx, enc_a, nullptr);
        }
        return codec_ctx;
    }

    AVCodecContext* CFFmpegHelper::create_audio_decodec_context(const AVStream* const audio_stream, bool open_codec)
    {
        if (!audio_stream)
        {
            std::cerr << term_color::red << "audio_stream is nullptr" << term_color::reset << std::endl;
            return nullptr;
        }
        AVCodecParameters *in_par = audio_stream->codecpar;

        // 1. 创建 Opus 解码器上下文
        const AVCodec *dec_a = avcodec_find_decoder(in_par->codec_id);
        if (!dec_a)
        {
            std::cerr << term_color::red << "Unsupported audio codec" << term_color::reset << in_par->codec_id << std::endl;
            return nullptr;
        }
        else
        {
            std::cout << "dec_a: " << dec_a->name << std::endl;
        }

        AVCodecContext* dec_a_ctx = avcodec_alloc_context3(dec_a);
        avcodec_parameters_to_context(dec_a_ctx, in_par);
        if (audio_stream->time_base.num > 0 && audio_stream->time_base.den > 0)
        {
            dec_a_ctx->time_base = audio_stream->time_base;
        }
        else
        {
            dec_a_ctx->time_base = AVRational{1, 48000}; // 默认25fps
        }
                    
        dec_a_ctx->time_base = AVRational{1, 48000}; // 默认25fps

        if(dec_a_ctx->sample_rate <= 0)
        {
            dec_a_ctx->sample_rate = 48000;
        }
        if (open_codec)
        {
            avcodec_open2(dec_a_ctx, dec_a, nullptr);
        }
        return dec_a_ctx;
    }
}   
