#pragma once

#include "platform.h"
#include <glad/glad.h>
#include "Image.h"
#include "Vertex.h"
#include "Buffer.h"
#include "Typedef.h"
#include "OpenGlInfo.h"
#include "FrameBuffer.h"
#include "BlurBuffer.h"
#include "Water.h"

class Texture;
class Program;
class Object;
class Material;
class TexturedQuad;
class Light;
class Camera;

class App
{
public:

    // Loop
    float deltaTime = 0;
    bool isRunning = false;

    // Input
    Input input;
    void Input();

    // Gui
    void GUI();

    // Render
    void RenderFrame();
    void RenderForward();
    void RenderDeferred();
    void RenderWater();
    void RenderBloom();
    void HotReload();

    // Graphics
    OpenGLInfo openGLInformation;
    ivec2 displaySize = {0, 0};

    // Vectors
    std::vector<Texture*>  textures;
    std::vector<Program*>  programs;
    std::vector<Object*>   objects;
    std::vector<Water*>    waters;
    std::vector<Material*> materials;
    std::vector<Light*>    lights;

    intptr_t selected = 0;
    TexturedQuad* InitTexturedQuad(const char* texture, glm::vec3 position = glm::vec3(0.f));
    void InitModel(const char* path, glm::vec3 position = glm::vec3(0.f), float scale = 1);
    void InitWater(glm::vec3 position = glm::vec3(0.f), float scale = 1);
    Light* AddPointLight(glm::vec3 color, glm::vec3 position);
    Light* AddDirectLight(glm::vec3 color, glm::vec3 direction);
    Light* AddSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cutoff);
    void DeleteObject(intptr_t selected);
    void ActivateBloom(bool active);
    bool globalBloom = true;

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
    FrameBuffer gBuffer; // TODO: Create a GeometryBuffer class and remove Final Attachment inside
    Buffer      forwardConstBuffer;
    Buffer      deferredGConstBuffer;
    Buffer      deferredLConstBuffer;
    Buffer      waterConstBuffer;
    BlurBuffer  blurBuffer;
    TexturedQuad* frameQuad = nullptr;

    // Camera
    glm::mat4 GlobalMatrix(glm::mat4 world);
    Camera* cam = nullptr;
    glm::mat4 global = glm::mat4(1.0f);

    // Configuration
    bool deferred = true;
    float ambient = 0.1;
    int currentRenderTarget = 0;
    const char* renderTargets[10] = {"FINAL", "SPECULAR", "NORMALS", "POSITION", "ALBEDO", "LIGHT", "BLOOM", "DEPTH", "WATERTEST", "WATERTEST2"};
    // LIGHT
    float threshold = 1;
    bool blackwhite = false;
    // DEPTH
    float depthNear = 0.1;
    float depthFar = 100;

    GLuint CurrentRenderTarget()
    {
        GLuint ret;

        switch (currentRenderTarget)
        {
            default: ret = frameBuffer.finalAttachHandle; break;
            case 1:  ret = frameBuffer.specularAttachHandle; break;
            case 2:  ret = frameBuffer.normalsAttachHandle; break;
            case 3:  ret = frameBuffer.positionAttachHandle; break;
            case 4:  ret = frameBuffer.albedoAttachHandle; break;
            case 5:  ret = frameBuffer.lightAttachHandle; break;
            case 6:  ret = blurBuffer.attachment[0]; break;
            case 7:  ret = frameBuffer.depthAttachHandle; break;
            case 8:  ret = waters[0]->buffer.albedoAttachHandle[WBT_REFLECTION];
            case 9:  ret = waters[0]->buffer.albedoAttachHandle[WBT_REFRACTION];
        }

        return ret;
    }

};

void Init(App* app);

void Update(App* app);

void Render(App* app);

u32 LoadTexture2D(App* app, const char* filepath);

void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);