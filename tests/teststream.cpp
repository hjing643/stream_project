#include "gtest/gtest.h"
#include "../src/includeall.h"
#include "../src/code/stream_transfer.h"
#include "../src/code/stream_filtergraph.h"
#include "../src/code/heicstream_read.h"

TEST(teststream, test_analyze_file)
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.analyze_file("../fileDepends/videos/h264.h264", false);
    int ret2 = stream_transfer.analyze_file("../fileDepends/videos/person1.mp4", false);
    int ret3 = stream_transfer.analyze_file("../fileDepends/videos/person1.webm", false);

    EXPECT_EQ(ret1, 1);
    EXPECT_EQ(ret2, 1);
    EXPECT_EQ(ret3, 1);
}
TEST(teststream, test_transfer)
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.format_raw_to_mp4("/home/gene/mnt-231-ubunto/output/h264.mp4", "../fileDepends/videos/h264.h264");
    int ret2 = stream_transfer.format_mp4_to_raw("/home/gene/mnt-231-ubunto/output/person1.h264", "../fileDepends/videos/person1.mp4");
    int ret3 = stream_transfer.format_raw_to_avi("/home/gene/mnt-231-ubunto/output/h264.avi", "../fileDepends/videos/h264.h264");
    int ret4 = stream_transfer.format_webm_to_mp4("/home/gene/mnt-231-ubunto/output/mp4_from_webm.mp4", "../fileDepends/videos/person1.webm");

    EXPECT_EQ(ret1, 1);
    EXPECT_EQ(ret2, 1);
    EXPECT_EQ(ret3, 1);
    EXPECT_EQ(ret4, 1);
}

TEST(teststream, test_change_resolution_fps)
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.change_resolution("/home/gene/mnt-231-ubunto/output/720P.mp4", "../fileDepends/videos/person1.mp4", 1280, 720);
    int ret2 = stream_transfer.change_fps("/home/gene/mnt-231-ubunto/output/15fps.mp4", "../fileDepends/videos/person1.mp4");   

    EXPECT_EQ(ret1, 1);
    EXPECT_EQ(ret2, 1);
}

TEST(teststream, test_get_first_frame)  
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.get_first_frame("/home/gene/mnt-231-ubunto/output/I.png", "../fileDepends/videos/person1.mp4", 1, 3);
    int ret2 = stream_transfer.get_first_frame("/home/gene/mnt-231-ubunto/output/P.png", "../fileDepends/videos/person1.mp4", 2, 3);

    EXPECT_EQ(ret1, 1);
    EXPECT_EQ(ret2, 1);
}

TEST(teststream, test_cut_video)
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.cut_video("/home/gene/mnt-231-ubunto/output/cut_video.mp4", "../fileDepends/videos/vehicle1.mp4", 10, 20);
    EXPECT_EQ(ret1, 1);
}

TEST(teststream, test_remove_audio)
{
    stream_project::CStreamTransfer stream_transfer;
    int ret1 = stream_transfer.remove_audio("/home/gene/mnt-231-ubunto/output/remove_audio.mp4", "../fileDepends/videos/vehicle1.mp4");
    EXPECT_EQ(ret1, 1);
}

TEST(teststream, test_filter_video)
{
    stream_project::CStreamFilterGraph stream_filter_graph;
    int ret1 = stream_filter_graph.filter_video("/home/gene/mnt-231-ubunto/output/filter_graph.mp4", "../fileDepends/videos/vehicle1.mp4");
    EXPECT_EQ(ret1, 1);
}
TEST(teststream, test_transfer_heic)
{
    stream_project::CHeicStreamRead stream_heic_read;
    int ret1 = stream_heic_read.transfer_heic_to_picture("/home/gene/mnt-231-ubunto/output/1.jpg", "../fileDepends/videos/1.HEIC", AV_CODEC_ID_MJPEG);
    int ret2 = stream_heic_read.transfer_heic_to_picture("/home/gene/mnt-231-ubunto/output/1.png", "../fileDepends/videos/1.HEIC", AV_CODEC_ID_PNG);
    EXPECT_EQ(ret1, 1);
    EXPECT_EQ(ret2, 1);
}
