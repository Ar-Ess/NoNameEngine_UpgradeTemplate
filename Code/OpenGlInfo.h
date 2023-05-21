#pragma once

struct OpenGLInfo
{
    OpenGLInfo()
    {

    }

    OpenGLInfo(const char* version, const char* renderer, const char* vendor, unsigned int numExtensions)
    {
        this->version = version;
        this->renderer = renderer;
        this->vendor = vendor;
        this->numExtensions = numExtensions;
    }

    const char* version = nullptr;
    const char* renderer = nullptr;
    const char* vendor = nullptr;
    unsigned int numExtensions = 0;
    std::vector<const char*> extensions;
};
