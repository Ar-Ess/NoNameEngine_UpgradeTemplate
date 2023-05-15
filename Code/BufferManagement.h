#pragma once

bool IsPowerOf2(u32 value)
{
    return value && !(value & (value - 1));
}

u32 Align(u32 value, u32 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

Buffer CreateBuffer(u32 size, GLenum type, GLenum usage)
{
    Buffer buffer = {};
    buffer.size = size;
    buffer.type = type;

    glGenBuffers(1, &buffer.handle);
    glBindBuffer(type, buffer.handle);
    glBufferData(type, buffer.size, NULL, usage);
    glBindBuffer(type, 0);

    return buffer;
}

#define CreateConstantBuffer(size) CreateBuffer(size, GL_UNIFORM_BUFFER, GL_STREAM_DRAW)
#define CreateStaticVertexBuffer(size) CreateBuffer(size, GL_ARRAY_BUFFER, GL_STATIC_DRAW)
#define CreateStaticIndexBuffer(size) CreateBuffer(size, GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW)

void BindBuffer(const Buffer& buffer)
{
    glBindBuffer(buffer.type, buffer.handle);
}

void UnbindBuffer(const Buffer& buffer)
{
    glBindBuffer(buffer.type, 0);
}

void MapBuffer(Buffer& buffer, GLenum access)
{
    glBindBuffer(buffer.type, buffer.handle);
    buffer.data = (u8*)glMapBuffer(buffer.type, access);
    buffer.head = 0;
}

void UnmapBuffer(Buffer& buffer)
{
    glUnmapBuffer(buffer.type);
    glBindBuffer(buffer.type, 0);
}

u32 BindBufferRange(Buffer& buffer, GLuint bind, u32 blockOffset, u32 blockSize)
{
    glBindBufferRange(buffer.type, bind, buffer.handle, blockOffset, blockSize);
    blockOffset += blockSize;
    return blockOffset;
}

void AlignHead(Buffer& buffer, u32 alignment)
{
    ASSERT(IsPowerOf2(alignment), "The alignment must be a power of 2");
    buffer.head = Align(buffer.head, alignment);
}

void PushAlignedData(Buffer& buffer, const void* data, u32 size, u32 alignment)
{
    ASSERT(buffer.data != NULL, "The buffer must be mapped first");
    AlignHead(buffer, alignment);
    memcpy((u8*)buffer.data + buffer.head, data, size);
    buffer.head += size;
}

FrameBuffer CreateFrameBuffer(ivec2 display)
{
    FrameBuffer buffer;
    buffer.colorAttachHandle = CreateFrameBufferAttachement(GL_RGBA, display);
    buffer.depthAttachHandle = CreateFrameBufferAttachement(GL_DEPTH_COMPONENT, display);
    InitFrameBuffer(buffer);

    return buffer;
}

GLuint CreateFrameBufferAttachement(GLuint format, ivec2 display)
{
    if (format != GL_RGBA || format != GL_DEPTH_COMPONENT) assert(false, "Function CreateFrameBufferAttachement(): ATTACHEMENT FORMAT NOT YET IMPLEMENTED");

    GLuint handle = 0;
    GLuint internalFormat = format == GL_RGBA ? GL_RGBA8 : GL_DEPTH_COMPONENT24;
    GLuint type = format == GL_RGBA ? GL_UNSIGNED_BYTE : GL_FLOAT;

    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, display.x, display.y, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    return handle;
}

void InitFrameBuffer(FrameBuffer& buffer)
{
    GLuint handle = 0;
    glGenFramebuffers(1, &handle);
    glBindFramebuffer(GL_FRAMEBUFFER, handle);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.colorAttachHandle, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, buffer.depthAttachHandle, 0);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        switch (status)
        {
        case GL_FRAMEBUFFER_UNDEFINED:                     ELOG("GL_FRAMEBUFFER_UNDEFINEd                    "); break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:         ELOG("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT        "); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: ELOG("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"); break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:        ELOG("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER       "); break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:        ELOG("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER       "); break;
        case GL_FRAMEBUFFER_UNSUPPORTED:                   ELOG("GL_FRAMEBUFFER_UNSUPPORTED                  "); break;
        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:        ELOG("GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE       "); break;
        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:      ELOG("GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS     "); break;
        default: ELOG("Unknoen framebuffer status error"); break;
        }
    }

    glDrawBuffers(1, &buffer.colorAttachHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    buffer.handle = handle;
}

#define PushData(buffer, data, size) PushAlignedData(buffer, data, size, 1)
#define PushUInt(buffer, value) { u32 v = value; PushAlignedData(buffer, &v, sizeof(v), 4); }
#define PushFloat(buffer, value) { float v = value; PushAlignedData(buffer, &v, sizeof(v), 4); }
#define PushVec3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushVec4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushMat3(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
#define PushMat4(buffer, value) PushAlignedData(buffer, value_ptr(value), sizeof(value), sizeof(vec4))
