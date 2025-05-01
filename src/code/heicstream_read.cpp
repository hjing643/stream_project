#include "heicstream_read.h"
#include "picture_transfer.h"
#include "ffmepg_helper.h"
namespace stream_project
{
    class CAutoDestroyHeicStreamRead
    {
        public:
            CAutoDestroyHeicStreamRead()
            {
            }
            ~CAutoDestroyHeicStreamRead()
            {
                if (this->ctx_)
                {
                    heif_context_free(this->ctx_);
                    this->ctx_ = nullptr;
                }
                if (this->handle_)
                {
                    heif_image_handle_release(this->handle_);
                    this->handle_ = nullptr;
                }
                if (this->img_)
                {
                    heif_image_release(this->img_);
                    this->img_ = nullptr;
                }
            }
            void set_ctx(heif_context* ctx)
            {
                this->ctx_ = ctx;   
            }
            void set_handle(heif_image_handle* handle)
            {
                this->handle_ = handle;
            }
            void set_img(heif_image* img)
            {
                this->img_ = img;
            }
        private:
            heif_context* ctx_ = nullptr;
            heif_image_handle* handle_ = nullptr;
            heif_image* img_ = nullptr;
    };

    // init the heic stream read
    int CHeicStreamRead::init()
    {
        
        return 1;
    }


    int CHeicStreamRead::transfer_heic_to_picture(const std::string& output_path, const std::string& heic_path, AVCodecID dst_codec_id)
    {
        init();
        CAutoDestroyHeicStreamRead auto_destroy_heic_stream_read;

        heif_context* ctx = heif_context_alloc();
        auto_destroy_heic_stream_read.set_ctx(ctx);
        if (!ctx) {
            std::cerr << term_color::red << "Failed to allocate heif_context\n" << term_color::reset;
            return -1;
        }

        heif_error err = heif_context_read_from_file(ctx, heic_path.c_str(), nullptr);
        if (err.code != heif_error_Ok) {
            std::cerr << term_color::red << "Failed to read HEIC: " << err.message << term_color::reset << "\n";
            return -1;
        }

        heif_image_handle* handle = nullptr;
        err = heif_context_get_primary_image_handle(ctx, &handle);
        if (err.code != heif_error_Ok) {
            std::cerr << term_color::red << "Failed to get primary image handle\n" << term_color::reset;
            return -1;
        }
        auto_destroy_heic_stream_read.set_handle(handle);

        int thumb_count = heif_image_handle_get_number_of_thumbnails(handle);
        std::cout << term_color::yellow << "thumb_count: " << thumb_count << term_color::reset << std::endl;


        heif_image* img;
        heif_decode_image(handle, &img, heif_colorspace_RGB, heif_chroma_interleaved_RGB, nullptr);
        auto_destroy_heic_stream_read.set_img(img);
        int stride;
        const uint8_t* data = heif_image_get_plane_readonly(img, heif_channel_interleaved, &stride);


        int width = heif_image_get_width(img, heif_channel_interleaved);
        int height = heif_image_get_height(img, heif_channel_interleaved);

        std::cout << term_color::yellow << "Image size: " << width << "x" << height << ", Stride: " << stride << term_color::reset << "\n";

        // 6. 写入完整的像素数据
        if (dst_codec_id == AV_CODEC_ID_RAWVIDEO)
        {
            std::ofstream out(output_path, std::ios::binary);
            if (!out.is_open()) {
                std::cerr << term_color::red << "Failed to open output file" << output_path << term_color::reset << std::endl;
                return -1;
            }
            // 逐行写入（处理可能的 stride 填充）
            for (int y = 0; y < height; ++y) {
                out.write(reinterpret_cast<const char*>(data + y * stride), width * 3); // RGB 24bit
            }
            out.close();
        }
        else
        {
            std::string tmp_path = "../output/tmp.rgb";
            std::ofstream out(tmp_path.c_str(), std::ios::binary);
            if (!out.is_open()) {
                std::cerr << term_color::red << "Failed to open output file" << tmp_path << term_color::reset << std::endl;
                return -1;
            }
            // 逐行写入（处理可能的 stride 填充）
            for (int y = 0; y < height; ++y) {
                out.write(reinterpret_cast<const char*>(data + y * stride), width * 3); // RGB 24bit
            }
            out.close();

            CPictureTransfer picture_transfer;
            picture_transfer.transfer_raw_to_picture(tmp_path, AV_PIX_FMT_RGB24, width, height, output_path, dst_codec_id);
            remove(tmp_path.c_str());
        }      

        std::cout << term_color::yellow << "read_heic_stream finished " << output_path << term_color::reset << "\n";
        return 1;
    }    
}