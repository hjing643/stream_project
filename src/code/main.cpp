#include <iostream>
#include "stream_transfer.h"
int main()
{
    std::cout<<"hello world"<<std::endl;
    CStreamTransfer stream_transfer;
    stream_transfer.format_to_mp4("./output/", "./fileDepends/video/raw.h264");
    return 0;
}
