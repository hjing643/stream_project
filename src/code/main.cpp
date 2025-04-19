#include <iostream>
#include <string>
#include "stream_transfer.h"
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

    std::string fullPath = argv[1];

    std::cout << "function type:"
            << "\n0-analyze_file"
            << "\n1-deep analyze_file"
            << "\n2-format_raw_to_mp4"
            << "\n3-format_yuv_to_rgb"
            << "\n10-get_first_I_frame_to_yuv"
            << "\n11-get_first_P_frame_to_yuv"
            << "\n12-get_first_I_frame_to_rgb"
            << "\n13-get_first_P_frame_to_rgb"
            << "\n14-get_first_I_frame_to_png"
            << "\n15-get_first_P_frame_to_png"
            << std::endl;

    int function_type = 0;
    std::cin >> function_type;

    CStreamTransfer stream_transfer;

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
                file_output = "../output/frame.rgb";      
            }
            stream_transfer.format_yuv_to_rgb(file_output, fullPath);       
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
                file_output = "../output/p.yuv";      
            }
            stream_transfer.get_first_frame(file_output, fullPath, 2, 1);
        }
        break;
        case 12:
        {
            if (file_output.empty())
            {
                file_output = "../output/i.rgb";      
            }
            stream_transfer.get_first_frame(file_output, fullPath, 1, 2);
        }
        break;
        case 13:
        {
            if (file_output.empty())
            {
                file_output = "../output/p.rgb";      
            }
            stream_transfer.get_first_frame(file_output, fullPath, 2, 2);
        }
        break;
        case 14:
        {
            if (file_output.empty())
            {
                file_output = "../output/i.png";      
            }
            stream_transfer.get_first_frame(file_output, fullPath, 1, 3);
        }
        break;
        case 15:
        {
            if (file_output.empty())
            {
                file_output = "../output/p.png";      
            }
            stream_transfer.get_first_frame(file_output, fullPath, 2, 3);      
        }
        break;
        default:
            break;

    }
    
    return 0;
}
