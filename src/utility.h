#define offset_of(STRUCT, MEMBER) (&((STRUCT *)0)->MEMBER)
#define array_count(ARR) (sizeof(ARR) / sizeof(ARR[0]))

struct Buffer
{
    u8 *contents;
    size_t size;
};

internal Buffer
read_entire_file_and_null_terminate(const char *filepath)
{
    Buffer result = {};

    FILE *file = fopen(filepath, "rb");
    if (file)
    {
        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);

        result.contents = (u8 *)malloc(size + 1);
        if (result.contents)
        {
            fread(result.contents, size, 1, file);
            result.contents[size] = 0;
            result.size = size + 1;
        }
        else
        {
            printf("ERROR: malloc() fail.\n");
        }
    }
    else
    {
        printf("ERROR: Couldn't open file %s.\n", filepath);
    }

    return result;
}

