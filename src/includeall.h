#pragma once
#include<iostream>
#include<string>
#include<map>
#include <fstream>
#include <vector>
#include <optional>
//#include<atomic> // my company linux and g++ is too old

#include <libheif/heif.h>
#include <libheif/heif_cxx.h>

#include <opencv2/opencv.hpp>
//ffmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavutil/intreadwrite.h>
#include <libavutil/imgutils.h>
//filtergraph
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
//#include <libavfilter/avfiltergraph.h>
}