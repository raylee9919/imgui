

enum Render_Type
{
    RECT,
};
struct Render_Element
{
    Render_Type type;
    void *data;
};
struct Render_Rect
{
    V3 cen;
    V2 half_dim;
    V4 color;
};

