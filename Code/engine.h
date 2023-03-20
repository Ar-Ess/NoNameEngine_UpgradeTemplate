//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include <glad/glad.h>
#include <vector>

typedef glm::vec2  vec2;
typedef glm::vec3  vec3;
typedef glm::vec4  vec4;
typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;

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

struct Image
{
    void* pixels;
    ivec2 size;
    i32   nchannels;
    i32   stride;
};

struct Vertex
{
    Vertex(float x, float y, float z, float uvx, float uvy)
    {
        this->pos = vec3(x, y, z);
        this->uv = vec2(uvx, uvy);
    }

    Vertex(vec3 pos, vec2 uv)
    {
        this->pos = pos;
        this->uv =uv;
    }

    vec3 pos;
    vec2 uv;
};

struct Texture
{
    GLuint      handle;
    std::string filepath;
};

struct Program
{
    GLuint             handle;
    std::string        filepath;
    std::string        programName;
    u64                lastWriteTimestamp; // What is this for?
};

enum Mode
{
    Mode_TexturedQuad,
    Mode_Count
};

struct App
{
    // Loop
    f32  deltaTime;
    bool isRunning;

    // Input
    Input input;

    // Graphics
    OpenGLInfo openGLInformation;

    ivec2 displaySize;

    std::vector<Texture>  textures;
    std::vector<Program>  programs;

    // program indices
    u32 texturedGeometryProgramIdx;
    
    // texture indices
    u32 diceTexIdx;
    u32 whiteTexIdx;
    u32 blackTexIdx;
    u32 normalTexIdx;
    u32 magentaTexIdx;

    // Mode
    Mode mode;

    // Embedded geometry (in-editor simple meshes such as
    // a screen filling quad, a cube, a sphere...)
    GLuint vertexIDs[10];
    GLuint elementsIDs[10];

    // Location of the texture uniform in the textured quad shader
    GLuint programUniformTexture;

    // VAO object to link our screen filling quad with our textured quad shader
    GLuint vao;
};

void Init(App* app);

void Update(App* app);

void Render(App* app);

