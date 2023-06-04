#pragma once
#include <string>
#include "VertexShaderAttribute.h"

typedef std::vector<VertexShaderAttribute*> VertexShaderLayout;

class Program
{
public:

    ~Program()
    {
        filepath.clear();
        programName.clear();
    }

    unsigned int handle;
    std::string  filepath;
    std::string  programName;
    unsigned long long int lastWriteTimestamp;
    VertexShaderLayout attributes;

};