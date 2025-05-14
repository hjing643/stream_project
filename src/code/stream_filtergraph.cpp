#include "stream_filtergraph.h"
#include "ffmepg_helper.h"
namespace stream_project
{
    class CAutoDestroyStreamFilterGraph
    {
    public:
        ~CAutoDestroyStreamFilterGraph()
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
            }
            if (filter_graph_ != NULL)
            {
                avfilter_graph_free(&filter_graph_);
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
        }
        void set_fmt_ctx(AVFormatContext* ptr_in, int type)
        {
            fmt_ctx_ = ptr_in;
            fmt_type_ = type;
        }
        void set_codec_context(AVCodecContext* ptr_in)
        {
            codec_ctx_ = ptr_in;
        }
        void set_filter_graph(AVFilterGraph* ptr_in)
        {
            filter_graph_ = ptr_in;
        }
        void set_packet(AVPacket* ptr_in)
        {
            packet_ = ptr_in;
        }
        void set_frame(AVFrame* ptr_in)
        {
            frame_ = ptr_in;
        }
    private:
        AVFormatContext *fmt_ctx_ = nullptr;
        int fmt_type_ = 0; //0 input, 1 output 
        AVCodecContext *codec_ctx_ = nullptr;
        AVFilterGraph *filter_graph_ = nullptr;
        AVPacket *packet_ = nullptr;
        AVFrame *frame_ = nullptr;
    };

    int CStreamFilterGraph::init()
    {
        if (is_init_)
        {
            return 1;
        }
        opts_ = nullptr;
        av_dict_set(&opts_, "threads", "8", 0);
        is_init_ = true;
        return 1;
    }
    CStreamFilterGraph::~CStreamFilterGraph()
    {
        av_dict_free(&opts_);
    }

    int CStreamFilterGraph::filter_video(const std::string& out, const std::string& video_path)
    {
        init();
        CAutoDestroyStreamFilterGraph auto_destroy_input;
        CAutoDestroyStreamFilterGraph auto_destroy_output;
        const char *input_filename = video_path.c_str();
        const char *output_filename = out.c_str();
        int ret = 0;

        AVFormatContext *fmt_ctx = nullptr;
        if (avformat_open_input(&fmt_ctx, input_filename, nullptr, &opts_) < 0) 
        {
            std::cerr << "Failed to open input file" << std::endl;
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(fmt_ctx, 0);
        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) 
        {
            std::cerr << "Failed to find stream info" << std::endl;
            return -1;
        }

        // Find the first video stream
        int video_stream_index = CFFmpegHelper::get_video_stream_index(fmt_ctx);
        if (video_stream_index == -1) 
        {
            std::cerr << term_color::red << "No video stream found" << term_color::reset << std::endl;
            return -1;
        }
        AVStream* in_video_stream = fmt_ctx->streams[video_stream_index];

        AVCodecContext* dec_ctx = CFFmpegHelper::create_video_decodec_context(in_video_stream, true);
        auto_destroy_input.set_codec_context(dec_ctx);

        // create encoder context
        AVCodecContext* encoder_ctx = CFFmpegHelper::create_video_encodec_context(in_video_stream, false);
        //encoder_ctx->width = 1280;
        //encoder_ctx->height = 720;
        std::cout << "encoder_ctx->width: " << encoder_ctx->width << std::endl;
        std::cout << "encoder_ctx->height: " << encoder_ctx->height << std::endl;
        encoder_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER; // it's very important, otherwise the output file will be invalid
        encoder_ctx->bit_rate = CFFmpegHelper::estimate_bitrate(encoder_ctx->width, encoder_ctx->height);
        auto_destroy_output.set_codec_context(encoder_ctx);

        // create video stream
        const AVCodec* encoder = avcodec_find_encoder(in_video_stream->codecpar->codec_id);
        if (!encoder) 
        {
            std::cerr << "Unsupported encoder" << std::endl;
            return -1;
        }        
        std::cout << "encoder_ctx->timebase(before open): " << encoder_ctx->time_base.num << "/" << encoder_ctx->time_base.den << std::endl;
        ret = avcodec_open2(encoder_ctx, encoder, &opts_);
        if (ret < 0)
        {
            std::cerr << term_color::red << "Failed to open encoder" << term_color::reset << std::endl;
            return ret;
        }
        std::cout << "encoder_ctx->timebase(after open): " << encoder_ctx->time_base.num << "/" << encoder_ctx->time_base.den << std::endl;

        // after open it, set the parameters
        std::string input_format_name = CFFmpegHelper::get_input_format_name(fmt_ctx);
        AVFormatContext* output_fmt_ctx = NULL;
        avformat_alloc_output_context2(&output_fmt_ctx, NULL, "mp4", output_filename);
        auto_destroy_output.set_fmt_ctx(output_fmt_ctx, 1);

        AVStream* out_stream = CFFmpegHelper::create_new_video_stream(encoder_ctx, output_fmt_ctx, encoder_ctx->codec_id);
        if (!out_stream)
        {
            std::cerr << term_color::red << "Failed to create new video stream" << term_color::reset << std::endl;
            return -1;
        }

        if (!(output_fmt_ctx->oformat->flags & AVFMT_NOFILE)) 
        {
            ret = avio_open(&output_fmt_ctx->pb, output_filename, AVIO_FLAG_WRITE);
            if (ret < 0 || !output_fmt_ctx->pb)
            {
                std::cout << term_color::red << "avio_open failed" << term_color::reset << output_filename << std::endl;
                return -1;
            }
        }

        ret = avformat_write_header(output_fmt_ctx, NULL);    
        if (ret < 0)
        {
            std::cerr << term_color::red << "Failed to write header" << term_color::reset << std::endl;
            return ret;
        }

        // 设置滤镜
        const char *filter_des =
        "[in]"
        //"crop=100:100:0:0,"
        //"scale=1920:1080,"
        //"scale=1280:720,"
        "drawbox=x=100:y=100:w=200:h=100:color=red@0.5:thickness=5,"
        "drawtext=fontfile='/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf':"
        "text='Gene.Huang.Frame\\:%{n}':fontsize=24:fontcolor=white:x=10:y=10"
        //"boxblur=2:1"
        "[out]";


        AVFilterGraph *filter_graph = avfilter_graph_alloc();
        filter_graph->nb_threads  = 8;
        auto_destroy_input.set_filter_graph(filter_graph);
        AVFilterContext *buffersrc_ctx = nullptr;
        AVFilterContext *buffersink_ctx = nullptr;

        // 创建 buffer filter
        char args[512]={0};
        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=1/1",
                in_video_stream->codecpar->width, 
                in_video_stream->codecpar->height, 
                in_video_stream->codecpar->format,
                in_video_stream->time_base.num,
                in_video_stream->time_base.den);

        std::cout << "args: " << args << std::endl;

        ret = avfilter_graph_create_filter(&buffersrc_ctx, avfilter_get_by_name("buffer"),
                                    "test_in", args, nullptr, filter_graph);
        if (ret < 0)
        {
            std::cerr << term_color::red << "Failed to create buffer source filter" << term_color::reset << std::endl;
            return ret;
        }
        ret = avfilter_graph_create_filter(&buffersink_ctx, avfilter_get_by_name("buffersink"),
                                    "test_out", nullptr, nullptr, filter_graph);
        if (ret < 0)    
        {
            std::cerr << term_color::red << "Failed to create buffer sink filter" << term_color::reset << std::endl;
            return ret;
        }
        // 连接 filter graph
        AVFilterInOut *outputs = avfilter_inout_alloc();
        outputs->name = av_strdup("in");               // 与 [in] 匹配
        outputs->filter_ctx = buffersrc_ctx;           // 对应 buffer 输入节点
        outputs->pad_idx = 0;
        outputs->next = nullptr;

        AVFilterInOut *inputs = avfilter_inout_alloc();
        inputs->name = av_strdup("out");               // 与 [out] 匹配
        inputs->filter_ctx = buffersink_ctx;           // 对应 buffersink 输出节点
        inputs->pad_idx = 0;
        inputs->next = nullptr;

        ret = avfilter_graph_parse_ptr(filter_graph, filter_des, &inputs, &outputs, nullptr);
        if (ret < 0)
        {
            std::cerr << term_color::red << "Failed to parse filter graph" << term_color::reset << std::endl;
            return ret;
        }
        ret = avfilter_graph_config(filter_graph, nullptr);
        if (ret < 0)
        {
            std::cerr << term_color::red << "Failed to config filter graph" << term_color::reset << std::endl;
            return ret;
        }

        // 解码并处理帧
        AVPacket *pkt = av_packet_alloc();
        auto_destroy_input.set_packet(pkt);
        AVFrame *frame = av_frame_alloc();
        auto_destroy_input.set_frame(frame);

        AVPacket *filt_pkt = av_packet_alloc();
        auto_destroy_output.set_packet(filt_pkt);
        AVFrame *filt_frame = av_frame_alloc();
        auto_destroy_output.set_frame(filt_frame);

        int frame_index = 0;
        while (av_read_frame(fmt_ctx, pkt) >= 0) 
        {
            if (pkt->stream_index == video_stream_index) 
            {
                if (avcodec_send_packet(dec_ctx, pkt) == 0) 
                {               
                    std::cout << "avcodec_send_packet success" << std::endl;
                    while (avcodec_receive_frame(dec_ctx, frame) >= 0) 
                    {
                        //std::cout << "avcodec_receive_frame success" << std::endl;
                        // 送入滤镜
                        ret = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
                        if (ret < 0)
                        {
                            std::cerr << term_color::red << "Failed to add frame to buffer source" << term_color::reset << std::endl;
                            return ret;
                        }   
                        //std::cout << "av_buffersrc_add_frame_flags success" << std::endl;

                        // 取出处理后帧
                        while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0) 
                        {
                            //std::cout << "filter width: " << filt_frame->width << std::endl;
                            //std::cout << "filt_frame->linesize[0] = " << filt_frame->linesize[0] << std::endl;

                            //std::cout << "av_buffersink_get_frame success" << std::endl;

                            filt_frame->pts = av_rescale_q(filt_frame->pts,
                                            in_video_stream->time_base,
                                            encoder_ctx->time_base);
                                
                            ret = avcodec_send_frame(encoder_ctx, filt_frame);
                            if (ret < 0)
                            {
                                std::cerr << term_color::red << "Failed to send frame to encoder" << term_color::reset << std::endl;
                                return ret;
                            }
                            //std::cout << "avcodec_send_frame filt_frame success" << std::endl;

                            while (avcodec_receive_packet(encoder_ctx, filt_pkt) == 0) 
                            {
                                filt_pkt->stream_index = out_stream->index;
                                //std::cout << "write frame, stream index: " << frame_index++ << std::endl;
                                av_interleaved_write_frame(output_fmt_ctx, filt_pkt);
                                av_packet_unref(filt_pkt);
                            }
                            av_frame_unref(filt_frame);
                        }                   
                        av_frame_unref(frame);

                    }
                }
            }
            av_packet_unref(pkt);
        }

        if (avcodec_send_packet(dec_ctx, NULL) == 0) 
        {                        
            while (avcodec_receive_frame(dec_ctx, frame) == 0) 
            {
                // 送入滤镜
                ret = av_buffersrc_add_frame_flags(buffersrc_ctx, frame, AV_BUFFERSRC_FLAG_KEEP_REF);
                if (ret < 0)
                {
                    std::cerr << term_color::red << "Failed to add frame to buffer source" << term_color::reset << std::endl;
                    return ret;
                }
                // 取出处理后帧
                while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0) 
                {
                    filt_frame->pts = av_rescale_q(filt_frame->pts,
                                    in_video_stream->time_base,
                                    encoder_ctx->time_base);
                        
                    avcodec_send_frame(encoder_ctx, filt_frame);
                    while (avcodec_receive_packet(encoder_ctx, filt_pkt) == 0) {
                        filt_pkt->stream_index = out_stream->index;
                        av_interleaved_write_frame(output_fmt_ctx, filt_pkt);
                        av_packet_unref(filt_pkt);
                    }
                    av_frame_unref(filt_frame);
                }
                av_frame_unref(frame);
            }
        }

        
        av_write_trailer(output_fmt_ctx);
        std::cout << "scale_video finished " << out << std::endl;
        return 1;
    } 
}