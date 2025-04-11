#include "stream_transfer.h"
bool CStreamTransfer::format_to_mp4(std::string& out, const std::string& video_path)
{
    return true;
}
bool CStreamTransfer::format_to_avi(std::string& out, const std::string& video_path)
{
    return true;
}
bool CStreamTransfer::format_to_yuv(std::string& out, const std::string& video_path)
{
    return true;
}
bool CStreamTransfer::format_yuv_to_rgb(std::string& out, const std::string& video_path)
{
    return true;
}

// mp4 to h264
bool CStreamTransfer::format_mp4_to_raw(std::string& out, const std::string& video_path)
{
    return true;
}

bool CStreamTransfer::analyze_file(FileFormat& file_format, const std::string& video_path)
{
    std::cout<<"start analyze"<<std::endl;
    
    AVFormatContext *fmt_ctx = NULL;
    const AVCodec *codec = NULL;
    
    // 1. 打开输入文件
    if (avformat_open_input(&fmt_ctx, video_path.c_str(), NULL, NULL) < 0) {
        fprintf(stderr, "无法打开文件%s\n", video_path.c_str());
        return false;
    }

    // 2. 获取流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "无法获取流信息\n");
        return false;
    }

    // 3. 打印封装格式
    printf("封装格式: %s\n", fmt_ctx->iformat->name);

    // 4. 遍历所有流
    for (int i = 0; i < fmt_ctx->nb_streams; i++) {
        AVStream *stream = fmt_ctx->streams[i];
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            codec = avcodec_find_decoder(stream->codecpar->codec_id);
            printf("视频编码: %s\n", codec->name);
            
            if (stream->codecpar->codec_id == AV_CODEC_ID_H264) {
                printf("这是H.264编码的视频流\n");
            }
        }
    }

    // 5. 释放资源
    avformat_close_input(&fmt_ctx);
    std::cout<<"analyze finished"<<std::endl;
    return true;
}
