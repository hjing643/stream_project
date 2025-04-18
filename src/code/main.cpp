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
            << "\n3-get_first_I_frame_to_yuv"
            << "\n4-get_first_P_frame_to_yuv"
            << "\n5-format_yuv_to_rgb"
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
                file_output = "../output/i.I";      
            }
            stream_transfer.get_first_frame_to_yuv(file_output, fullPath, 1);       
        }
        break;
        case 4:
        {
            if (file_output.empty())
            {
                file_output = "../output/p.P";      
            }
            stream_transfer.get_first_frame_to_yuv(file_output, fullPath, 2);       
        }
        break;
        case 5:
        {
            if (file_output.empty())
            {
                file_output = "../output/png.png";      
            }
            stream_transfer.format_yuv_to_rgb(file_output, fullPath);       
        }
        break;
        default:
            break;

    }
    
    return 0;
}
