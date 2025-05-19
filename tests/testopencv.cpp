#include "gtest/gtest.h"
#include "../src/code/stream_display.h"

TEST(testopencv, test_compress_png)
{
    stream_project::CStreamDisplay stream_display;
    int ret1 = stream_display.compress_png("/home/gene/mnt-231-ubunto/output/i_compress.png", "../fileDepends/pics/i.png");
    EXPECT_EQ(ret1, 1);
}

TEST(testopencv, test_png_to_jpg)
{
    stream_project::CStreamDisplay stream_display;
    int ret1 = stream_display.png_to_jpg("/home/gene/mnt-231-ubunto/output/i_compress.jpg", "../fileDepends/pics/i.png");
    EXPECT_EQ(ret1, 1);
}

