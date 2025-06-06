#include "stream_transfer.h"
#include "picture_transfer.h"
#include "ffmepg_helper.h"
namespace stream_project
{
    class CAutoDestroyStreamTransfer
    {
    public:
        ~CAutoDestroyStreamTransfer()
        {
            if (fmt_ctx_ != NULL)
            {
                if(fmt_type_ == 0)
                {
                    avformat_close_input(&fmt_ctx_);        
                }
                else
                {
                    if (!(fmt_ctx_->oformat->flags & AVFMT_NOFILE)
                    && fmt_ctx_->pb) 
                    {
                        // rtsp pb is not null, but we can't closep
                        avio_closep(&fmt_ctx_->pb);
                    }
                    avformat_free_context(fmt_ctx_);
                }
                fmt_ctx_ = NULL;
            }
            if (codec_ctx_ != NULL)
            {
                avcodec_free_context(&codec_ctx_);
                codec_ctx_ = NULL;
            }
            if (packet_ != NULL)
            {
                av_packet_free(&packet_);
                packet_ = NULL;
            }
            if (frame_ != NULL)
            {
                av_frame_free(&frame_);
                frame_ = NULL;
            }
            if (sws_ctx_ != NULL)
            {
                sws_freeContext(sws_ctx_);
                sws_ctx_ = NULL;
            }
            if (swr_ctx_ != NULL)
            {
                swr_free(&swr_ctx_);
                swr_ctx_ = NULL;
            }
            if (audio_fifo_ != NULL)
            {
                av_audio_fifo_free(audio_fifo_);
                audio_fifo_ = NULL;
            }
        }
        // type 0: open iput, 1:alloc_output
        void set_fmt_ctx(AVFormatContext* ptr_in, int type)
        {
            fmt_ctx_ = ptr_in;
            fmt_type_ = type;
        }
        void set_codec_context(AVCodecContext* ptr_in)
        {
            codec_ctx_ = ptr_in;
        }
        void set_packet(AVPacket* ptr_in)
        {
            packet_ = ptr_in;
        }
        void set_frame(AVFrame* ptr_in)
        {
        frame_ = ptr_in;
        }
        void set_sws_ctx(SwsContext* ptr_in)
        {
            sws_ctx_ = ptr_in;
        }
        void set_swr_ctx(SwrContext* ptr_in)
        {
            swr_ctx_ = ptr_in;
        }
        void set_audio_fifo(AVAudioFifo* ptr_in)
        {
            audio_fifo_ = ptr_in;
        }
    private:
        AVFormatContext* fmt_ctx_ = NULL; 
        int fmt_type_ = 0; //0 input, 1 output 
        AVCodecContext* codec_ctx_ = NULL;
        AVPacket* packet_= NULL; 
        AVFrame* frame_= NULL; 
        SwsContext* sws_ctx_ = NULL;
        SwrContext* swr_ctx_ = NULL;
        AVAudioFifo* audio_fifo_ = NULL;
    };

    // stream_type: 0 video
    std::string transfer_video_pix_format(int pix_format, int stream_type)
    {
        if (stream_type == 0)
        {
            if (AV_PIX_FMT_YUV420P == pix_format)
            {
                return "AV_PIX_FMT_YUV420P";
            }
            else if (AV_PIX_FMT_YUV422P == pix_format)
            {
                return "AV_PIX_FMT_YUV422P";
            }
            else if (AV_PIX_FMT_RGB24 == pix_format)
            {
                return "AV_PIX_FMT_RGB24";
            }
            else
            {
                return "unknow pix format";
            }
        }
        else if(stream_type == 1)
        {
            
        }
        return "unknow pix format";
    }
    void printf_stream_info(const FileFormat& file_format)
    {
        std::cout<<"format name:" << file_format.iformat_name << std::endl;
        std::cout<<"nb_streams:" << file_format.nb_streams << std::endl;
        if (file_format.duration > 0)
        {
            std::cout<<"duration(s):" << file_format.duration/(1000*1000)<< std::endl;
        }
        else{
            std::cout<<"detect duration failed:"<< file_format.duration << "(" << AV_NOPTS_VALUE<< ")" <<std::endl;
        }

        if (file_format.bit_rate > 0)
        {
            std::cout<<"bit_rate:" << file_format.bit_rate<< std::endl;
        }
        else{
            std::cout<<"detect bit_rate failed:"<< file_format.bit_rate<< std::endl;
        }

        if (file_format.start_time >= 0)
        {
            std::cout<<"start_time:" << file_format.start_time<< std::endl;
        }
        else{
            std::cout<<"detect start_time failed:"<< file_format.start_time << "(" << AV_NOPTS_VALUE<< ")" <<std::endl;
        }
        

        if (file_format.video_stream != NULL)
        {
            std::cout<<"*****include video:"<< std::endl;
            const AVCodec *codec = avcodec_find_decoder(file_format.video_stream->codecpar->codec_id);
            if (codec)
            {
                std::cout << "video codec id:" << file_format.video_stream->codecpar->codec_id 
                        << ",name:" << codec->name
                        << ",longname:" << codec->long_name <<std::endl;
                std::cout << "video resolution:" << file_format.video_stream->codecpar->width  << "*" 
                    << file_format.video_stream->codecpar->height <<std::endl;

                ;
                std::cout << "pix format:" << file_format.video_stream->codecpar->format
                    << ",name:" << av_get_pix_fmt_name(static_cast<AVPixelFormat>(file_format.video_stream->codecpar->format)) 
                    << ",color_range: " << (int)file_format.video_stream->codecpar->color_range << std::endl; 
                // time base, Number of units between two frames  file_format.video_stream->time_base.den / r_frame_rate
                std::cout << "time_base:" << file_format.video_stream->time_base.num
                    << "/" << file_format.video_stream->time_base.den << std::endl;

                std::cout << "avg_frame_rate:" << file_format.video_stream->avg_frame_rate.num
                    << "/" << file_format.video_stream->avg_frame_rate.den << std::endl;

                std::cout << "r_frame_rate:" << file_format.video_stream->r_frame_rate.num
                    << "/" << file_format.video_stream->r_frame_rate.den << std::endl;

                // based on time_base. file_format.video_stream->duration* time_base = seconds
                std::cout << "duration:" << file_format.video_stream->duration << std::endl; 

                std::cout << "nb_frames:" << file_format.video_stream->nb_frames << std::endl;      
                std::cout << "start_time:" << file_format.video_stream->start_time << std::endl;      
                std::cout << "sample_aspect_ratio:" << file_format.video_stream->sample_aspect_ratio.num
                    << "/" << file_format.video_stream->sample_aspect_ratio.den << std::endl; 
                std::cout << "bit_rate:" << file_format.video_stream->codecpar->bit_rate << std::endl;
                //std::cout<< "frame number:" << file_format.video_stream->duration / file_format.video_stream->time_base.den << std::endl;
            }
            else
            {
                std::cout << "unknown video codec name" <<std::endl;
            }
        }
        if (file_format.audio_stream != NULL)
        {
            std::cout<<"*****include audio:" << std::endl;
            const AVCodec *codec = avcodec_find_decoder(file_format.audio_stream->codecpar->codec_id);
            std::cout << "audio codec id:" << file_format.audio_stream->codecpar->codec_id 
                    << ",name:" << codec->name
                    << ",longname:" << codec->long_name <<std::endl;
            std::cout << "audio sample_rate:" << file_format.audio_stream->codecpar->sample_rate <<std::endl;
        }
    }
    void printf_ffmepg_error(int error_code, const std::string& fun_name)
    {
        char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
        av_strerror(error_code, errbuf, sizeof(errbuf));

        char finalbuf[256] = {0};
        snprintf(finalbuf, sizeof(finalbuf), "%s faied, error code [%d-%s]", fun_name.c_str(), error_code, errbuf);

        std::cout << term_color::red << finalbuf << term_color::reset << std::endl;
    }
    int write_packet_to_file(const char* file_path, AVPacket* ptr_packet)
    {
        FILE* f = fopen(file_path, "wb");
        if (!f)
        {
            std::cout << term_color::red << "open file "<<file_path << " failed" << term_color::reset << std::endl;
            return -1;
        }
        fwrite(ptr_packet->data, 1, ptr_packet->size, f);
        fclose(f);
        return 1;
    }
    int write_frame_to_yuv(const char* file_path, AVFrame* ptr_frame)
    {
        std::cout << "write_frame_to_yuv " << file_path 
        << " width:"<< ptr_frame->width 
        << ",height:"<<ptr_frame->height
        << ",format:"<<ptr_frame->format
        << std::endl;

        FILE* f = fopen(file_path, "wb");
        if (!f)
        {
            std::cout << term_color::red << "open file "<<file_path << " failed" << term_color::reset << std::endl;
            return -1;
        }
        if (ptr_frame->format == AV_PIX_FMT_YUV420P
            || ptr_frame->format == AV_PIX_FMT_YUVJ420P)
            {
                // Y 
                for (int i = 0; i < ptr_frame->height; ++i)
                    fwrite(ptr_frame->data[0] + i * ptr_frame->linesize[0], 1, ptr_frame->width, f);

                //  U 
                for (int i = 0; i < ptr_frame->height / 2; ++i)
                    fwrite(ptr_frame->data[1] + i * ptr_frame->linesize[1], 1, ptr_frame->width / 2, f);

                // V
                for (int i = 0; i < ptr_frame->height / 2; ++i)
                    fwrite(ptr_frame->data[2] + i * ptr_frame->linesize[2], 1, ptr_frame->width / 2, f);
            }

        fclose(f);
        return 1;
    }
    int write_frame_to_rgb(const char* file_path, AVFrame* src_frame)
    {
        if(src_frame->format != AV_PIX_FMT_YUV420P)
        {
            std::cout << term_color::red << "src_frame->format is not YUV420P" << term_color::reset << std::endl;
            return -1;
        }

        int width = src_frame->width;
        int height = src_frame->height;

        CAutoDestroyStreamTransfer auto_destroy;
        // 1. 创建 RGB24 的目标帧
        AVFrame* rgb_frame = av_frame_alloc();
        auto_destroy.set_frame(rgb_frame);

        rgb_frame->format = AV_PIX_FMT_RGB24;
        rgb_frame->width = width;
        rgb_frame->height = height;
        av_frame_get_buffer(rgb_frame, 32);

        // 2. 使用 sws_scale 做格式转换（YUV → RGB）
        SwsContext* sws_ctx = sws_getContext(
            width, height, (AVPixelFormat)src_frame->format,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, NULL, NULL, NULL);

        sws_scale(sws_ctx,
                src_frame->data, src_frame->linesize,
                0, height,
                rgb_frame->data, rgb_frame->linesize);

        sws_freeContext(sws_ctx);

        FILE* f_w = fopen(file_path, "wb");
        if (!f_w)
        {
            std::cout << term_color::red << "open file "<<file_path << " failed" << term_color::reset << std::endl;
            return -1;
        }
        for (int y = 0; y < rgb_frame->height; y++) 
        {
            fwrite(rgb_frame->data[0] + y * rgb_frame->linesize[0], 1, rgb_frame->width * 3, f_w);
        }    
        fclose(f_w);
        return 1;
        }
    int write_frame_to_png(const char* file_path, AVFrame* src_frame)
    {
        std::cout << "write_frame_to_png " << file_path << std::endl;
        CPictureTransfer picture_transfer;
        if (src_frame->format == AV_PIX_FMT_YUV420P)
        {              
            std::string tmp_path = "../output/tmp.yuv";
            write_frame_to_yuv(tmp_path.c_str(), src_frame);

            picture_transfer.transfer_raw_to_picture(tmp_path, AV_PIX_FMT_YUV420P, src_frame->width, src_frame->height, file_path, AV_CODEC_ID_PNG);
            remove(tmp_path.c_str());
        }
        else if (src_frame->format == AV_PIX_FMT_RGB24)
        {
            std::string tmp_path = "../output/tmp.rgb";
            write_frame_to_rgb(tmp_path.c_str(), src_frame);

            picture_transfer.transfer_raw_to_picture(tmp_path, AV_PIX_FMT_RGB24, src_frame->width, src_frame->height, file_path, AV_CODEC_ID_PNG);
            remove(tmp_path.c_str());
        }
        else
        {
            std::cout << term_color::red << "src_frame->format is not YUV420P or RGB24" << term_color::reset << std::endl;
            return -1;
        }
        return 1;

        /*int width = src_frame->width;
        int height = src_frame->height;

        CAutoDestroyStreamTransfer auto_destroy;
        // 1. 创建 RGB24 的目标帧
        AVFrame* rgb_frame = av_frame_alloc();
        auto_destroy.set_frame(rgb_frame);

        rgb_frame->format = AV_PIX_FMT_RGB24;
        rgb_frame->width = width;
        rgb_frame->height = height;
        av_frame_get_buffer(rgb_frame, 32);

        // 2. 使用 sws_scale 做格式转换（YUV → RGB）
        SwsContext* sws_ctx = sws_getContext(
            width, height, (AVPixelFormat)src_frame->format,
            width, height, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, NULL, NULL, NULL);

        sws_scale(sws_ctx,
                src_frame->data, src_frame->linesize,
                0, height,
                rgb_frame->data, rgb_frame->linesize);

        sws_freeContext(sws_ctx);


        // 3. 查找 PNG 编码器并创建上下文
        const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_PNG);
        if (!codec) {
            fprintf(stderr, "找不到 PNG 编码器\n");
            return false;
        }

        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        auto_destroy.set_codec_context(codec_ctx);

        codec_ctx->bit_rate = 400000;
        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->time_base = (AVRational){1, 25};
        codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;

        if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
            fprintf(stderr, "open codec failed\n");
            return false;
        }

        // 4. 编码帧
        AVPacket* pkt = av_packet_alloc();
        auto_destroy.set_packet(pkt);
        if (!pkt) 
        {
            return -1;
        }

        if (avcodec_send_frame(codec_ctx, rgb_frame) < 0) {
            fprintf(stderr, "send frame failed\n");
            return false;
        }

        if (avcodec_receive_packet(codec_ctx, pkt) == 0) {
            // 5. 写入 PNG 文件
            FILE* f_w = fopen(file_path, "wb");
            if (!f_w)
            {
                std::cout<<"open file "<<file_path << " failed" << std::endl;
                return -1;
            }
            fwrite(pkt->data, 1, pkt->size, f_w);
            fclose(f_w);
            av_packet_unref(pkt);
        }
        return 1;*/
    } 


    int CStreamTransfer::init()
    {
        if (is_init_)
        {
            return 1;
        }
        //av_register_all();
        printf("compile version: %d.%d.%d\n", LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR, LIBAVFORMAT_VERSION_MICRO);
        printf("run version: %s\n", av_version_info());
        is_init_ = true;
        return 1;
    }

    int CStreamTransfer::analyze_file(const std::string& video_path, bool deep)
    {
        std::cout<<"******************start analyze_file******************"<< std::endl;
        CAutoDestroyStreamTransfer auto_destroy;
        init();

        FileFormat file_format;
        std::cout<<"file path:"<< video_path << std::endl;
        AVFormatContext *fmt_ctx = NULL;
        const AVCodec *codec = NULL;
        
        // open the url/file, didn't read to the buffer
        // [url] file or rtsp/http url
        // [AVInputFormat *] set format/by url suffix/detect by ffmepg/.
        // priority: AVInputFormat* > url > ffmepg detect
        // if we set AVInputFormat*, ffmepg will not detect it. 
        // if we didn't set, we set url suffix, fffmpeg will use url to try at first.
        // [AVDictionary **]: some opition, timeout, buffer size. 
        // default time out is Infinite waiting. default buffer size is 32KB
        int ret = avformat_open_input(&fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0) {
            printf_ffmepg_error(ret, "avformat_open_input");
            if (fmt_ctx)
            {
                avformat_free_context(fmt_ctx);
                fmt_ctx = NULL;
            }
            return -1;
        }
        auto_destroy.set_fmt_ctx(fmt_ctx, 0);
        // read some streas and detect stream information:codec_id, resolution, video, audio, timestamp, etc
        // [AVDictionary **] set thread number,probesize, analyzeduration,fpsprobesize.
        // av_dict_set and av_dict_free 
        ret = avformat_find_stream_info(fmt_ctx, NULL); 
        if (ret < 0) {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        // 4. check format
        if (!fmt_ctx->iformat) {
            file_format.iformat_name = "no format";
        } 
        else {
            if (strcmp(fmt_ctx->iformat->name, "h264") == 0
            || strcmp(fmt_ctx->iformat->name, "hevc") == 0
            || strcmp(fmt_ctx->iformat->name, "mpegvideo") == 0
            || strcmp(fmt_ctx->iformat->name, "vp8") == 0
            || strcmp(fmt_ctx->iformat->name, "vp9") == 0
            || strcmp(fmt_ctx->iformat->name, "aac") == 0)
            {
                file_format.iformat_name = "raw data " + std::string(fmt_ctx->iformat->name);
            }
            else
            {
                file_format.iformat_name = "format " + std::string(fmt_ctx->iformat->name);
            }
        }

        file_format.nb_streams = fmt_ctx->nb_streams;
        file_format.duration = fmt_ctx->duration;
        file_format.bit_rate = fmt_ctx->bit_rate;
        file_format.start_time = fmt_ctx->start_time;

        int video_stream_index = CFFmpegHelper::get_video_stream_index(fmt_ctx);
        if (video_stream_index < 0)
        {
            std::cout << term_color::red << "no video stream found" << term_color::reset << std::endl;
            return -1;
        }
        file_format.video_stream = fmt_ctx->streams[video_stream_index];
        int audio_stream_index = CFFmpegHelper::get_audio_stream_index(fmt_ctx);
        if (audio_stream_index >= 0)
        {
            file_format.audio_stream = fmt_ctx->streams[audio_stream_index];  
        }
        printf_stream_info(file_format);

        if(deep)
        {
            do{
                // read all frame
                if (video_stream_index < 0)
                {
                    break;
                }
                AVCodecContext* codec_ctx = CFFmpegHelper::create_video_decodec_context(fmt_ctx->streams[video_stream_index], true);
                if (!codec_ctx)
                {
                    std::cout << term_color::red << "create_video_decodec_context failed" << term_color::reset << std::endl;
                    break;
                }
                auto_destroy.set_codec_context(codec_ctx);

                AVPacket* pkt = av_packet_alloc();
                AVFrame* frame = av_frame_alloc();
                auto_destroy.set_packet(pkt);
                auto_destroy.set_frame(frame);

                int index = 0;
                int max_index = 100;
                while (av_read_frame(fmt_ctx, pkt) >= 0) 
                {
                    if (pkt->stream_index == video_stream_index) 
                    {
                        char finalbuf[256] = {0};
                        bool idr_mark = false;
                        if (CFFmpegHelper::is_keyframe(pkt))
                        {
                            idr_mark = true;
                        }
                        snprintf(finalbuf, sizeof(finalbuf), "AVPacket[%d] IDR[%d],size[%d],pts[%ld],dts[%ld],duration[%ld],pos[%ld]", 
                        index++,
                        idr_mark,
                        pkt->size,
                        pkt->pts,
                        pkt->dts,
                        pkt->duration,
                        pkt->pos
                        );
                        std::cout << finalbuf << std::endl;    
                        
                        
                        avcodec_send_packet(codec_ctx, pkt);
                        int ret = avcodec_receive_frame(codec_ctx, frame);
                        if (ret != 0)
                        {
                            std::cout << "avcodec_receive_frame failed:" << ret << std::endl;
                        }
                        else
                        {
                            
                            while (ret == 0) 
                            {
                                char pict_type = av_get_picture_type_char(frame->pict_type);
                                double pts_sec = frame->pts * av_q2d(fmt_ctx->streams[video_stream_index]->time_base);
                                std::cout << "  Frame: type=" << pict_type
                                        << ", pts=" << pts_sec
                                        << ", size=" << frame->width << "x" << frame->height
                                        << std::endl;

                                if (!CFFmpegHelper::is_keyframe(pkt)
                                && frame->pict_type == AV_PICTURE_TYPE_I)
                                {
                                    std::cout << "  non IDR I Frame" << std::endl;
                                }
                                av_frame_unref(frame);

                                ret = avcodec_receive_frame(codec_ctx, frame);
                            }
                        }
                    }
                    av_packet_unref(pkt);
                    if (index > max_index)
                    {
                        break;
                    }
                }

            }while(0);
        }
        std::cout<<"******************analyze_file finished******************"<<std::endl;
        return 1;
    }


    int CStreamTransfer::format_raw_to_mp4(const std::string& out, const std::string& video_path)
    {
        std::cout << "format_raw_to_mp4 " << video_path << std::endl;
        int ret = format_A_to_B2(out, video_path, "", "mp4");
        std::cout << "format_raw_to_mp4 finished" << video_path << std::endl;
        return ret;
    }

    int CStreamTransfer::format_raw_to_avi(const std::string& out, const std::string& video_path)
    {
        std::cout << "format_raw_to_avi " << video_path << std::endl;
        int ret = format_A_to_B2(out, video_path, "", "avi");
        std::cout << "format_raw_to_avi finished" << video_path << std::endl;
        return ret;
    }
    int CStreamTransfer::format_raw_to_yuv(const std::string& out, const std::string& video_path)
    {
        return 1;
    }
    int CStreamTransfer::format_yuv_to_rgb(const std::string& out, const std::string& video_path, int width_in, int height_in)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        int width = width_in;
        int height = height_in;
        enum AVPixelFormat pix_fmt = AV_PIX_FMT_YUV420P;

        FILE* in = fopen(video_path.c_str(), "rb");
        if (!in)
        {
            std::cout<<"read file failed"<<std::endl;
            return -1;
        }
        AVFrame* yuv_frame = av_frame_alloc();
        auto_destroy_input.set_frame(yuv_frame);

        yuv_frame->format = pix_fmt;
        yuv_frame->width = width;
        yuv_frame->height = height;
        av_frame_get_buffer(yuv_frame, 32); // 分配data

        // 读取 Y 分量
        for (int i = 0; i < height; i++)
            fread(yuv_frame->data[0] + i * yuv_frame->linesize[0], 1, width, in);

        // 读取 U 分量
        for (int i = 0; i < height / 2; i++)
            fread(yuv_frame->data[1] + i * yuv_frame->linesize[1], 1, width / 2, in);

        // 读取 V 分量
        for (int i = 0; i < height / 2; i++)
            fread(yuv_frame->data[2] + i * yuv_frame->linesize[2], 1, width / 2, in);
        fclose(in);

        struct SwsContext* sws_ctx = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P,
        width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

        int rgb_stride = width * 3;
        std::vector<uint8_t> rgb_buffer(rgb_stride * height);


        uint8_t* rgb_data[1] = { rgb_buffer.data() };
        int rgb_linesize[1] = { rgb_stride };
        sws_scale(sws_ctx,
                yuv_frame->data, yuv_frame->linesize, 0, height,
                rgb_data, rgb_linesize);

        sws_freeContext(sws_ctx);

     
        std::ofstream stream_out(out.c_str(), std::ios::binary);
         if (stream_out)
        {
            stream_out.write(reinterpret_cast<const char*>(rgb_buffer.data()), rgb_buffer.size());
        }
        else
        { 
            std::cout<<"write file failed"<<std::endl;   
        }        
        std::cout<<"******************format_yuv_to_rgb finished******************"<<std::endl;
        return 1;
    }

    // mp4 to h264
    void write_nalu(const uint8_t* data, int size, std::ofstream& outfile) {
        static const uint8_t start_code[4] = {0x00, 0x00, 0x00, 0x01};
        outfile.write((const char*)start_code, 4);
        outfile.write((const char*)data, size);
    }

    int CStreamTransfer::format_mp4_to_raw(const std::string& out, const std::string& video_path)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        const char* input_filename = video_path.c_str();
        const char* output_filename = out.c_str();

        AVFormatContext* fmt_ctx = NULL;
        int ret = avformat_open_input(&fmt_ctx, input_filename, NULL, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            if (fmt_ctx)
            {
                avformat_free_context(fmt_ctx);
                fmt_ctx = NULL;
            }
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(fmt_ctx, 0);

        ret = avformat_find_stream_info(fmt_ctx, NULL);
        if (ret < 0) {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        // 找到视频流索引
        int video_stream_index = CFFmpegHelper::get_video_stream_index(fmt_ctx);
        if (video_stream_index < 0)
        {
            std::cout << term_color::red << "no video stream found" << term_color::reset << std::endl;
            return -1;
        }

        // get sps and pps
        uint8_t* extradata = fmt_ctx->streams[video_stream_index]->codecpar->extradata;
        int extradata_size = fmt_ctx->streams[video_stream_index]->codecpar->extradata_size;

        std::cout << "extradata_size:" << extradata_size << std::endl;

        std::ofstream output(output_filename, std::ios::binary);
        if (!output.is_open()) 
        {
            std::cerr << "Failed to open output file" << std::endl;
            return -1;
        }

        AVPacket* packet = av_packet_alloc();
        auto_destroy_input.set_packet(packet);

        bool wrote_header = false;

        while (av_read_frame(fmt_ctx, packet) >= 0) 
        {
            if (packet->stream_index == video_stream_index) 
            {
                if (!wrote_header) 
                {
                    // 第一次，写extradata（SPS/PPS）
                    int offset = 6; // skip AVCC header (6 bytes固定头)
                    int num_sps = extradata[5] & 0x1F; // 5th字节是sps数量（通常是1）
                    for (int i = 0; i < num_sps; ++i) {
                        int sps_size = (extradata[offset] << 8) | extradata[offset + 1];
                        offset += 2;
                        write_nalu(extradata + offset, sps_size, output);
                        offset += sps_size;
                    }

                    int num_pps = extradata[offset];
                    offset += 1;
                    for (int i = 0; i < num_pps; ++i) {
                        int pps_size = (extradata[offset] << 8) | extradata[offset + 1];
                        offset += 2;
                        write_nalu(extradata + offset, pps_size, output);
                        offset += pps_size;
                    }                
                    wrote_header = true;
                }
                
                // packet本身是AVCC格式，要转成Annex-B格式
                int pos = 0;
                while (pos + 4 <= packet->size) 
                {
                    int nalu_size = AV_RB32(packet->data + pos);
                    pos += 4;
                    if (pos + nalu_size <= packet->size) 
                    {
                        write_nalu(packet->data + pos, nalu_size, output);
                        pos += nalu_size;
                    } 
                    else 
                    {
                        break;
                    }
                }
            }
            av_packet_unref(packet);
        }

        output.close();
        std::cout << "format_mp4_to_raw finshed " << output_filename << std::endl;
        return 1;
    }

    int CStreamTransfer::get_first_frame(const std::string& out, const std::string& video_path, int frame_type, int dst_codec)
    {
        std::cout << "get_first_frame " << video_path << std::endl;
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;
        AVFormatContext *fmt_ctx = NULL;
        int ret = avformat_open_input(&fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            if (fmt_ctx)
            {
                avformat_free_context(fmt_ctx);
                fmt_ctx = NULL;
            }
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(fmt_ctx, 0);

        // read some streas and detect stream information:codec_id, resolution, video, audio, timestamp, etc
        // [AVDictionary **] set thread number,probesize, analyzeduration,fpsprobesize.
        // av_dict_set and av_dict_free 
        ret = avformat_find_stream_info(fmt_ctx, NULL); 
        if (ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        int video_stream_index = CFFmpegHelper::get_video_stream_index(fmt_ctx);
        if (video_stream_index < 0)
        {
            std::cout << term_color::red << "no video stream found" << term_color::reset << std::endl;
            return -1;
        }

        AVCodecParameters* codec_parameter = fmt_ctx->streams[video_stream_index]->codecpar;

        AVCodecContext* codec_ctx = CFFmpegHelper::create_video_decodec_context(fmt_ctx->streams[video_stream_index], true);
        if (!codec_ctx)
        {
            std::cerr << term_color::red << "Failed to create video decodec context" << term_color::reset << std::endl;
            return -1;
        }
        auto_destroy_output.set_codec_context(codec_ctx);


        AVPacket* pkt = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();
        auto_destroy_input.set_packet(pkt);
        auto_destroy_output.set_frame(frame);

        int index = 0;
                        
        std::cout << term_color::yellow << "start decoder" << term_color::reset << std::endl;
        bool find = false;
        int find_frame_index = 3; // find the 5 frame
        int current_frame_index = 0;
        while (av_read_frame(fmt_ctx, pkt) >= 0) 
        {
            if (pkt->stream_index == video_stream_index) 
            {
                avcodec_send_packet(codec_ctx, pkt);
                while (avcodec_receive_frame(codec_ctx, frame) == 0) 
                {
                    //std::cout << term_color::yellow << "pict_type:" << frame->pict_type << term_color::reset << std::endl;
                    if (frame->pict_type == AV_PICTURE_TYPE_I)
                    {                    
                        ++current_frame_index;
                        if (current_frame_index >= find_frame_index)
                        {
                            find = true;
                            if(dst_codec == 0)
                            {
                                write_packet_to_file(out.c_str(), pkt);
                            }
                            else if (dst_codec == 1)
                            {
                                write_frame_to_yuv(out.c_str(), frame);
                            }
                            else if(dst_codec == 2)
                            {                            
                                write_frame_to_rgb(out.c_str(), frame);
                            }
                            else if(dst_codec == 3)
                            {
                                write_frame_to_png(out.c_str(), frame);
                            }

                        }
                        //std::cout << term_color::yellow << "find the frame, current_frame_index:" << current_frame_index << term_color::reset << std::endl;
                        //std::cout << "Y[0]: " << (int)frame->data[0][0] << std::endl;
                        //break;
                    }
                }
            }
            if(find)
            {
                break;
            }
        }
        std::cout << term_color::yellow << "******************get_first_frame_to_yuv finished******************" << term_color::reset << std::endl;
        return 1;
    }
    int CStreamTransfer::format_webm_to_mp4(const std::string& out, const std::string& video_path)
    {
        return format_A_to_B1(out, video_path, "webm", "mp4", AV_CODEC_ID_H264);
        std::cout << term_color::yellow << "format_webm_to_mp4 finished " << out << term_color::reset << std::endl;
        return 1;
    }

    int CStreamTransfer::change_resolution(const std::string& out, const std::string& video_path, int target_width, int target_height)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(input_fmt_ctx, 0);

        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);
        if (video_stream_index < 0)
        {
            std::cerr << "No video stream found\n";
            return -1;
        }

        // decoder
        AVCodecContext* decoder_ctx = CFFmpegHelper::create_video_decodec_context(input_fmt_ctx->streams[video_stream_index], true);
        if (!decoder_ctx)
        {
            std::cerr << "Failed to create video decodec context" << std::endl;
            return -1;
        }
        auto_destroy_input.set_codec_context(decoder_ctx);

        const AVCodec* encoder = avcodec_find_encoder(input_fmt_ctx->streams[video_stream_index]->codecpar->codec_id);
        if (!encoder) 
        {
            std::cerr << "Unsupported encoder" << std::endl;
            return -1;
        }
        // encoder
        AVCodecContext* encoder_ctx = CFFmpegHelper::create_video_encodec_context(input_fmt_ctx->streams[video_stream_index], 
                                                                                input_fmt_ctx->streams[video_stream_index]->codecpar->codec_id, 
                                                                                false);
        auto_destroy_output.set_codec_context(encoder_ctx);
        // width/height自己设置
        encoder_ctx->width = target_width;
        encoder_ctx->height = target_height;

        std::cout << "encoder_ctx->timebase(before open): " << encoder_ctx->time_base.num << "/" << encoder_ctx->time_base.den << std::endl;
        encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // it's very important, otherwise the output file will be invalid

        ret = avcodec_open2(encoder_ctx, encoder, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avcodec_open2");
            return -1;
        }
        std::cout << "encoder_ctx->timebase(after open): " << encoder_ctx->time_base.num << "/" << encoder_ctx->time_base.den << std::endl;
        
        // extradata要手动malloc+copy, resolution changed, we can't copy decoder_ctx->extradata to encoder_ctx->extradata
        /*if (decoder_ctx->extradata && decoder_ctx->extradata_size > 0) {
            encoder_ctx->extradata = (uint8_t*)av_mallocz(decoder_ctx->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
            memcpy(encoder_ctx->extradata, decoder_ctx->extradata, decoder_ctx->extradata_size);
            encoder_ctx->extradata_size = decoder_ctx->extradata_size;
        }*/
        
        AVFormatContext* output_fmt_ctx = NULL;
        std::string input_format_name = CFFmpegHelper::get_input_format_name(input_fmt_ctx);
        avformat_alloc_output_context2(&output_fmt_ctx, NULL, input_format_name.c_str(), out.c_str());
        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        AVStream* out_stream = CFFmpegHelper::create_new_video_stream(encoder_ctx, output_fmt_ctx, encoder_ctx->codec_id);
        if (!out_stream)
        {
            std::cerr << term_color::red << "Failed to create new video stream" << term_color::reset << std::endl;
            return -1;
        }

        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << "avio_open failed" << out << std::endl;
                return -1;
            }
        }

       
        ret = avformat_write_header(output_fmt_ctx, NULL);    
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }

        AVPacket* packet = av_packet_alloc();
        auto_destroy_input.set_packet(packet);

        AVFrame* frame = av_frame_alloc();
        auto_destroy_input.set_frame(frame);

        AVPacket* output_packet = av_packet_alloc();
        auto_destroy_output.set_packet(output_packet);

        AVFrame* output_frame = av_frame_alloc();
        auto_destroy_output.set_frame(output_frame);

        output_frame->format = encoder_ctx->pix_fmt;
        output_frame->width = encoder_ctx->width;
        output_frame->height = encoder_ctx->height;
        ret = av_frame_get_buffer(output_frame, 32);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "av_frame_get_buffer");
            return -1;
        }

        struct SwsContext* sws_ctx = sws_getContext(
            decoder_ctx->width, decoder_ctx->height, decoder_ctx->pix_fmt,
            encoder_ctx->width, encoder_ctx->height, encoder_ctx->pix_fmt,
            SWS_BILINEAR, NULL, NULL, NULL);
        auto_destroy_output.set_sws_ctx(sws_ctx);

        while(av_read_frame(input_fmt_ctx, packet) >= 0)
        {
            if(packet->stream_index == video_stream_index)
            {
                avcodec_send_packet(decoder_ctx, packet);
                while(avcodec_receive_frame(decoder_ctx, frame) == 0)
                {
                    sws_scale(sws_ctx,
                        frame->data, frame->linesize, 0, frame->height,
                        output_frame->data, output_frame->linesize);
                        
                    output_frame->pts = frame->pts; // 设置pts based on new timebase??

                    avcodec_send_frame(encoder_ctx, output_frame);

                    while(avcodec_receive_packet(encoder_ctx, output_packet) == 0)
                    {
                        output_packet->stream_index = out_stream->index;
                        av_interleaved_write_frame(output_fmt_ctx, output_packet);
                        av_packet_unref(output_packet);
                    }
                    // we can't unref output_frame, because it will be reused
                    // output_frame is alloc by av_frame_get_buffer
                    // frame buffer we can use unref, because it's alloc by avcodec_receive_frame
                    //av_frame_unref(output_frame);
                }
                av_frame_unref(frame);
            }
            av_packet_unref(packet);
        }
        avcodec_send_frame(encoder_ctx, nullptr);
        while(avcodec_receive_packet(encoder_ctx, output_packet) == 0)
        {
            output_packet->stream_index = out_stream->index;
            av_interleaved_write_frame(output_fmt_ctx, output_packet);
            av_packet_unref(output_packet);
        }

        ret = av_write_trailer(output_fmt_ctx);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "av_write_trailer");
            return -1;
        }

        std::cout << "change_resolution finished " << out << std::endl;
        return 1;
    }
    int CStreamTransfer::change_fps(const std::string& out, const std::string& video_path)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(input_fmt_ctx, 0);

        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);
        if (video_stream_index < 0)
        {
            std::cerr << "No video stream found\n";
            return -1;
        }

        AVCodecParameters* codec_parameter = input_fmt_ctx->streams[video_stream_index]->codecpar;

        // new file
        AVFormatContext* output_fmt_ctx = NULL;
        std::string input_format_name = CFFmpegHelper::get_input_format_name(input_fmt_ctx);
        std::cout << "input_format_name: " << input_format_name << std::endl;
        avformat_alloc_output_context2(&output_fmt_ctx, NULL, input_format_name.c_str(), out.c_str());
        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        AVStream* out_stream = avformat_new_stream(output_fmt_ctx, nullptr);
        avcodec_parameters_copy(out_stream->codecpar, input_fmt_ctx->streams[video_stream_index]->codecpar);
        out_stream->time_base = input_fmt_ctx->streams[video_stream_index]->time_base;

        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << term_color::red << "avio_open failed" << term_color::reset << out << std::endl;
                return -1;
            }
        }

        ret = avformat_write_header(output_fmt_ctx, nullptr);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }

        AVPacket* packet = av_packet_alloc();
        auto_destroy_input.set_packet(packet);

        int frame_idx = 0;
        while (av_read_frame(input_fmt_ctx, packet) >= 0) {
            if (packet->stream_index == video_stream_index) {
                if (CFFmpegHelper::is_keyframe(packet))
                {
                    av_interleaved_write_frame(output_fmt_ctx, packet);
                }
                else 
                {
                    if (frame_idx % 2 == 0) { // 只保留偶数帧
                        av_interleaved_write_frame(output_fmt_ctx, packet);
                    }
                }
                frame_idx++;
            }
            av_packet_unref(packet);
        }
        av_write_trailer(output_fmt_ctx);

        std::cout << "change_fps finished " << out << std::endl;
        return 1;
    }

    int CStreamTransfer::format_A_to_B1(const std::string& out, const std::string& video_path, const std::string& src_fmt, const std::string& dst_fmt, AVCodecID dst_codec_id)
    {
        init();
        bool enable_b_frame = false;
        bool enable_audio = false;
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;
        CAutoDestroyStreamTransfer auto_destroy_audio_input;
        CAutoDestroyStreamTransfer auto_destroy_audio_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = 0;
        if (!src_fmt.empty())
        {
            const AVInputFormat* input_fmt = av_find_input_format(src_fmt.c_str());
            ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), input_fmt, NULL);
        }
        else
        {
            ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
            
        }
        if (ret < 0 || !input_fmt_ctx)
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            return -1;

        }
        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            return -1;
        }

        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);
        int audio_stream_index = CFFmpegHelper::get_audio_stream_index(input_fmt_ctx);
        if (audio_stream_index < 0)
        {
            enable_audio = false;
        }

        if (video_stream_index < 0) 
        {
            std::cerr << term_color::red << "No video stream found\n" << term_color::reset;
            return -1;
        }
        auto video_stream = input_fmt_ctx->streams[video_stream_index];
        // --- 创建解码器 ---
        AVCodecContext* video_dec_ctx = CFFmpegHelper::create_video_decodec_context(video_stream, true);
        if (!video_dec_ctx)
        {
            std::cerr << term_color::red << "Failed to create video decodec context" << term_color::reset << std::endl;
            return -1;
        }
        auto_destroy_input.set_codec_context(video_dec_ctx);


        // --- 创建输出上下文 ---
        AVFormatContext* output_fmt_ctx = NULL;
        if (!dst_fmt.empty())
        {
            ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, dst_fmt.c_str(), out.c_str());
        }
        else
        {
            // if dst_fmt is empty, use out format
            ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, out.c_str());
        }

        if (!output_fmt_ctx || ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_alloc_output_context2");
            return -1;
        }

        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        // 视频编码器（H.264）
        const AVCodec* video_encoder = avcodec_find_encoder(dst_codec_id);
        if (!video_encoder) 
        {
            std::cerr << term_color::red << "Unsupported video codec" << term_color::reset << dst_codec_id << std::endl;
            return -1;
        }
        AVCodecContext* video_enc_ctx = CFFmpegHelper::create_video_encodec_context(video_stream, dst_codec_id, false);
        if (!video_enc_ctx)
        {
            std::cerr << term_color::red << "Failed to create video encodec context" << term_color::reset << std::endl;
            return -1;
        }
        auto_destroy_output.set_codec_context(video_enc_ctx);

        AVStream* out_video_stream = CFFmpegHelper::create_new_video_stream(video_enc_ctx, output_fmt_ctx, dst_codec_id);
        if (!out_video_stream)
        {
            std::cerr << term_color::red << "Failed to create new video stream" << term_color::reset << std::endl;
            return -1;
        }

        
        if(!enable_b_frame)
        {
            video_enc_ctx->max_b_frames = 0;
        }
     
        avcodec_open2(video_enc_ctx, video_encoder, NULL);
      
        AVCodecContext *dec_a_ctx = NULL;
        AVCodecContext *enc_a_ctx = NULL;
        struct SwrContext* swr_ctx_audio = NULL;
        AVStream *out_a_stream = NULL;
        // audio stream
        if (enable_audio && audio_stream_index >= 0)
        {               
            AVStream *in_a_stream = input_fmt_ctx->streams[audio_stream_index];
            dec_a_ctx = CFFmpegHelper::create_audio_decodec_context(in_a_stream, true);
            if (!dec_a_ctx)
            {
                std::cerr << term_color::red << "Failed to create audio decodec context" << term_color::reset << std::endl;
                return -1;
            }
            auto_destroy_audio_input.set_codec_context(dec_a_ctx);

            enc_a_ctx = CFFmpegHelper::create_audio_encodec_context(in_a_stream, AV_CODEC_ID_AAC, false);
            if (!enc_a_ctx)
            {
                std::cerr << term_color::red << "Failed to create audio encodec context" << term_color::reset << std::endl;
                return -1;
            }
            out_a_stream = CFFmpegHelper::create_new_audio_stream(enc_a_ctx, output_fmt_ctx, AV_CODEC_ID_AAC);
            if (!out_a_stream)
            {
                std::cerr << term_color::red << "Failed to create new audio stream" << term_color::reset << std::endl;
                return -1;
            }

            auto_destroy_audio_output.set_codec_context(enc_a_ctx);
            const AVCodec *enc_a = avcodec_find_encoder(AV_CODEC_ID_AAC);
            ret = avcodec_open2(enc_a_ctx, enc_a, nullptr);
            if (ret < 0)
            {
                printf_ffmepg_error(ret, "avcodec_open2");
                return -1;
            }
            else
            {
                std::cout<< "avcodec_open2 audio success" << std::endl;
                std::cout << "enc_a_ctx->frame_size: " << enc_a_ctx->frame_size << std::endl;
            }

            AVCodecParameters *in_par = in_a_stream->codecpar;
            // 4. 创建重采样上下文：从输入的 Opus 配置 转到 AAC 要求的配置
            // 输出通道布局
            AVChannelLayout out_ch_layout;
            av_channel_layout_default(&out_ch_layout, enc_a_ctx->ch_layout.nb_channels);

            // 输入通道布局
            AVChannelLayout in_ch_layout;
            av_channel_layout_default(&in_ch_layout, dec_a_ctx->ch_layout.nb_channels);  // 或者从解码器 AVCodecContext 拿

            // 设置 swr context
            ret = swr_alloc_set_opts2(
                &swr_ctx_audio,
                &out_ch_layout, enc_a_ctx->sample_fmt, enc_a_ctx->sample_rate,
                &in_ch_layout,  dec_a_ctx->sample_fmt, dec_a_ctx->sample_rate,
                0, nullptr);
            if (ret < 0)
            {
                printf_ffmepg_error(ret, "swr_alloc_set_opts2");
                return -1;
            }
            if (swr_init(swr_ctx_audio) < 0) 
            {
                std::cerr << term_color::red << "swr_init failed" << term_color::reset << std::endl;
                return -1;
            }
            else
            {
                std::cout<< "swr_init success" << std::endl;
            }
            auto_destroy_audio_output.set_swr_ctx(swr_ctx_audio);
            std::cout<< "include audio,swr_ctx_audio: "<< swr_ctx_audio << std::endl;
        }

        // 设置输出文件
        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << term_color::red << "avio_open failed" << term_color::reset << out << std::endl;
                return -1;
            }
        }
        
        // out_video_stream modify to 1/16000, before is 1/1000
        // video_enc_ctx time base is 1/1000, so we need to rescale the time base
        ret = avformat_write_header(output_fmt_ctx, NULL); 
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }


        AVPacket* input_packet = av_packet_alloc();
        auto_destroy_input.set_packet(input_packet);

        AVPacket* output_packet = av_packet_alloc();
        auto_destroy_output.set_packet(output_packet);

        AVFrame* input_frame = av_frame_alloc();
        auto_destroy_input.set_frame(input_frame);

        AVFrame* output_frame = av_frame_alloc();
        auto_destroy_output.set_frame(output_frame);

        struct SwsContext* sws_ctx = sws_getContext(
            video_dec_ctx->width, video_dec_ctx->height, video_dec_ctx->pix_fmt,
            video_enc_ctx->width, video_enc_ctx->height, video_enc_ctx->pix_fmt,
            SWS_BICUBIC, NULL, NULL, NULL
        );

        auto_destroy_output.set_sws_ctx(sws_ctx);

        int video_frame_index = 0;
        int audio_frame_index = 0;
        AVAudioFifo* audio_fifo = nullptr;
        if (enable_audio)
        {
            audio_fifo = av_audio_fifo_alloc(enc_a_ctx->sample_fmt, enc_a_ctx->ch_layout.nb_channels, 1024 * 10);
            auto_destroy_audio_output.set_audio_fifo(audio_fifo);
            if (!audio_fifo) 
            {
                std::cerr << term_color::red << "Failed to allocate AVAudioFifo" << term_color::reset << std::endl;
                return -1;
            }
        }

        AVRational enc_video_tb  = video_enc_ctx->time_base;      // {1,1000}
        AVRational out_video_tb  = out_video_stream->time_base;     // {1,16000}
        int transfer_frame_count = 0;
        while (av_read_frame(input_fmt_ctx, input_packet) >= 0) 
        {
            transfer_frame_count++;
            if (transfer_frame_count > 500)
            {
                break;
            }
            if (input_packet->stream_index == video_stream_index) 
            {
                ret = avcodec_send_packet(video_dec_ctx, input_packet);
                if (ret < 0) 
                {
                    printf_ffmepg_error(ret, "avcodec_send_packet");
                    // 根据情况判断是不是 continue 还是直接 break
                    continue;
                }

                while (avcodec_receive_frame(video_dec_ctx, input_frame) == 0) 
                {
                    // 转换像素格式
                    // set dst frame format, width, height for encoder
                    output_frame->format = video_enc_ctx->pix_fmt;
                    output_frame->width = video_enc_ctx->width;
                    output_frame->height = video_enc_ctx->height;
                    av_frame_get_buffer(output_frame, 32);
                    sws_scale(sws_ctx,
                            input_frame->data, input_frame->linesize, 0, input_frame->height,
                            output_frame->data, output_frame->linesize);

                    //output_frame->pts = video_frame_index++;
                    output_frame->pts = av_rescale_q(video_frame_index,
                                 (AVRational){1, video_enc_ctx->framerate.num},
                                 video_enc_ctx->time_base);
                    ++video_frame_index;
                    // send frame to encoder,start encoding
                    avcodec_send_frame(video_enc_ctx, output_frame);
                    
                    // receive packet from encoder,end encoding
                    
                    while (avcodec_receive_packet(video_enc_ctx, output_packet) == 0) 
                    {
                       //std::cout << "before av_packet_rescale_ts output_packet->pts: " << output_packet->pts << std::endl;
                        av_packet_rescale_ts(output_packet,
                         enc_video_tb,
                         out_video_tb);
                        output_packet->stream_index = out_video_stream->index;
                       
                        av_interleaved_write_frame(output_fmt_ctx, output_packet); 
                        av_packet_unref(output_packet);
                    }
                    av_frame_unref(output_frame); // 清除旧的
                    av_frame_unref(input_frame);
                }
            }
            else if (input_packet->stream_index == audio_stream_index && enable_audio)
            {
                ret = avcodec_send_packet(dec_a_ctx, input_packet);
                if (ret < 0)
                {
                    printf_ffmepg_error(ret, "avcodec_send_packet");
                    continue;
                }
                
                while (avcodec_receive_frame(dec_a_ctx, input_frame) >= 0)
                {
                    input_frame->pts = av_rescale_q(input_packet->pts, 
                        input_fmt_ctx->streams[audio_stream_index]->time_base, 
                        dec_a_ctx->time_base);

                    uint8_t **converted_data = nullptr;
                    int converted_samples = av_rescale_rnd(
                        swr_get_delay(swr_ctx_audio, input_frame->sample_rate) + input_frame->nb_samples,
                        enc_a_ctx->sample_rate, input_frame->sample_rate, AV_ROUND_UP);

                    av_samples_alloc_array_and_samples(&converted_data, nullptr,
                        enc_a_ctx->ch_layout.nb_channels, converted_samples, enc_a_ctx->sample_fmt, 0);

                    av_channel_layout_copy(&output_frame->ch_layout, &enc_a_ctx->ch_layout);    

                    ret = swr_convert(swr_ctx_audio,
                                converted_data, converted_samples,
                                 (const uint8_t **)input_frame->data, input_frame->nb_samples);


                    if (ret < 0) 
                    {
                        printf_ffmepg_error(ret, "swr_convert");
                        av_freep(&converted_data[0]);
                        av_freep(&converted_data);
                        continue;
                    }
                    av_audio_fifo_write(audio_fifo, (void **)converted_data, ret);
                    av_freep(&converted_data[0]);
                    av_freep(&converted_data);


                    // 如果 FIFO 中有足够样本数，开始编码
                    while (av_audio_fifo_size(audio_fifo) >= enc_a_ctx->frame_size) 
                    {
                        output_frame->nb_samples = enc_a_ctx->frame_size;
                        av_channel_layout_copy(&output_frame->ch_layout, &enc_a_ctx->ch_layout);
                        output_frame->format = enc_a_ctx->sample_fmt;
                        output_frame->sample_rate = enc_a_ctx->sample_rate;

                        av_frame_get_buffer(output_frame, 0);

                        av_audio_fifo_read(audio_fifo, (void **)output_frame->data, enc_a_ctx->frame_size);

                        output_frame->pts = audio_frame_index;
                        audio_frame_index += output_frame->nb_samples;

                        avcodec_send_frame(enc_a_ctx, output_frame);
                        while (avcodec_receive_packet(enc_a_ctx, output_packet) == 0)
                        {
                            output_packet->stream_index = out_a_stream->index;
                            av_packet_rescale_ts(output_packet, enc_a_ctx->time_base, out_a_stream->time_base);
                            av_interleaved_write_frame(output_fmt_ctx, output_packet);
                            av_packet_unref(output_packet);
                        }
                    }
                    av_frame_unref(output_frame); // 清除旧的
                    av_frame_unref(input_frame);
                }
            }
            av_packet_unref(input_packet);
        }

        // decoder flush
        avcodec_send_packet(video_dec_ctx, NULL);
        while (avcodec_receive_frame(video_dec_ctx, input_frame) == 0) 
        {
            output_frame->format = video_enc_ctx->pix_fmt;
            output_frame->width = video_enc_ctx->width;
            output_frame->height = video_enc_ctx->height;
            av_frame_get_buffer(output_frame, 32);
            sws_scale(sws_ctx,
                        input_frame->data, input_frame->linesize, 0, input_frame->height,
                        output_frame->data, output_frame->linesize);

            output_frame->pts = av_rescale_q(video_frame_index,
                                            (AVRational){1, video_enc_ctx->framerate.num},
                                            video_enc_ctx->time_base);
            video_frame_index++;

            avcodec_send_frame(video_enc_ctx, output_frame);

            while (avcodec_receive_packet(video_enc_ctx, output_packet) == 0) 
            {
                output_packet->stream_index = out_video_stream->index;

                av_packet_rescale_ts(output_packet, enc_video_tb, out_video_tb);

                av_interleaved_write_frame(output_fmt_ctx, output_packet); 
                av_packet_unref(output_packet);
            }
            av_frame_unref(output_frame);
            av_frame_unref(input_frame);
        }

        // encoder flush
        avcodec_send_frame(video_enc_ctx, NULL);
        while (avcodec_receive_packet(video_enc_ctx, output_packet) == 0) 
        {
            output_packet->stream_index = out_video_stream->index;

            av_packet_rescale_ts(output_packet, enc_video_tb, out_video_tb);

            av_interleaved_write_frame(output_fmt_ctx, output_packet);
            av_packet_unref(output_packet);
        }

        // audio encoder flush
        if (enable_audio && audio_stream_index >= 0)
        {
            avcodec_send_frame(enc_a_ctx, nullptr);

            // 然后反复读取剩余的 packet
            av_packet_unref(output_packet);
            while (avcodec_receive_packet(enc_a_ctx, output_packet) == 0) 
            {
                // 记得做时间基转换并写出
                av_packet_rescale_ts(output_packet,
                                    enc_a_ctx->time_base,
                                    out_a_stream->time_base);
                output_packet->stream_index = out_a_stream->index;
                av_interleaved_write_frame(output_fmt_ctx, output_packet);
                av_packet_unref(output_packet);
            }
        }
        av_write_trailer(output_fmt_ctx);

        std::cout << "format_A_to_B1 finished " << out << std::endl;
        return 1;
    }

    int CStreamTransfer::format_A_to_B2(const std::string& out, const std::string& video_path, const std::string& src_fmt, const std::string& dst_fmt)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = 0;
        if (!src_fmt.empty())
        {
            const AVInputFormat* input_fmt = av_find_input_format(src_fmt.c_str());
            ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), input_fmt, NULL);
        }
        else
        {
            ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
            
        }
        if (ret < 0 || !input_fmt_ctx)
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            std::cout << term_color::red << "avformat_open_input failed" << term_color::reset << video_path << std::endl;
            return -1;

        }
        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            std::cout << term_color::red << "avformat_find_stream_info failed" << term_color::reset << video_path << std::endl;
            return -1;
        }

        // --- 创建输出上下文 ---
        AVFormatContext* output_fmt_ctx = NULL;
        if (!dst_fmt.empty())
        {
            ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, dst_fmt.c_str(), out.c_str());
        }
        else
        {
            // if dst_fmt is empty, use out format
            ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, out.c_str());
        }

        if (!output_fmt_ctx || ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_alloc_output_context2");
            return -1;
        }
        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        // 复制输入流到输出
        AVStream* out_stream = CFFmpegHelper::clone_new_video_stream(input_fmt_ctx, output_fmt_ctx);
        if (!out_stream)
        {
            std::cerr << term_color::red << "Failed to clone new video stream" << term_color::reset << std::endl;
            return -1;
        }
        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);
        if (video_stream_index == -1)
        {
            std::cerr << term_color::red << "No video stream found" << term_color::reset << std::endl;
            return -1;
        }
        
        // 设置输出文件
        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << term_color::red << "avio_open failed" << term_color::reset  << out << std::endl;
                return -1;
            }
        }
        
        // output_fmt_ctx modify the time base even i set to 1/90000
        // 1/1000 is not a good choice
        ret = avformat_write_header(output_fmt_ctx, NULL); 
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }

        std::cout << term_color::yellow << "output_fmt_ctx->time_base:" << output_fmt_ctx->streams[0]->time_base.num << "/" << output_fmt_ctx->streams[0]->time_base.den << term_color::reset << std::endl;

        AVPacket* input_packet = av_packet_alloc();
        auto_destroy_input.set_packet(input_packet);

        int frame_index = 0;
        while (av_read_frame(input_fmt_ctx, input_packet) >= 0) 
        {
            if (input_packet->stream_index == video_stream_index) 
            {
                AVStream* in_stream = input_fmt_ctx->streams[input_packet->stream_index];
                AVStream* out_stream = output_fmt_ctx->streams[0];

                // re-calculate PTS/DTS
                input_packet->pts = av_rescale_q_rnd(input_packet->pts, in_stream->time_base, out_stream->time_base, 
                                        static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                input_packet->dts = av_rescale_q_rnd(input_packet->dts, in_stream->time_base, out_stream->time_base, 
                                        static_cast<AVRounding>(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
                input_packet->duration = av_rescale_q(input_packet->duration, in_stream->time_base, out_stream->time_base);
                input_packet->pos = -1;

                if (input_packet->pts == AV_NOPTS_VALUE)
                {                  
                    int frame_duration = output_fmt_ctx->streams[0]->time_base.den / 25;   // 每帧 = 3600 时间单位
                    input_packet->pts = (frame_index * frame_duration);
                    input_packet->dts = input_packet->pts;
                    input_packet->duration = frame_duration;
                    input_packet->pos = -1;
                }
                else
                {
                    //std::cout << "input_packet->pts2:" << input_packet->pts << std::endl;
                }
                ++frame_index;

                // write packet, av_interleaved_write_frame will use the new format context
                // the packet is the same as the input packet
                av_interleaved_write_frame(output_fmt_ctx, input_packet); 
            }
            av_packet_unref(input_packet);
        }

        av_write_trailer(output_fmt_ctx);

        std::cout << "format_A_to_B2 finished " << out << std::endl;
        return 1;
    }

    int CStreamTransfer::cut_video(const std::string& out, const std::string& video_path, int start_time, int end_time)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = 0;
        ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0 || !input_fmt_ctx)
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            std::cout << term_color::red << "avformat_open_input failed" << term_color::reset << video_path << std::endl;
            return -1;

        }
        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            std::cout << term_color::red << "avformat_find_stream_info failed" << term_color::reset << video_path << std::endl;
            return -1;
        }

        // --- 创建输出上下文 ---
        AVFormatContext* output_fmt_ctx = NULL;
        ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, out.c_str());

        if (!output_fmt_ctx || ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_alloc_output_context2");
            return -1;
        }

        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        // 复制输入流到输出
        AVStream* out_stream = CFFmpegHelper::clone_new_video_stream(input_fmt_ctx, output_fmt_ctx);
        if (!out_stream)
        {
            std::cerr << term_color::red << "Failed to clone new video stream" << term_color::reset << std::endl;
            return -1;
        }
        
        // 设置输出文件
        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << term_color::red << "avio_open failed" << term_color::reset  << out << std::endl;
                return -1;
            }
        }
        
        // output_fmt_ctx modify the time base even i set to 1/90000
        // 1/1000 is not a good choice
        ret = avformat_write_header(output_fmt_ctx, NULL); 
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }

        std::cout << term_color::yellow << "output_fmt_ctx->time_base:" << output_fmt_ctx->streams[0]->time_base.num << "/" << output_fmt_ctx->streams[0]->time_base.den << term_color::reset << std::endl;

        AVPacket* input_packet = av_packet_alloc();
        auto_destroy_input.set_packet(input_packet);

        int frame_index = 0;
        int new_frame_index = 0;
        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);
        int start_time_in_time_base = start_time * output_fmt_ctx->streams[video_stream_index]->time_base.den;
        int end_time_in_time_base = end_time * output_fmt_ctx->streams[video_stream_index]->time_base.den;
        std::cout << "start_time_in_time_base:" << start_time_in_time_base << std::endl;
        std::cout << "end_time_in_time_base:" << end_time_in_time_base << std::endl;

        std::cout << "input time base:" << input_fmt_ctx->streams[video_stream_index]->time_base.num << "/" << input_fmt_ctx->streams[video_stream_index]->time_base.den << std::endl;

        bool is_start = false;
        while (av_read_frame(input_fmt_ctx, input_packet) >= 0) 
        {
            if (input_packet->stream_index == video_stream_index) 
            {                    
                int frame_duration = output_fmt_ctx->streams[video_stream_index]->time_base.den / 25;   // 每帧 = 3600 时间单位
                input_packet->pts = (frame_index * frame_duration);
                input_packet->dts = input_packet->pts;
                input_packet->duration = frame_duration;
                input_packet->pos = -1;

                if (input_packet->pts >= start_time_in_time_base && input_packet->pts <= end_time_in_time_base)
                {
                    //std::cout << "pts_time in time base:" << input_packet->pts << std::endl;
                    if (!is_start)
                    {
                        if(CFFmpegHelper::is_keyframe(input_packet))
                        {
                            is_start = true;
                            input_packet->pts = new_frame_index * frame_duration;
                            input_packet->dts = input_packet->pts;
                            input_packet->duration = frame_duration;
                            input_packet->pos = -1;
                            av_interleaved_write_frame(output_fmt_ctx, input_packet); 
                            new_frame_index++;
                        }
                    }
                    else
                    {
                        input_packet->pts = new_frame_index * frame_duration;
                        input_packet->dts = input_packet->pts;
                        input_packet->duration = frame_duration;
                        input_packet->pos = -1;
                        av_interleaved_write_frame(output_fmt_ctx, input_packet); 
                        new_frame_index++;
                    }
                }
                ++frame_index;
            }
            av_packet_unref(input_packet);
        }

        av_write_trailer(output_fmt_ctx);

        std::cout << "cut_video finished " << out << std::endl;
        return 1;
    }

    int CStreamTransfer::remove_audio(const std::string& out, const std::string& video_path)
    {
        init();
        CAutoDestroyStreamTransfer auto_destroy_input;
        CAutoDestroyStreamTransfer auto_destroy_output;

        AVFormatContext* input_fmt_ctx = NULL;
        int ret = 0;
        ret = avformat_open_input(&input_fmt_ctx, video_path.c_str(), NULL, NULL);
        if (ret < 0 || !input_fmt_ctx)
        {
            printf_ffmepg_error(ret, "avformat_open_input");
            std::cout << term_color::red << "avformat_open_input failed" << term_color::reset << video_path << std::endl;
            return -1;

        }
        ret = avformat_find_stream_info(input_fmt_ctx, NULL);
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_find_stream_info");
            std::cout << term_color::red << "avformat_find_stream_info failed" << term_color::reset << video_path << std::endl;
            return -1;
        }

        // --- 创建输出上下文 ---
        AVFormatContext* output_fmt_ctx = NULL;
        ret = avformat_alloc_output_context2(&output_fmt_ctx, NULL, NULL, out.c_str());

        if (!output_fmt_ctx || ret < 0) 
        {
            printf_ffmepg_error(ret, "avformat_alloc_output_context2");
            return -1;
        }

        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        // 复制输入流到输出
        AVStream* out_stream = CFFmpegHelper::clone_new_video_stream(input_fmt_ctx, output_fmt_ctx);
        if (!out_stream)
        {
            std::cerr << term_color::red << "Failed to clone new video stream" << term_color::reset << std::endl;
            return -1;
        }
        
        // 设置输出文件
        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                printf_ffmepg_error(ret, "avio_open");
                std::cout << term_color::red << "avio_open failed" << term_color::reset  << out << std::endl;
                return -1;
            }
        }
        
        // output_fmt_ctx modify the time base even i set to 1/90000
        // 1/1000 is not a good choice
        ret = avformat_write_header(output_fmt_ctx, NULL); 
        if (ret < 0)
        {
            printf_ffmepg_error(ret, "avformat_write_header");
            return -1;
        }

        std::cout << term_color::yellow << "output_fmt_ctx->time_base:" << output_fmt_ctx->streams[0]->time_base.num << "/" << output_fmt_ctx->streams[0]->time_base.den << term_color::reset << std::endl;

        AVPacket* input_packet = av_packet_alloc();
        auto_destroy_input.set_packet(input_packet);

        int frame_index = 0;
        int new_frame_index = 0;
        int video_stream_index = CFFmpegHelper::get_video_stream_index(input_fmt_ctx);

        while (av_read_frame(input_fmt_ctx, input_packet) >= 0) 
        {
            if (input_packet->stream_index == video_stream_index) 
            {                    
                int frame_duration = output_fmt_ctx->streams[video_stream_index]->time_base.den / 25;   // 每帧 = 3600 时间单位
                input_packet->pts = (frame_index * frame_duration);
                input_packet->dts = input_packet->pts;
                input_packet->duration = frame_duration;
                input_packet->pos = -1;

                av_interleaved_write_frame(output_fmt_ctx, input_packet); 
                av_packet_unref(input_packet);
                ++frame_index;
            }
            av_packet_unref(input_packet);
        }
        av_write_trailer(output_fmt_ctx);

        std::cout << "remove_audio finished " << out << std::endl;
        return 1;
    }
}
