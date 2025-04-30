// based on opencv
#pragma once

class CStreamDisplay
{
    public:
        int init();
        int display();
        int display_video();
        int display_audio();
        int display_text();
        int display_pic();
        int display_hls();
        int display_yuv();
    private:
        bool is_init_ = false;
};