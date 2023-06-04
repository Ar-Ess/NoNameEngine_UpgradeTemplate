#pragma once
#include "Typedef.h"

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};