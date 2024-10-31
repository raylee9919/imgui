
struct V2
{
    f32 x, y;
};

inline V2
operator + (V2 a, V2 b)
{
    return V2{a.x + b.x, a.y + b.y};
}

inline V2
operator - (V2 a, V2 b)
{
    return V2{a.x - b.x, a.y - b.y};
}

inline V2
operator * (V2 a, f32 b)
{
    return V2{a.x * b, a.y * b};
}

inline V2
operator * (f32 b, V2 a)
{
    return V2{a.x * b, a.y * b};
}

inline V2 &
operator += (V2 &a, V2 b)
{
    a.x += b.x;
    a.y += b.y;
    return a;
}


struct V3
{
    union {
        struct {
            V2 xy;
            f32 z;
        };
        struct {
            f32 x, y, z;
        };
    };
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


struct Rect2
{
    V2 min;
    V2 max;
};


