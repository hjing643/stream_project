#include "stream_transfer.h"

class CAutoDestroy
{
public:
    ~CAutoDestroy()
    {
        if (input_fmt_ctx_ != NULL)
        {
            avformat_close_input(&input_fmt_ctx_);
            input_fmt_ctx_ = NULL;
        }
        if (output_fmt_ctx_ != NULL)
        {
            if (!(output_fmt_ctx_->oformat->flags & AVFMT_NOFILE)
            && output_fmt_ctx_->pb) 
            {
                // rtsp pb is not null, but we can't closep
                avio_closep(&output_fmt_ctx_->pb);
            }
            avformat_free_context(output_fmt_ctx_);
            output_fmt_ctx_ = NULL;
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
    }
    void set_input_fmt_ctx(AVFormatContext* ptr_in)
    {
        input_fmt_ctx_ = ptr_in;
    }
    void set_out_fmt_ctx(AVFormatContext* ptr_in)
    {
        output_fmt_ctx_ = ptr_in;
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
private:
    AVFormatContext* input_fmt_ctx_ = NULL;  
    AVFormatContext* output_fmt_ctx_ = NULL;  
    AVCodecContext* codec_ctx_ = NULL;
    AVPacket* packet_= NULL; 
    AVFrame* frame_= NULL; 
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
            return "unkknow pix format";
        }
    }
    else if(stream_type == 1)
    {

    }
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
        std::cout << "audio channels:" << file_format.audio_stream->codecpar->channels <<std::endl;  
    }
}
void printf_ffmepg_error(int error_code, const std::string& fun_name)
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
    av_strerror(error_code, errbuf, sizeof(errbuf));

    char finalbuf[256] = {0};
    snprintf(finalbuf, sizeof(finalbuf), "%s faied, error code [%d-%s]", fun_name.c_str(), error_code, errbuf);

    std::cout << finalbuf << std::endl;
}

void write_frame_to_yuv(const char* file_path, AVFrame* ptr_frame)
{
    std::cout<<"width:"<< ptr_frame->width << ",height"<<ptr_frame->height<< std::endl;

    FILE* f = fopen(file_path, "wb");

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
    CAutoDestroy auto_destroy;
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
    auto_destroy.set_input_fmt_ctx(fmt_ctx);
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

    int video_stream_index = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        if (stream == NULL)
        {
            std::cout<<"stream is null"<<std::endl;        
        }
        else
        {
            if (stream->codecpar != NULL)
            {
                if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) 
                {
                    video_stream_index = i;
                    file_format.video_stream = stream;
                }
                else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) 
                {
                    file_format.audio_stream = stream;  
                }
            }
            else
            {
                std::cout<<"stream->codecpar is null"<<std::endl; 
            }
        }
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
            AVCodecParameters* codec_parameter = fmt_ctx->streams[video_stream_index]->codecpar;
            const AVCodec* codec = avcodec_find_decoder(codec_parameter->codec_id);
            if (!codec) {
                std::cerr << "Unsupported codec" << std::endl;
                return -1;
            }

            AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
            auto_destroy.set_codec_context(codec_ctx);

            avcodec_parameters_to_context(codec_ctx, codec_parameter);
            ret = avcodec_open2(codec_ctx, codec, NULL);
            if (ret < 0)
            {
                printf_ffmepg_error(ret, "avcodec_open2");
                break;
            }

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
                    if (pkt->flags & AV_PKT_FLAG_KEY)
                    {
                        idr_mark = true;
                    }
                    snprintf(finalbuf, sizeof(finalbuf), "%d IDR[%d],size[%d],pts[%d],dts[%d],duration[%d],pos[%d]", 
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
                    while (avcodec_receive_frame(codec_ctx, frame) == 0) 
                    {
                        char pict_type = av_get_picture_type_char(frame->pict_type);
                        double pts_sec = frame->pts * av_q2d(fmt_ctx->streams[video_stream_index]->time_base);
                        std::cout << "  Frame: type=" << pict_type
                                << ", pts=" << pts_sec
                                << ", size=" << frame->width << "x" << frame->height
                                << std::endl;

                        if (!(pkt->flags & AV_PKT_FLAG_KEY)
                        && frame->pict_type == AV_PICTURE_TYPE_I)
                        {
                            std::cout << "  non IDR I Frame" << std::endl;
                        }
                        av_frame_unref(frame);
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
    init();
    CAutoDestroy auto_destroy;

    const char* input_filename = video_path.c_str();
    const char* output_filename = out.c_str();

    AVFormatContext* input_ctx = NULL;
    int ret = 0;

    // open raw h264
    if ((ret = avformat_open_input(&input_ctx, input_filename, NULL, NULL)) < 0) {
        printf_ffmepg_error(ret,"avformat_open_input");
        if (input_ctx)
        {
            avformat_free_context(input_ctx);
            input_ctx = NULL;
        }
        return ret;
    }

    auto_destroy.set_input_fmt_ctx(input_ctx);
    // find stram information
    ret =  avformat_find_stream_info(input_ctx, NULL);
    if (ret < 0) {
        printf_ffmepg_error(ret,"avformat_find_stream_info");
        return ret;
    }

    // 创建输出上下文
    AVFormatContext* output_ctx = NULL;
    ret = avformat_alloc_output_context2(&output_ctx, NULL, NULL, output_filename);
    if (ret < 0) 
    {
        printf_ffmepg_error(ret, "avformat_alloc_output_context2");
        return ret;
    }
    auto_destroy.set_out_fmt_ctx(output_ctx);
    
    //std::cout << "out format codec_tag:" << output_ctx->oformat->codec_tag << std::endl;

    // 假设输入只有一个视频流
    do
    {
        AVStream* out_stream = NULL;
        for (unsigned int i = 0; i < input_ctx->nb_streams; i++) 
        {
            AVStream* in_stream = input_ctx->streams[i];

            if (in_stream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
                continue;
            }

            // 创建输出流
            out_stream = avformat_new_stream(output_ctx, NULL);
            if (!out_stream) {
                std::cerr << "Failed allocating output stream\n";
                ret = AVERROR_UNKNOWN;
                break;
            }

            // 复制编解码器参数
            if ((ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar)) < 0) {
                std::cerr << "Failed to copy codec parameters\n";
                break;
            }

            // 对于 H.264 视频，需要设置一些 MP4 特定的参数
            out_stream->codecpar->codec_tag = av_codec_get_tag(output_ctx->oformat->codec_tag, out_stream->codecpar->codec_id);
            if (out_stream->codecpar->codec_tag == 0) {
                std::cerr << "Could not find codec tag for codec id " << out_stream->codecpar->codec_id << "\n";
                ret = AVERROR_UNKNOWN;
                break;
            }
        }
        if (ret < 0)
        {
            break;
        }

        // 如果没有找到视频流
        if (!out_stream) {
            std::cerr << "Could not find video stream in input\n";
            ret = AVERROR_UNKNOWN;
            break;
        }

        // 打开输出文件
        if (!(output_ctx->oformat->flags & AVFMT_NOFILE)) {
            if ((ret = avio_open(&output_ctx->pb, output_filename, AVIO_FLAG_WRITE)) < 0) {
                std::cerr << "Could not open output file '" << output_filename << "'\n";
                break;
            }
        }

        // 写入文件头
        if ((ret = avformat_write_header(output_ctx, NULL)) < 0) {
            std::cerr << "Error occurred when opening output file\n";
            break;
        }

        // 复制数据包
        AVPacket pkt;
        int64_t frame_index = 0;  // 第 N 帧（从 0 开始）
        while (1) {
            ret = av_read_frame(input_ctx, &pkt);
            if (ret < 0) {
                break; // 读取结束或出错
            }

            AVStream* in_stream = input_ctx->streams[pkt.stream_index];
            if (in_stream->codecpar->codec_type != AVMEDIA_TYPE_VIDEO) {
                continue;
            }

            //out_stream = output_ctx->streams[0]; // we only have video stream in output
            
            // 转换 PTS/DTS
            // 裸流一般没有time_base，初始化一个我想要的
            AVRational mp4_time_base = (AVRational){1, 90000};
            output_ctx->streams[0]->r_frame_rate = (AVRational){25, 1};
            output_ctx->streams[0]->avg_frame_rate = (AVRational){25, 1};
            output_ctx->streams[0]->time_base = mp4_time_base;
            int frame_duration = 90000 / 25;   // 每帧 = 3600 时间单位

            pkt.pts = (frame_index * frame_duration);
            pkt.dts = pkt.pts;
            pkt.duration = frame_duration;
            pkt.pos = -1;
            ++frame_index;
            // 写入数据包
            if ((ret = av_interleaved_write_frame(output_ctx, &pkt)) < 0) {
                std::cerr << "Error muxing packet\n";
                av_packet_unref(&pkt);
                break;
            }

            av_packet_unref(&pkt);
        }

        // 写入文件尾
        av_write_trailer(output_ctx);
    }while(0);


    // 清理
    
    //avformat_free_context(output_ctx);
    ///avformat_close_input(&input_ctx);

    if (ret < 0 && ret != AVERROR_EOF) {
        std::cerr << "Error occurred: " << ret << std::endl;
        return 1;
    }

    std::cout << "Successfully converted " << input_filename << " to " << output_filename << std::endl;
    return 1;
}

int CStreamTransfer::format_to_avi(const std::string& out, const std::string& video_path)
{
    return 1;
}
int CStreamTransfer::format_to_yuv(const std::string& out, const std::string& video_path)
{
    return 1;
}
int CStreamTransfer::format_yuv_to_rgb(const std::string& out, const std::string& video_path)
{
    init();
    CAutoDestroy auto_destroy1;
    CAutoDestroy auto_destroy2;

    int width = 1920;
    int height = 1080;
    enum AVPixelFormat pix_fmt = AV_PIX_FMT_YUV420P;

    FILE* in = fopen(video_path.c_str(), "rb");
    if (!in)
    {
        std::cout<<"read file failed"<<std::endl;
        return -1;
    }
    AVFrame* yuv_frame = av_frame_alloc();
    auto_destroy1.set_frame(yuv_frame);

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
    width, height, pix_fmt,
    width, height, AV_PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    AVFrame* rgb_frame = av_frame_alloc();
    auto_destroy2.set_frame(rgb_frame);
    rgb_frame->format = AV_PIX_FMT_RGB24;
    rgb_frame->width = width;
    rgb_frame->height = height;
    av_frame_get_buffer(rgb_frame, 32);

    sws_scale(sws_ctx,
            yuv_frame->data, yuv_frame->linesize, 0, height,
            rgb_frame->data, rgb_frame->linesize);

    sws_freeContext(sws_ctx);

    // 6. 编码为 PNG 或 BMP
    AVCodecID codec_id = AV_CODEC_ID_PNG; // 改成 AV_CODEC_ID_BMP 即可保存 BMP
    const AVCodec* codec = avcodec_find_encoder(codec_id);
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    auto_destroy2.set_codec_context(codec_ctx);
    codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;
    codec_ctx->width = width;
    codec_ctx->height = height;
    codec_ctx->time_base = (AVRational){1, 25};
    avcodec_open2(codec_ctx, codec, NULL);

    AVPacket* pkt = av_packet_alloc();
    auto_destroy2.set_packet(pkt);
    pkt->data = NULL;
    pkt->size = 0;

    avcodec_send_frame(codec_ctx, rgb_frame);
    if (avcodec_receive_packet(codec_ctx, pkt) == 0) {
        FILE* f_out = fopen(out.c_str(), "wb");
        if (f_out)
        {
            fwrite(pkt->data, 1, pkt->size, f_out);
            fclose(f_out);
        }
        else
        { 
            std::cout<<"write file failed"<<std::endl;   
        }
        av_packet_unref(pkt);
    }
        
    std::cout<<"******************format_yuv_to_rgb finished******************"<<std::endl;
    return 1;
}

// mp4 to h264
int CStreamTransfer::format_mp4_to_raw(const std::string& out, const std::string& video_path)
{
    return 1;
}

int CStreamTransfer::get_first_frame_to_yuv(const std::string& out, const std::string& video_path, int frame_type)
{
    init();
    CAutoDestroy auto_destroy;
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
    auto_destroy.set_input_fmt_ctx(fmt_ctx);

    // read some streas and detect stream information:codec_id, resolution, video, audio, timestamp, etc
    // [AVDictionary **] set thread number,probesize, analyzeduration,fpsprobesize.
    // av_dict_set and av_dict_free 
    ret = avformat_find_stream_info(fmt_ctx, NULL); 
    if (ret < 0) 
    {
        printf_ffmepg_error(ret, "avformat_find_stream_info");
        return -1;
    }

    int video_stream_index = -1;
    for (int i = 0; i < fmt_ctx->nb_streams; i++) 
    {
        AVStream *stream = fmt_ctx->streams[i];
        if (stream == NULL)
        {
            std::cout<<"stream is null"<<std::endl;
            return -1;     
        }
        else
        {
            if (stream->codecpar != NULL)
            {
                if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) 
                {
                    video_stream_index = i;
                    break;
                }
            }
            else
            {
                std::cout<<"stream->codecpar is null"<<std::endl; 
                return -1;
            }
        }
    }
    if (video_stream_index < 0)
    {           
        std::cout<<"find video stream failed"<<std::endl;
        return -1;
    }
   
    AVCodecParameters* codec_parameter = fmt_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_parameter->codec_id);
    if (!codec) {
        std::cerr << "Unsupported codec" << std::endl;
        return -1;
    }

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    auto_destroy.set_codec_context(codec_ctx);

    avcodec_parameters_to_context(codec_ctx, codec_parameter);
    
    ret = avcodec_open2(codec_ctx, codec, NULL);
    if (ret < 0)
    {
        printf_ffmepg_error(ret, "avcodec_open2");
        return -1;
    }

    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    auto_destroy.set_packet(pkt);
    auto_destroy.set_frame(frame);

    int index = 0;
                    
    std::cout<<"start decoder"<<std::endl;
    bool find = false;
    int find_frame_index = 1; // find the 5 frame
    int current_frame_index = 0;
    while (av_read_frame(fmt_ctx, pkt) >= 0) 
    {
        if (pkt->stream_index == video_stream_index) 
        {
            avcodec_send_packet(codec_ctx, pkt);
            while (avcodec_receive_frame(codec_ctx, frame) == 0) 
            {
                std::cout<<"pict_type:"<<frame->pict_type<< std::endl;

                if (frame->pict_type == frame_type)
                {
                    ++current_frame_index;
                    if (current_frame_index == find_frame_index)
                    {
                        find = true;
                        write_frame_to_yuv(out.c_str(), frame);

                    }
                    std::cout<<"find the frame, current_frame_index:"<< current_frame_index << std::endl;
                    break;
                }
            }
        }
        if(find)
        {
            break;
        }
    }
    std::cout<<"******************get_first_frame_to_yuv finished******************"<<std::endl;
    return 1;
}