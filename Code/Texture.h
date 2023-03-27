#pragma once
#include <string>

struct Texture
{
    ~Texture()
    {
        filepath.clear();
    }

    unsigned int handle;
    std::string filepath;
};
