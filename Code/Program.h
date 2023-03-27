#pragma once
#include <string>

struct Program
{
    ~Program()
    {
        filepath.clear();
        programName.clear();
    }

    unsigned int handle;
    std::string  filepath;
    std::string  programName;
    unsigned long long int lastWriteTimestamp;
};