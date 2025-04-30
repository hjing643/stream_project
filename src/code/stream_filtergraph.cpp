#include "stream_filtergraph.h"

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
        is_init_ = true;
        return 1;
    }
    int CStreamFilterGraph::scale_video(const std::string& out, const std::string& video_path)
    {
        init();
        CAutoDestroyStreamFilterGraph auto_destroy_input;
        CAutoDestroyStreamFilterGraph auto_destroy_output;
        const char *input_filename = video_path.c_str();

        AVFormatContext *fmt_ctx = nullptr;
        if (avformat_open_input(&fmt_ctx, input_filename, nullptr, nullptr) < 0) {
            std::cerr << "Failed to open input file\n";
            return -1;
        }
        auto_destroy_input.set_fmt_ctx(fmt_ctx, 0);
        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            std::cerr << "Failed to find stream info\n";
            return -1;
        }

        // Find the first video stream
        int video_stream_index = -1;
        for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
                video_stream_index = i;
                break;
            }
        }
        if (video_stream_index == -1) {
            std::cerr << "No video stream found\n";
            return -1;
        }

        const AVCodec *decoder = avcodec_find_decoder(fmt_ctx->streams[video_stream_index]->codecpar->codec_id);
        AVCodecContext *dec_ctx = avcodec_alloc_context3(decoder);
        auto_destroy_input.set_codec_context(dec_ctx);
        avcodec_parameters_to_context(dec_ctx, fmt_ctx->streams[video_stream_index]->codecpar);
        avcodec_open2(dec_ctx, decoder, nullptr);

        // 设置滤镜
        const char *filter_descr = "scale=640:360,drawbox=x=100:y=100:w=200:h=100:color=red@0.5:thickness=5,drawtext=text='Frame\\:%{n}':fontsize=20:fontcolor=white:x=10:y=10";

        AVFilterGraph *filter_graph = avfilter_graph_alloc();
        auto_destroy_input.set_filter_graph(filter_graph);
        AVFilterContext *buffersrc_ctx = nullptr;
        AVFilterContext *buffersink_ctx = nullptr;

        // 创建 buffer filter
        char args[512]={0};
        snprintf(args, sizeof(args),
                "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=1/1",
                dec_ctx->width, dec_ctx->height, dec_ctx->pix_fmt,
                fmt_ctx->streams[video_stream_index]->time_base.num,
                fmt_ctx->streams[video_stream_index]->time_base.den);

        avfilter_graph_create_filter(&buffersrc_ctx, avfilter_get_by_name("buffer"),
                                    "in", args, nullptr, filter_graph);
        avfilter_graph_create_filter(&buffersink_ctx, avfilter_get_by_name("buffersink"),
                                    "out", nullptr, nullptr, filter_graph);

        // 连接 filter graph
        AVFilterInOut *outputs = avfilter_inout_alloc();
        AVFilterInOut *inputs = avfilter_inout_alloc();
        outputs->name = av_strdup("in");
        outputs->filter_ctx = buffersrc_ctx;
        outputs->pad_idx = 0;
        outputs->next = nullptr;

        inputs->name = av_strdup("out");
        inputs->filter_ctx = buffersink_ctx;
        inputs->pad_idx = 0;
        inputs->next = nullptr;

        avfilter_graph_parse_ptr(filter_graph, filter_descr, &inputs, &outputs, nullptr);
        avfilter_graph_config(filter_graph, nullptr);

        // 解码并处理帧
        AVPacket *pkt = av_packet_alloc();
        auto_destroy_input.set_packet(pkt);
        AVFrame *frame = av_frame_alloc();
        auto_destroy_input.set_frame(frame);
        AVFrame *filt_frame = av_frame_alloc();
        auto_destroy_output.set_frame(filt_frame);

        while (av_read_frame(fmt_ctx, pkt) >= 0) 
        {
            if (pkt->stream_index == video_stream_index) 
            {
                if (avcodec_send_packet(dec_ctx, pkt) == 0) 
                {
                    while (avcodec_receive_frame(dec_ctx, frame) == 0) 
                    {
                        // 送入滤镜
                        av_buffersrc_add_frame(buffersrc_ctx, frame);
                        // 取出处理后帧
                        while (av_buffersink_get_frame(buffersink_ctx, filt_frame) >= 0) 
                        {
                            std::cout << "Processed frame: " << filt_frame->pts << std::endl;
                            av_frame_unref(filt_frame);
                        }
                        av_frame_unref(frame);
                    }
                }
            }
            av_packet_unref(pkt);
        }
        std::cout << "scale_video finished " << out << std::endl;
        return 1;
    }
    int CStreamFilterGraph::crop_video(const std::string& out, const std::string& video_path)
    {
        return 1;   
    }
    int CStreamFilterGraph::overload_video(const std::string& out, const std::string& video_path)
    {
        return 1;
    }
    int CStreamFilterGraph::drawbox_video(const std::string& out, const std::string& video_path)
    {
        return 1;
    }
    int CStreamFilterGraph::drawtext_video(const std::string& out, const std::string& video_path)
    {
        return 1;   
    }
    int CStreamFilterGraph::boxblur_video(const std::string& out, const std::string& video_path)
    {
        return 1;
    }       
}