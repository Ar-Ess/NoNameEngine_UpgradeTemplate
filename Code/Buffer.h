#pragma once

struct Buffer
{
    GLuint handle;
    GLenum type;
    u32 size;
    u32 head;
    void* data = nullptr;
};