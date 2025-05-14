#include "stream_display.h"

namespace stream_project
{
    int CStreamDisplay::init()
    {
        if (is_init_)
        {
            return 1;
        }   
        is_init_ = true;
        return 1;
    }
    int CStreamDisplay::compress_png(const std::string &dst_path, const std::string &src_path)
    {
        if (src_path.empty())
        {
            std::cout << "Failed to load image: " << src_path << std::endl;
            return -1;
        }
        cv::Mat src_img = cv::imread(src_path);
        if (src_img.empty())
        {
            std::cout << "Failed to load image: " << src_path << std::endl;
            return -1;
        }
        if (!cv::imwrite(dst_path, src_img, {cv::IMWRITE_PNG_COMPRESSION, 9}))
        {
            std::cout << "Failed to write image: " << dst_path << std::endl;
            return -1;
        }
        std::cout << "Compress image: " << dst_path << " success" << std::endl;
        return 1;
    }
    int CStreamDisplay::display_video()
    {
        return 1;
    }
    
}