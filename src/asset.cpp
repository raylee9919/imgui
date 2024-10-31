#include "asset.h"

// IMPORTANT: bitmap->memory starts with lowest scanline of the image.
internal Bitmap *
load_bmp(const char *filename)
{
    Bitmap *result = push_struct(arena, Bitmap);
    *result = {};
    
    Buffer read = read_entire_file_and_null_terminate(filename);
    if (read.contents) 
    {
        BMP_Info_Header *header = (BMP_Info_Header *)read.contents;
        u32 *pixels = (u32 *)((u8 *)read.contents + header->bitmap_offset);

        result->memory  = pixels + header->width * (header->height - 1);
        result->width   = header->width;
        result->height  = header->height;
        result->pitch   = result->width * 4;
        result->size    = result->width * result->height * 4;
        result->handle  = 0;

        Assert(header->compression == 3);

        u32 r_mask = header->r_mask;
        u32 g_mask = header->g_mask;
        u32 b_mask = header->b_mask;
        u32 a_mask = ~(r_mask | g_mask | b_mask);        
        
        Bit_Scan_Result r_scan = find_least_significant_set_bit(r_mask);
        Bit_Scan_Result g_scan = find_least_significant_set_bit(g_mask);
        Bit_Scan_Result b_scan = find_least_significant_set_bit(b_mask);
        Bit_Scan_Result a_scan = find_least_significant_set_bit(a_mask);
        
        Assert(r_scan.found);
        Assert(g_scan.found);   
        Assert(b_scan.found);
        Assert(a_scan.found);

        s32 r_shift = (s32)r_scan.index;
        s32 g_shift = (s32)g_scan.index;
        s32 b_shift = (s32)b_scan.index;
        s32 a_shift = (s32)a_scan.index;

        f32 inv_255f = 1.0f / 255.0f;
        
        u32 *at = pixels;
        for(s32 y = 0;
            y < header->height;
            ++y)
        {
            for(s32 x = 0;
                x < header->width;
                ++x)
            {
                u32 c = *at;

                f32 r = (f32)((c & r_mask) >> r_shift);
                f32 g = (f32)((c & g_mask) >> g_shift);
                f32 b = (f32)((c & b_mask) >> b_shift);
                f32 a = (f32)((c & a_mask) >> a_shift);

                f32 ra = a * inv_255f;
#if 1
                r *= ra;
                g *= ra;
                b *= ra;
#endif

                *at++ = (((u32)(a + 0.5f) << 24) |
                         ((u32)(r + 0.5f) << 16) |
                         ((u32)(g + 0.5f) <<  8) |
                         ((u32)(b + 0.5f) <<  0));
            }
        }
        result->memory = pixels;
    }


    return result;
}
