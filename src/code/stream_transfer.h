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
        int format_to_mp4(const std::string& out, const std::string& video_path);
        int format_to_avi(const std::string& out, const std::string& video_path);
        int format_to_yuv(const std::string& out, const std::string& video_path);
        int format_yuv_to_rgb(const std::string& out, const std::string& video_path);

        // get the frame
        int get_first_I_frame(const std::string& out, const std::string& video_path);
        int get_first_P_frame(const std::string& out, const std::string& video_path);
        int I_frame_to_yuv(const std::string& out, const std::string& video_path);
        int P_frame_to_yuv(const std::string& out, const std::string& video_path);

        // mp4 to h264
        int format_mp4_to_raw(const std::string& out, const std::string& video_path);

        
    private:
        bool is_init_ = false;

        //std::atomic<bool> is_init_ = false;
};