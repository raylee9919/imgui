
#include "render.h"

static void
push_rect_to_batch(V3 cen, V2 half_dim, V4 color,
          std::vector<Render_Element> &batch, Memory_Arena *arena)
{
    Render_Element e;
    e.type = Render_Type::RECT;
    e.data = push_struct(arena, Render_Rect);

    Render_Rect *data = (Render_Rect *)e.data;
    data->cen = cen;
    data->half_dim = half_dim;
    data->color = color;

    batch.push_back(e);
}

