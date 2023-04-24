//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "Object.h"
#include "Material.h"
#include "Texture.h"
#include "Program.h"

#include "Typedef.h"

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

struct App
{
    // Loop
    float deltaTime;
    bool isRunning;

    // Input
    Input input;
    void Input();

    // Gui
    void GUI();

    // Graphics
    OpenGLInfo openGLInformation;

    ivec2 displaySize;

    std::vector<Texture*> textures;
    std::vector<Program*> programs;
    std::vector<Object*> objects;
    std::vector<Material*> materials;

    void InitTexturedQuad(const char* texture, bool draw = true);
    void InitMesh(const char* path, bool draw = true);
    void HotReload();

    GLint GetMaxUniformBlockSize() const
    {
        GLint a = 0;
        glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &a);
        return a;
    }
    GLint GetUniformBlockAlignment() const
    {
        GLint a = 0;
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &a);
        return a;
    }

    GLuint uniformBufferHandle;
    glm::mat4 worldMatrix;
    glm::mat4 worldViewProjectionMatrix;

    void Translate(float x, float y, float z)
    {
        glm::translate(worldMatrix, glm::vec3(x, y, z));
    }
    void Translate(glm::vec3 delta)
    {
        glm::translate(worldMatrix, delta);
    }
};

void Init(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);