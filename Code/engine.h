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
#include "OpenGlInfo.h"
#include "FrameBuffer.h"
//jaume
#include "ReflectionBuffer.h"
#include "RefractionBuffer.h"

class TexturedQuad;
class Light;

class App
{
public:

    // Loop
    float deltaTime = 0;
    bool isRunning;

    // Input
    Input input;
    void Input();

    // Gui
    void GUI();

    // Render
    void RenderForward();
    void RenderDeferred();
    void RenderDeferred2();
    void HotReload();

    // Graphics
    OpenGLInfo openGLInformation;
    ivec2 displaySize;

    // Vectors
    std::vector<Texture*> textures;
    std::vector<Program*> programs;
    std::vector<Object*>  objects;
    std::vector<Material*> materials;
    std::vector<Light*> lights;


    intptr_t selected = 0;
    TexturedQuad* InitTexturedQuad(const char* texture, bool forward, glm::vec3 position = glm::vec3(0.f));
    void InitModel(const char* path, ObjectType objectType, glm::vec3 position = glm::vec3(0.f), float scale = 1);
    Light* AddPointLight(glm::vec3 color, glm::vec3 position);
    Light* AddDirectLight(glm::vec3 color, glm::vec3 direction);
    Light* AddSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cutoff);
    void DeleteObject(intptr_t selected);
  
    // Getters
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

    // Buffers
    FrameBuffer frameBuffer;
    FrameBuffer gBuffer;
    Buffer      forwardConstBuffer;
    Buffer      deferredGConstBuffer;
    Buffer      deferredLConstBuffer;
    TexturedQuad* frameQuad = nullptr;
    TexturedQuad* deferredQuad = nullptr;

    //water render targets
    GLuint Reflection;
    GLuint Refraction;
    GLuint ReflectionDepth;
    GLuint RefractionDepth;

    ReflectionBuffer BufferReflection;
    RefractionBuffer frameBufferRefraction;



    // Camera
    glm::mat4 GlobalMatrix(glm::mat4 world)
    {
        global = cam->projection * cam->view * world;
        return global;
    }
    Camera* cam = nullptr;
    glm::mat4 global = glm::mat4(1.0f);

    // Configuration
    bool deferred = true;
    float ambient = 0.1;
    int currentRenderTarget = 0;
    const char* renderTargets[6] = {"FINAL", "SPECULAR", "NORMALS", "POSITION", "ALBEDO", "DEPTH"};
    float depthNear = 0.1;
    float depthFar = 100;

    GLuint CurrentRenderTarget(bool geometryDepth)
    {
        GLuint ret;

        FrameBuffer* buffer = geometryDepth ? &gBuffer : &frameBuffer;

        switch (currentRenderTarget)
        {
            default: ret = frameBuffer.finalAttachHandle; break;
            case 1:  ret = frameBuffer.specularAttachHandle; break;
            case 2:  ret = frameBuffer.normalsAttachHandle; break;
            case 3:  ret = frameBuffer.positionAttachHandle; break;
            case 4:  ret = frameBuffer.albedoAttachHandle; break;
            case 5:  ret = buffer->depthAttachHandle; break;
        }

        return ret;
    }

};

void Init(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);