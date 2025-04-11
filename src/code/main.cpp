#include <iostream>
#include <string>
#include "stream_transfer.h"
int main()
{
    std::cout<<"hello world"<<std::endl;
    CStreamTransfer stream_transfer;
    std::string file_output = "./output/output1.mp4";
    stream_transfer.format_to_mp4(file_output, "../fileDepends/video/raw.h264");

    FileFormat file_format;
    stream_transfer.analyze_file(file_format, "../fileDepends/video/raw.h264");
    return 0;
}
