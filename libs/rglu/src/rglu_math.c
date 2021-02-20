#include "rglu_math.h"

const float deg2Rad(const float deg)
{
    return (deg * 2.0f * PI) / 360.0f;
}

const float rad2Deg(const float rad)
{
    return rad * 180.0f / PI; 
}
