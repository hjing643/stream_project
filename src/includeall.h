#pragma once
#include<iostream>
#include<string>
#include<map>
#include <fstream>
//#include<atomic> // my company linux and g++ is too old

//ffmpeg
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
#include <libavutil/intreadwrite.h>
//filtergraph
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersrc.h>
#include <libavfilter/buffersink.h>
}