
struct V2
{
    f32 x, y;
};

struct V3
{
    f32 x, y, z;
};

inline V3
operator + (V3 a, V3 b)
{
    return V3{a.x + b.x, a.y + b.y, a.z + b.z};
}

inline V3
operator - (V3 a, V3 b)
{
    return V3{a.x - b.x, a.y - b.y, a.z - b.z};
}

union V4
{
    struct { f32 x, y, z, w; };
    struct { f32 r, g, b, a; };
};

struct M4x4
{
    f32 e[4][4];
};
