#include "stream_transfer.h"

int CStreamTransfer::init()
{
    //av_register_all();
    printf("compile version: %d.%d.%d\n", LIBAVFORMAT_VERSION_MAJOR, LIBAVFORMAT_VERSION_MINOR, LIBAVFORMAT_VERSION_MICRO);
    printf("run version: %s\n", av_version_info());

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

int CStreamTransfer::analyze_file(FileFormat& file_format, const std::string& video_path)
{
    std::cout<<"start analyze:"<< video_path << std::endl;
    init();
   
    AVFormatContext *fmt_ctx = NULL;
    const AVCodec *codec = NULL;
    
    // 1. 打开输入文件
    int ret = avformat_open_input(&fmt_ctx, video_path.c_str(), NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "openf faied%d, %s\n", ret, video_path.c_str());

        char errbuf[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(ret, errbuf, sizeof(errbuf));
        
        return -1;
    }

    // 2. 获取流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "无法获取流信息\n");
        return -1;
    }

    // 4. 检查输入格式
    if (!fmt_ctx->iformat) {
        std::cout << "iformat is null, no format" << std::endl;
    } 
    else {
        if (strcmp(fmt_ctx->iformat->name, "h264") == 0)
        {
            std::cout << "raw data" << fmt_ctx->iformat->name << std::endl;
        }
        else
        {
            std::cout << "format: " << fmt_ctx->iformat->name << std::endl;
        }
    }

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
                    codec = avcodec_find_decoder(stream->codecpar->codec_id);
                    printf("video codec: %s\n", codec->name);
                
                    if (stream->codecpar->codec_id == AV_CODEC_ID_H264) 
                    {
                        printf("it is h264 codec\n");
                    }
                }
                else if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) 
                {

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
    std::cout<<"analyze finished"<<std::endl;
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
