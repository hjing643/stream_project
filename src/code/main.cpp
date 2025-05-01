#include <iostream>
#include <string>
#include "stream_transfer.h"
#include "rawstream_read.h"
#include "heicstream_read.h"
#include "picture_transfer.h"
int main(int argc, char* argv[])
{
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

    std::string fullPath = argv[1];

    std::cout <<"filetype:" 
        << "\ntest:0"
        << "\nstream_transcode:1"
        << "\nrawstream_read:2"
        << std::endl;

    int file_type = 0;
    std::cin >> file_type;
    if (file_type == 0)
    {
        if (file_output.empty())
        {
            file_output = "../output/heic.jpg";
        }
        heic_stream_read.transfer_heic_to_picture(file_output, fullPath, AV_CODEC_ID_MJPEG);
    }
    if (file_type == 1)
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
            << "\n10-get_first_I_frame_to_yuv"
            << "\n11-get_first_I_frame_to_png"
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
                    file_output = "../output/i.yuv";      
                }
                stream_transfer.get_first_frame(file_output, fullPath, 1, 1);
            }
                break;
            case 11:
            {
                if (file_output.empty())
                {
                    file_output = "../output/i.png";      
                }
                stream_transfer.get_first_frame(file_output, fullPath, 2, 1);
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
    
    return 0;
}
