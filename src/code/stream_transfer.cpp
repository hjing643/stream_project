#include "stream_transfer.h"

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

void printf_stream_info(const FileFormat& file_format)
{
    std::cout<<"format name:" << file_format.iformat_name << std::endl;
    std::cout<<"nb_streams:" << file_format.nb_streams << std::endl;
    if (file_format.duration > 0)
    {
        std::cout<<"duration(s):" << file_format.duration/(1000*1000)<< std::endl;
    }
    else{
        std::cout<<"detect duration failed"<< std::endl;
    }
    if (file_format.bit_rate > 0)
    {
        std::cout<<"bit_rate:" << file_format.bit_rate<< std::endl;
    }
    else{
        std::cout<<"detect bit_rate failed"<< std::endl;
    }

    if (file_format.start_time > 0)
    {
        std::cout<<"start_time:" << file_format.time_base<< std::endl;
    }
    else{
        std::cout<<"detect start_time failed"<< std::endl;
    }
    




    if (file_format.video_codec_id != AV_CODEC_ID_NONE)
    {
        std::cout<<"*****include video:"<< std::endl;
        const AVCodec *codec = avcodec_find_decoder(file_format.video_codec_id);
        if (codec)
        {
            std::cout << "video codec id:" << file_format.video_codec_id << ",name:" << codec->name <<std::endl;
        }
        else
        {
            std::cout << "unknown video codec name" <<std::endl;
        }
    }
    if (file_format.audio_codec_id != AV_CODEC_ID_NONE)
    {
        std::cout<<"*****include audio:" << std::endl;
        const AVCodec *codec = avcodec_find_decoder(file_format.audio_codec_id);
        std::cout << "audio codec id:" << file_format.audio_codec_id << ",name:" << codec->name <<std::endl;
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
int CStreamTransfer::analyze_file(FileFormat& file_format, const std::string& video_path)
{
    std::cout<<"start analyze:"<< video_path << std::endl;
    init();
   
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
        return -1;
    }

    // read some streas and detect stream information:codec_id, resolution, video, audio, timestamp, etc
    // [AVDictionary **] set thread number,probesize, analyzeduration,fpsprobesize.
    // av_dict_set and av_dict_free 
    ret = avformat_find_stream_info(fmt_ctx, NULL); 
    if (ret < 0 || fmt_ctx == NULL) {
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
    file_format.time_base = fmt_ctx->time_base;

    // 4. 遍历所有流
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
                    file_format.video_codec_id = stream->codecpar->codec_id;
                }
                else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) 
                {
                    file_format.audio_codec_id = stream->codecpar->codec_id;  
                }
            }
            else
            {
                std::cout<<"stream->codecpar is null"<<std::endl; 
            }
        }
    }

    // 5. 释放资源
    avformat_close_input(&fmt_ctx);
    printf_stream_info(file_format);
    std::cout<<"analyze finished"<<std::endl;
    return 1;
}


int CStreamTransfer::format_to_mp4(const std::string& out, const std::string& video_path)
{
    init();

    FILE* in_file = fopen(video_path.c_str(), "rb");
    if (!in_file) {
        fprintf(stderr, "Cannot open input file\n");
        return -1;
    }

     // 初始化输出上下文
    AVFormatContext* out_fmt_ctx = NULL;
    avformat_alloc_output_context2(&out_fmt_ctx, NULL, NULL, out.c_str());
    if (!out_fmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        return -1;
    }
    const AVOutputFormat* out_fmt = out_fmt_ctx->oformat;

    // 添加视频流
    AVStream* out_stream = avformat_new_stream(out_fmt_ctx, NULL);
    if (!out_stream) {
        fprintf(stderr, "Failed allocating output stream\n");
        return -1;
    }

    // 设置视频流的参数（伪参数，通常从编码器中获取）
    AVCodecParameters* codecpar = out_stream->codecpar;
    codecpar->codec_type = AVMEDIA_TYPE_VIDEO;
    codecpar->codec_id = AV_CODEC_ID_H264;
    codecpar->format = AV_PIX_FMT_YUV420P;
    codecpar->width = 1280;
    codecpar->height = 720;
    codecpar->bit_rate = 400000;
    out_stream->time_base = AVRational{1, 25};

    // 打开输出文件
    if (!(out_fmt->flags & AVFMT_NOFILE)) {
        if (avio_open(&out_fmt_ctx->pb, out.c_str(), AVIO_FLAG_WRITE) < 0) {
            fprintf(stderr, "Could not open output file '%s'\n", out.c_str());
            return -1;
        }
    }

    // 写入文件头
    avformat_write_header(out_fmt_ctx, NULL);

    // 构建 AVPacket 读取原始 h264 流
    AVPacket pkt;
    while (!feof(in_file)) {
        uint8_t buffer[4096];
        int bytes_read = fread(buffer, 1, sizeof(buffer), in_file);
        if (bytes_read <= 0)
            break;

        av_init_packet(&pkt);
        pkt.data = buffer;
        pkt.size = bytes_read;
        pkt.stream_index = out_stream->index;
        pkt.pts = pkt.dts = 0; // 简化处理：实际中要根据帧率计算时间戳

        av_interleaved_write_frame(out_fmt_ctx, &pkt);
    }

    av_write_trailer(out_fmt_ctx);

    fclose(in_file);
    avio_closep(&out_fmt_ctx->pb);
    avformat_free_context(out_fmt_ctx);

    printf("H.264 已封装为 MP4: %s\n", out.c_str());

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
    return 1;
}

// mp4 to h264
int CStreamTransfer::format_mp4_to_raw(const std::string& out, const std::string& video_path)
{
    return 1;
}

int CStreamTransfer::get_first_I_frame(const std::string& out, const std::string& video_path)
{

}
int CStreamTransfer::get_first_P_frame(const std::string& out, const std::string& video_path)
{

}
int CStreamTransfer::I_frame_to_yuv(const std::string& out, const std::string& video_path)
{

}
int CStreamTransfer::P_frame_to_yuv(const std::string& out, const std::string& video_path)
{
    
}
