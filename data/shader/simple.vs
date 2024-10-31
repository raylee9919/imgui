layout(location = 0) in v3 vP;
layout(location = 1) in v4 vC;

uniform m4x4  P;

smooth out v4 fC;

void main()
{
    fC = vC;

    gl_Position = P * v4(vP, 1.0f);
}
