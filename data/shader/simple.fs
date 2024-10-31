smooth in v4 fC;

out v4 C;

void main()
{
    // To sRGB.
    C = to_srgb(fC);
    if (C.a == 0.0f)
        discard;
}
