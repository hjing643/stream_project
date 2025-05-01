#include "rawstream_read.h"

namespace stream_project
{
    uint32_t swap_big_to_little(uint32_t big) {
        return ((big >> 24) & 0x000000FF) |
            ((big >> 8)  & 0x0000FF00) |
            ((big << 8)  & 0x00FF0000) |
            ((big << 24) & 0xFF000000);
    }

    bool is_start_code3(const uint8_t* p) 
    {
        return p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x01;
    }

    bool is_start_code4(const uint8_t* p) 
    {
        return p[0] == 0x00 && p[1] == 0x00 && p[2] == 0x00 && p[3] == 0x01;
    }

    int CRawStreamRead::init()
    {
        return 1;
    }

    int CRawStreamRead::read_mp4_box(const std::string& box_path, const std::string& video_path)
    {
        FILE* file = fopen(video_path.c_str(), "rb");
        if (!file)
        {
            std::cerr << "Failed to open file: " << video_path << std::endl;
            return -1;
        }
        int box_index = 1;
        int naul_index = 1;

        int box_max_size = 20;
        int nalu_max_size = 50;
        while (true)
        {
            if (box_index > box_max_size)
            {            
                std::cerr << "box number is more than " << box_max_size << std::endl;

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
                    if (naul_index > nalu_max_size)
                    {
                        std::cerr << "naul number is more than " << nalu_max_size << std::endl;
                        break;
                    }
                    std::cout << "naul_size:" << naul_size << std::endl;
                    char sz_file_naul_name[128] = {0};
                    char naul_header = buffer[seek_pos];
                    int f = naul_header & 0x80;
                    int nri = (naul_header >> 5) & 0x03;
                    int type = naul_header & 0x1f;
                    std::string str_type;
                    switch (type)
                    {
                        case 1:
                            str_type = "NonIDR";
                            break;
                        case 5:
                            str_type = "IDR";
                            break;
                        case 6:
                            str_type = "SEI";
                            break;
                        case 7:
                            str_type = "SPS";
                            break;
                        case 8:
                            str_type = "PPS";
                            break;
                        case 9:
                            str_type = "AUD";
                            break;
                        default:
                            str_type = "unknown";
                            break;
                    }

                    if (f != 0)
                    {
                        sprintf(sz_file_naul_name, "%s/nalu%d_error", box_path.c_str(), naul_index++);
                    }
                    else
                    {
                        sprintf(sz_file_naul_name, "%s/nalu%d_%d_%s", box_path.c_str(), naul_index++, nri, str_type.c_str());
                    }
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
        return 1;
    }

    

    int CRawStreamRead::read_h264_nalu(const std::string& naul_path, const std::string& video_path)
    {
        std::ifstream file(video_path.c_str(), std::ios::binary);
        if (!file)
        {
            std::cerr << "Failed to open file: " << video_path << std::endl;
            return -1;
        }
        
        
        const size_t chunk_size = 4096;
        std::vector<uint8_t> chunk(chunk_size);

        int naul_index = 1;
        while (file.read((char*)chunk.data(), chunk_size) || file.gcount() > 0) 
        {
            size_t bytes_read = file.gcount();
            std::vector<uint8_t> data(chunk.begin(), chunk.begin() + bytes_read);

            size_t pos = 0;
            while (pos + 4 < data.size()) 
            {
                size_t start = 0;
                size_t next_start = 0;

                // 找起始码
                if (is_start_code4(&data[pos])) 
                {
                    start = pos + 4;
                    pos += 4;
                } else if (is_start_code3(&data[pos])) 
                {
                    start = pos + 3;
                    pos += 3;
                } else 
                {
                    pos++;
                    continue;
                }

                // 找下一个起始码作为结束点
                next_start = pos;
                while (next_start + 4 < data.size()) {
                    if (is_start_code3(&data[next_start]) || is_start_code4(&data[next_start]))
                        break;
                    next_start++;
                }

                if (start >= data.size()) 
                {
                    break;
                }

                uint8_t nalu_header = data[start];

                int f = nalu_header & 0x80; // f should be 0 forbidden_zero_bit
                int nri = (nalu_header >> 5) & 0x03; // 0~3， low to high
                int type = nalu_header & 0x1f;

                std::string str_type;
                switch (type)
                {
                    case 1:
                        str_type = "NonIDR";
                        break;
                    case 5:
                        str_type = "IDR";
                        break;
                    case 6:
                        str_type = "SEI";
                        break;
                    case 7:
                        str_type = "SPS";
                        break;
                    case 8:
                        str_type = "PPS";
                        break;
                    case 9:
                        str_type = "AUD";
                        break;
                    default:
                        str_type = "unknown";
                        break;
                }
                                    
                char sz_file_naul_name[128] = {0};

               if (f != 0)
                {
                    sprintf(sz_file_naul_name, "%s/nalu%d_error", naul_path.c_str(), naul_index++);
                }
                else
                {
                    sprintf(sz_file_naul_name, "%s/nalu%d_%d_%s", naul_path.c_str(), naul_index++, nri, str_type.c_str());
                }
                FILE* file_naul = fopen(sz_file_naul_name, "wb");
                if (!file_naul)
                {
                    std::cerr << "Failed to open file: " << sz_file_naul_name << std::endl;
                    break;
                }
                fwrite("\x00\x00\x00\x01", 4, 1, file_naul);
                fwrite(&data[start], 1, next_start-start, file_naul);
                fclose(file_naul);

                pos = next_start;
            }
            
        }
        file.close();
        return 1;
    }
}