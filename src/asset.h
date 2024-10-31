#pragma pack(push, 1)

struct BMP_Info_Header 
{
    u16 filetype;
    u32 filesize;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 bitmap_info_header_size;
    s32 width;
    s32 height;
    s16 plane;
    u16 bpp; // bits
    u32 compression;
    u32 image_size;
    u32 h_resolution;
    u32 v_resolution;
    u32 plt_entry_cnt;
    u32 important;

    u32 r_mask;
    u32 g_mask;
    u32 b_mask;
};

struct Bitmap 
{
    s32     width;
    s32     height;
    s32     pitch;
    u32     handle;
    size_t  size;
    void    *memory;
};

#pragma pack(pop)
