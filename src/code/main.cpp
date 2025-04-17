#include <iostream>
#include <string>
#include "stream_transfer.h"
int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "please input file name" <<std::endl;
    }

    std::string fullPath = argv[1];

    std::cout << "function type: 0-analyze_file, 1-deep analyze_file, 2-format_raw_to_mp4" << std::endl;
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
            std::string file_output = "../output/mp4.mp4";
            stream_transfer.format_raw_to_mp4(file_output, fullPath);       
        }
            break;
        default:
            break;

    }

    

    
    return 0;
}
