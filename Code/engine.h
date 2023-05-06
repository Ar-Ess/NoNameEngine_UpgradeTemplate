//
// engine.h: This file contains the types and functions relative to the engine.
//

#pragma once

#include "platform.h"
#include "Object.h"
#include "Material.h"
#include "Texture.h"
#include "Program.h"
#include "Camera.h"
#include "Light.h"
#include "Image.h"
#include "Vertex.h"
#include "Buffer.h"

#include "Typedef.h"

struct Buffer;

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

struct App
{
    // Loop
    float deltaTime = 0;
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
    std::vector<Object*>  objects;
    std::vector<Material*> materials;

    void InitTexturedQuad(const char* texture, bool draw = true);
    void InitMesh(const char* path, bool draw = true);
    void AddLight(LightType type, glm::vec3 color, glm::vec3 position, glm::vec3 direction = glm::vec3(0, -1, 0));
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

    GLuint uniformBufferHandle = 0;
    intptr_t selected = 0;

    std::vector<Light*> lights;

    glm::mat4 GlobalMatrix(glm::mat4 world)
    {
        global = cam->projection * cam->view * world;
        return global;
    }

    Camera* cam = nullptr;
    glm::mat4 global = glm::mat4(1.0f);
    u32 globalParamsOffset = 0;
    u32 globalParamsSize = 0;
    Buffer cbuffer;
};

void Init(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);