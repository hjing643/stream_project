#include "read_stream.h"

int CReadStream::init()
{
    return 0;
}

uint32_t swap_big_to_little(uint32_t big) {
    return ((big >> 24) & 0x000000FF) |
           ((big >> 8)  & 0x0000FF00) |
           ((big << 8)  & 0x00FF0000) |
           ((big << 24) & 0xFF000000);
}

int CReadStream::read_mp4_box(const std::string& box_path, const std::string& video_path)
{
    FILE* file = fopen(video_path.c_str(), "rb");
    if (!file)
    {
        std::cerr << "Failed to open file: " << video_path << std::endl;
        return -1;
    }
    int box_index = 1;
    int naul_index = 1;

    while (true)
    {
        if (box_index > 50)
        {            
            std::cerr << "box number is more than 50: " << box_index << std::endl;

            break;
        }

        uint32_t box_size = 0;
        fread(&box_size, 4, 1, file);
        box_size = swap_big_to_little(box_size);
        if (box_size == 0 || box_size > 0x00ffffff)
        {
            std::cerr << "box size error: " << box_size << std::endl;
            break;
        }
        char sz_box_name[5] = {0};
        fread(sz_box_name, 1, 4, file);

        char sz_file_box_name[128] = {0};
        sprintf(sz_file_box_name, "%s/box_%d_%s", box_path.c_str(), box_index++, sz_box_name);

        FILE* file_box = fopen(sz_file_box_name, "wb");
        if (!file_box)
        {
            std::cerr << "Failed to open file: " << sz_file_box_name << std::endl;
            return -1;
        }
        if (box_size <= 8)
        {
            std::cerr << "box_size error " << box_size << std::endl;
            continue;
        }
        std::cerr << "box_size:" << box_size << std::endl;

        char* buffer = new char[box_size];
        memset(buffer, 0, box_size);
        fread(buffer, 1, box_size-8, file);
        fwrite(buffer, 1, box_size-8, file_box);

        if (strcmp(sz_box_name, "mdat") == 0)
        {
            std::cout << "mdat box found" << std::endl;
            uint32_t naul_size = 0;
            memcpy(&naul_size, buffer, 4);
            naul_size = swap_big_to_little(naul_size);
            int seek_pos = 4;
            while (naul_size > 0 && naul_size < box_size)
            {
                if (naul_index > 100)
                {
                    std::cerr << "naul number is more than 100: " << naul_index << std::endl;
                    break;
                }
                std::cout << "naul_size:" << naul_size << std::endl;
                char sz_file_naul_name[128] = {0};
                int naul_header = buffer[seek_pos];
                sprintf(sz_file_naul_name, "%s/naul/naul_%d_%x", box_path.c_str(), naul_index++, naul_header);
                FILE* file_naul = fopen(sz_file_naul_name, "wb");
                if (!file_naul)
                {
                    std::cerr << "Failed to open file: " << sz_file_naul_name << std::endl;
                    break;
                }
                fwrite("\x00\x00\x00\x01", 4, 1, file_naul);
                fwrite(buffer+seek_pos, 1, naul_size, file_naul);
                fclose(file_naul);
                seek_pos += naul_size;      

                memcpy(&naul_size, buffer + seek_pos, 4);
                naul_size = swap_big_to_little(naul_size);
                seek_pos += 4;
            }
        }

        delete[] buffer;
        fclose(file_box);
    }
    fclose(file);
    return 0;
}