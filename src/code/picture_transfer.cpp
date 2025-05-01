#include "picture_transfer.h"
#include "ffmepg_helper.h"
namespace stream_project
{
    class CAutoDestroyPictureTransfer
    {
    public:
        ~CAutoDestroyPictureTransfer()
        {
            if (codec_ctx_ != NULL)
            {
                avcodec_free_context(&codec_ctx_);
                codec_ctx_ = NULL;
            }
            if (frame_ != NULL)
            {
                av_frame_free(&frame_);
                frame_ = NULL;
            }
            if (packet_ != NULL)
            {
                av_packet_free(&packet_);
                packet_ = NULL;
            }
        }
        void set_codec_context(AVCodecContext* codec_ctx)
        {
            codec_ctx_ = codec_ctx;
        }
        void set_frame(AVFrame* frame)
        {
            frame_ = frame;
        }
        void set_packet(AVPacket* packet)
        {
            packet_ = packet;
        }
    private:
        AVCodecContext* codec_ctx_ = NULL;
        AVFrame* frame_ = NULL;
        AVPacket* packet_ = NULL;
    };

    int CPictureTransfer::init()
    {
        return 1;
    }

    int CPictureTransfer::transfer_raw_to_picture(const std::string& input_path, AVPixelFormat src_fmt, int width, int height, const std::string& output_path, AVCodecID dst_codec_id)
    {
        std::cout << "start transfer_raw_to_picture" 
                << "\ninput_path:" << input_path
                << "\noutput_path:" << output_path
                << "\nwidth:" << width
                << "\nheight:" << height
                << "src_fmt:\n" << src_fmt
                << "dst_codec_id:\n" << dst_codec_id
                << std::endl;

        if (dst_codec_id != AV_CODEC_ID_MJPEG
        && dst_codec_id != AV_CODEC_ID_PNG
        && dst_codec_id != AV_CODEC_ID_BMP)
        {
            std::cout << term_color::red << "dst_codec_id is not supported" << term_color::reset << dst_codec_id << std::endl;
            return -1;
        }

        std::ifstream in(input_path, std::ios::binary);
        std::ofstream out(output_path, std::ios::binary);
        if (!in.is_open() || !out.is_open())
        {
            std::cout << term_color::red << "failed to open file" << term_color::reset << input_path << " or " << output_path << std::endl;
            return -1;
        }

        int buffer_size = width * height * 3;
        char* buffer = new char[buffer_size];
        in.read(buffer, buffer_size);
        
        const AVCodec* codec = avcodec_find_encoder(dst_codec_id);
        if (!codec) 
        {
            std::cout << term_color::red << "not find codec" << term_color::reset << dst_codec_id << std::endl;
            return -1;
        }
        CAutoDestroyPictureTransfer auto_destroy_input;
        CAutoDestroyPictureTransfer auto_destroy_output;
        
        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
        auto_destroy_output.set_codec_context(codec_ctx);

        codec_ctx->bit_rate = 6*1024*1024;
        codec_ctx->width = width;
        codec_ctx->height = height;
        codec_ctx->time_base = (AVRational){1, 25};
        codec_ctx->pix_fmt = src_fmt;

        // YUVJ420P support jpeg
        // BGR24 support bmp
        // RGB24 support png
        if (dst_codec_id == AV_CODEC_ID_MJPEG) 
        {
            codec_ctx->pix_fmt = AV_PIX_FMT_YUVJ420P;
        } 
        else if (dst_codec_id == AV_CODEC_ID_BMP) 
        {
            codec_ctx->pix_fmt = AV_PIX_FMT_BGR24;
        } 
        else 
        {
            codec_ctx->pix_fmt = AV_PIX_FMT_RGB24;
        }

        AVFrame* dst_frame = av_frame_alloc();
        auto_destroy_output.set_frame(dst_frame);
        dst_frame->format = codec_ctx->pix_fmt;
        dst_frame->width = width;
        dst_frame->height = height;

        if(codec_ctx->pix_fmt != src_fmt)
        {
            std::cout << "codec_ctx->pix_fmt: " << codec_ctx->pix_fmt << ", src_fmt: " << src_fmt << std::endl;
            AVFrame* src_frame = av_frame_alloc();
            auto_destroy_input.set_frame(src_frame);

            src_frame->format = src_fmt;
            src_frame->width = width;
            src_frame->height = height;
            av_image_fill_arrays(src_frame->data, src_frame->linesize,
                                reinterpret_cast<const uint8_t*>(buffer), 
                                src_fmt, width, height, 1);

            // 3. 转换为目标格式
            av_frame_get_buffer(dst_frame, 32);
            SwsContext* sws_ctx = sws_getContext(width, height, src_fmt,
                                                width, height, codec_ctx->pix_fmt,
                                                SWS_BILINEAR, nullptr, nullptr, nullptr);

            sws_scale(sws_ctx,
                    src_frame->data, src_frame->linesize,
                    0, height,
                    dst_frame->data, dst_frame->linesize);

            sws_freeContext(sws_ctx);
        }
        else
        {
            // 将外部数据拷贝到 AVFrame（或你可以跳过复制，直接填入 frame->data）            
            av_image_fill_arrays(dst_frame->data, dst_frame->linesize,
                     reinterpret_cast<const uint8_t*>(buffer),  // 原始图像数据指针
                     src_fmt, width, height, 1);
        }


        if (avcodec_open2(codec_ctx, codec, NULL) < 0) 
        {
            delete[] buffer;
            std::cout << term_color::red << "open codec failed" << term_color::reset << std::endl;
            return -1;
        }

        // 4. 编码帧
        AVPacket* pkt = av_packet_alloc();
        auto_destroy_output.set_packet(pkt);
        if (!pkt) 
        {
            delete[] buffer;
            std::cout << term_color::red << "alloc packet failed" << term_color::reset << std::endl;
            return -1;
        }

        if (avcodec_send_frame(codec_ctx, dst_frame) < 0) 
        {
            delete[] buffer;
            std::cout << term_color::red << "send frame failed" << term_color::reset << std::endl;
            return -1;
        }

        if (avcodec_receive_packet(codec_ctx, pkt) == 0) 
        {
            std::cout << term_color::yellow << "pkt->size: " << pkt->size << term_color::reset << std::endl;
            // 5. 写入 PNG 文件
            out.write(reinterpret_cast<const char*>(pkt->data), pkt->size);
            av_packet_unref(pkt);
        }
        
        delete[] buffer;
        std::cout << term_color::yellow << "transfer_raw_to_picture finished" << term_color::reset << std::endl;
        return 1;
    }
}