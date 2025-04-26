#pragma once
#include "../includeall.h"

struct FileFormat
{
    std::string iformat_name; 
    int nb_streams = 0;


    AVCodecID  video_codec_id = AV_CODEC_ID_NONE; // video codec id, AV_CODEC_ID_NONE means no video stream
    AVCodecID audio_codec_id = AV_CODEC_ID_NONE; // audio codec id, AV_CODEC_ID_NONE means no audio stream
    int64_t duration = AV_NOPTS_VALUE; // microseconds
    int64_t bit_rate = 0;
    int64_t start_time = AV_NOPTS_VALUE;

    AVStream* video_stream = NULL;
    AVStream* audio_stream = NULL;
};

class CStreamTransfer
{
    public:
        int init();

        // analyze the file
        int analyze_file(const std::string& video_path, bool deep);

        // h264 raw data to others
        int format_raw_to_mp4(const std::string& out, const std::string& video_path);
        int format_raw_to_avi(const std::string& out, const std::string& video_path);
        int format_raw_to_yuv(const std::string& out, const std::string& video_path);
        int format_yuv_to_rgb(const std::string& out, const std::string& video_path);

        //v9 to h264
        int format_v9_to_h264(const std::string& out, const std::string& video_path);
        int change_resolution(const std::string& out, const std::string& video_path, int width, int height);
        int change_fps(const std::string& out, const std::string& video_path, int fps);

        // get the frame and write the data to yuv
        // frame_type: 1-I，2-P,3-B
        // dst_codec: 0-raw, no decoder, 1-YUV,2-RGB,3-PNG
        int get_first_frame(const std::string& out, const std::string& video_path, int frme_type, int dst_codec);

        // mp4 to h264
        int format_mp4_to_raw(const std::string& out, const std::string& video_path);

    public:
        int draw_text();
        int draw_pic();
        int hls();

    private:
        bool is_init_ = false;

        //std::atomic<bool> is_init_ = false;
};
