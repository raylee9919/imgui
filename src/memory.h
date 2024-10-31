struct Memory_Arena
{
    void *base;
    size_t size;
    size_t used;
};

#define push_array(ARENA, STRUCT, COUNT) push_size(ARENA, COUNT*sizeof(STRUCT))
#define push_struct(ARENA, STRUCT) (STRUCT *)push_size(ARENA, sizeof(STRUCT))
static void *
push_size(Memory_Arena *arena, size_t size)
{
    void *result;
    assert(arena->used + size < arena->size);
    result = (u8 *)arena->base + arena->used;
    arena->used += size;
    return result;
}
