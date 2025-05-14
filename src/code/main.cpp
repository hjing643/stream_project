#include <iostream>
#include <string>
#include "stream_transfer.h"
#include "rawstream_read.h"
#include "heicstream_read.h"
#include "picture_transfer.h"
#include "stream_filtergraph.h"
#include "stream_display.h"
#include "test.h"
int main(int argc, char* argv[])
{

    test::CTest test;
    test.test();

    if (argc < 2)
    {
        std::cout << "please input file name" <<std::endl;
        return -1;
    }
    std::string file_output;
    if(argc > 2)
    {
        file_output = argv[2];
    }
    stream_project::CStreamTransfer stream_transfer;
    stream_project::CRawStreamRead read_stream;
    stream_project::CHeicStreamRead heic_stream_read;
    stream_project::CPictureTransfer picture_transfer;
    stream_project::CStreamFilterGraph stream_filter_graph;
    stream_project::CStreamDisplay stream_display;

    std::string fullPath = argv[1];

    std::cout <<"filetype:" 
        << "\ntest_all:0"
        << "\nstream_transcode:1"
        << "\nrawstream_read:2"
        << "\nopencv:3"
        << "\ntest_tmp:4"
        << std::endl;

    int file_type = 0;
    std::cin >> file_type;
    if (file_type == 4)
    {
        //stream_filter_graph.scale_video("/home/gene/mnt-231-ubunto/output/filter_graph.mp4", "../fileDepends/videos/person1.mp4");
        picture_transfer.transfer_raw_to_picture("../output/i.yuv", 
        AV_PIX_FMT_YUV420P, 3840, 2160, "../output/i.png", AV_CODEC_ID_PNG);
    }
    else if (file_type == 0)
    {
        stream_transfer.analyze_file("../fileDepends/videos/h264.h264", false);
        stream_transfer.analyze_file("../fileDepends/videos/person1.mp4", false);
        stream_transfer.analyze_file("../fileDepends/videos/person1.webm", false);

        stream_transfer.format_raw_to_mp4("/home/gene/mnt-231-ubunto/output/h264.mp4", "../fileDepends/videos/h264.h264");
        stream_transfer.format_mp4_to_raw("/home/gene/mnt-231-ubunto/output/person1.h264", "../fileDepends/videos/person1.mp4");
        stream_transfer.format_raw_to_avi("/home/gene/mnt-231-ubunto/output/h264.avi", "../fileDepends/videos/h264.h264");
        stream_transfer.format_webm_to_mp4("/home/gene/mnt-231-ubunto/output/mp4_from_webm.mp4", "../fileDepends/videos/person1.webm");

        stream_transfer.change_resolution("/home/gene/mnt-231-ubunto/output/720P.mp4", "../fileDepends/videos/person1.mp4", 1280, 720);
        stream_transfer.change_fps("/home/gene/mnt-231-ubunto/output/15fps.mp4", "../fileDepends/videos/person1.mp4");


        heic_stream_read.transfer_heic_to_picture("/home/gene/mnt-231-ubunto/output/1.jpg", "../fileDepends/videos/1.HEIC", AV_CODEC_ID_MJPEG);
        heic_stream_read.transfer_heic_to_picture("/home/gene/mnt-231-ubunto/output/1.png", "../fileDepends/videos/1.HEIC", AV_CODEC_ID_PNG);

        stream_filter_graph.filter_video("/home/gene/mnt-231-ubunto/output/filter_graph.mp4", "../fileDepends/videos/vehicle1.mp4");
        stream_transfer.cut_video("/home/gene/mnt-231-ubunto/output/cut_video.mp4", "../fileDepends/videos/vehicle1.mp4", 10, 20);
        stream_transfer.remove_audio("/home/gene/mnt-231-ubunto/output/remove_audio.mp4", "../fileDepends/videos/vehicle1.mp4");

        stream_transfer.get_first_frame("/home/gene/mnt-231-ubunto/output/I.png", "../fileDepends/videos/person1.mp4", 1, 3);
        stream_transfer.get_first_frame("/home/gene/mnt-231-ubunto/output/P.png", "../fileDepends/videos/person1.mp4", 2, 3);
        return 0;
    }
    else if (file_type == 1)
    {
        std::cout << "function type:"
            << "\n0-analyze_file"
            << "\n1-deep analyze_file"
            << "\n2-format_raw_to_mp4"
            << "\n3-format_mp4_to_raw"
            << "\n4-format_raw_to_avi"
            << "\n5-format_webm_to_mp4"
            << "\n6-change_resolution"
            << "\n7-change_fps"
            << "\n8-heic_to_jpg"
            << "\n9-heic_to_png"
            << "\n10-filter_video"
            << "\n11-cut_video"
            << "\n12-remove_audio"
            << "\n100-get_first_I_frame_to_yuv"
            << "\n101-get_first_I_frame_to_png"
            << std::endl;

        int function_type = 0;
        std::cin >> function_type;

        switch (function_type)
        {
            case 0:
            {
                stream_transfer.analyze_file(fullPath, false);   
            }
                break;
            case 1:
            {
                stream_transfer.analyze_file(fullPath, true);   
            }
                break;
            case 2:
            {
                if (file_output.empty())
                {
                    file_output = "../output/mp4.mp4";      
                }
                stream_transfer.format_raw_to_mp4(file_output, fullPath);       
            }
                break;
            case 3:
            {
                if (file_output.empty())
                {
                    file_output = "../output/h264.h264";      
                }
                stream_transfer.format_mp4_to_raw(file_output, fullPath);       
            }
                break;
            case 4:
            {
                if (file_output.empty())
                {
                    file_output = "../output/avi.avi";      
                }
                stream_transfer.format_raw_to_avi(file_output, fullPath);
            }
                break;
            case 5:
            {
                if (file_output.empty())
                {
                    file_output = "../output/mp4_from_webm.mp4";      
                }
                stream_transfer.format_webm_to_mp4(file_output, fullPath);
            }
                break;
            case 6:
            {
                int target_width = 1280;
                int target_height = 720;
                if (file_output.empty())
                {
                    file_output = "../output/720P.mp4";      
                }
                stream_transfer.change_resolution(file_output, fullPath, target_width, target_height);
            }
                break;
            case 7:
            {
                if (file_output.empty())
                {
                    file_output = "../output/15fps.mp4";
                }
                stream_transfer.change_fps(file_output, fullPath);
            }
                break;
            case 8:
            {
                if (file_output.empty())
                {
                    file_output = "../output/jpg.jpg";
                }
                heic_stream_read.transfer_heic_to_picture(file_output, fullPath, AV_CODEC_ID_MJPEG);
            }
                break;
            case 9:
            {
                if (file_output.empty())
                {
                    file_output = "../output/png.png";
                }
                heic_stream_read.transfer_heic_to_picture(file_output, fullPath, AV_CODEC_ID_PNG);
            }
                break;
            case 10:
            {
                if (file_output.empty())
                {
                    file_output = "../output/filter_video.mp4";
                }
                stream_filter_graph.filter_video(file_output, fullPath);
            }
                break;
            case 11:
            {
                if (file_output.empty())
                {
                    file_output = "../output/cut_video.mp4";
                }
                stream_transfer.cut_video(file_output, fullPath, 10, 20);
            }
                break;
            case 12:
            {
                if (file_output.empty())
                {
                    file_output = "../output/remove_audio.mp4"; 
                }
                stream_transfer.remove_audio(file_output, fullPath);
            }
                break;
            case 100:
            {
                if (file_output.empty())
                {
                    file_output = "../output/i.yuv";      
                }
                stream_transfer.get_first_frame(file_output, fullPath, 1, 1);
            }
                break;
            case 101:
            {
                if (file_output.empty())
                {
                    file_output = "../output/i.png";      
                }
                stream_transfer.get_first_frame(file_output, fullPath, 1, 3);
            }
                break;
            default:
                break;
        }
    }
    else if (file_type == 2)
    {
        std::cout << "function type:"
            << "\n0-read_mp4_box"
            << std::endl;

        int function_type = 0;
        std::cin >> function_type;
        switch (function_type)
        {
            case 0:
                if (file_output.empty())
                {
                    file_output = "../output";      
                }
                read_stream.read_mp4_box(file_output, fullPath);
                break;
            default:
                break;
        }
    }
    else if (file_type == 3)
    {
        stream_display.compress_png("../output/i.png", "../fileDepends/images/i.png");
    }
    return 0;
}
