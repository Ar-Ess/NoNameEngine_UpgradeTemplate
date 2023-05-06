#pragma once

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};