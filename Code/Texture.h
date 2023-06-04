#pragma once
#include <string>

class Texture
{
public:

    ~Texture()
    {
        filepath.clear();
    }

    unsigned int handle;
    std::string filepath;
};
