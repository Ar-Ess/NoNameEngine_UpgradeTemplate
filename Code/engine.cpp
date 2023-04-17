#include "engine.h"
#include "TexturedQuad.h"
#include "Model.h"
#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include "AssimpLoading.h"

#define BINDING(b) b

GLuint CreateProgramFromSource(String programSource, const char* shaderName)
{
    GLchar  infoLogBuffer[1024] = {};
    GLsizei infoLogBufferSize = sizeof(infoLogBuffer);
    GLsizei infoLogSize;
    GLint   success;

    char versionString[] = "#version 430\n";
    char shaderNameDefine[128];
    sprintf(shaderNameDefine, "#define %s\n", shaderName);
    char vertexShaderDefine[] = "#define VERTEX\n";
    char fragmentShaderDefine[] = "#define FRAGMENT\n";

    const GLchar* vertexShaderSource[] = {
        versionString,
        shaderNameDefine,
        vertexShaderDefine,
        programSource.str
    };
    const GLint vertexShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(vertexShaderDefine),
        (GLint) programSource.len
    };
    const GLchar* fragmentShaderSource[] = {
        versionString,
        shaderNameDefine,
        fragmentShaderDefine,
        programSource.str
    };
    const GLint fragmentShaderLengths[] = {
        (GLint) strlen(versionString),
        (GLint) strlen(shaderNameDefine),
        (GLint) strlen(fragmentShaderDefine),
        (GLint) programSource.len
    };

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vshader, ARRAY_COUNT(vertexShaderSource), vertexShaderSource, vertexShaderLengths);
    glCompileShader(vshader);
    glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with vertex shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fshader, ARRAY_COUNT(fragmentShaderSource), fragmentShaderSource, fragmentShaderLengths);
    glCompileShader(fshader);
    glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fshader, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glCompileShader() failed with fragment shader %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vshader);
    glAttachShader(programHandle, fshader);
    glLinkProgram(programHandle);
    glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(programHandle, infoLogBufferSize, &infoLogSize, infoLogBuffer);
        ELOG("glLinkProgram() failed with program %s\nReported message:\n%s\n", shaderName, infoLogBuffer);
    }

    glUseProgram(0);

    glDetachShader(programHandle, vshader);
    glDetachShader(programHandle, fshader);
    glDeleteShader(vshader);
    glDeleteShader(fshader);

    return programHandle;
}

u32 LoadProgram(App* app, const char* filepath, const char* programName)
{
    String programSource = ReadTextFile(filepath);

    Program program = {};
    program.handle = CreateProgramFromSource(programSource, programName);
    program.filepath = filepath;
    program.programName = programName;
    program.lastWriteTimestamp = GetFileLastWriteTimestamp(filepath);
    app->programs.emplace_back(new Program(program));

    return app->programs.size() - 1;
}

Image LoadImage(const char* filename)
{
    Image img = {};
    stbi_set_flip_vertically_on_load(true);
    img.pixels = stbi_load(filename, &img.size.x, &img.size.y, &img.nchannels, 0);
    if (img.pixels)
    {
        img.stride = img.size.x * img.nchannels;
    }
    else
    {
        ELOG("Could not open file %s", filename);
    }
    return img;
}

void FreeImage(Image image)
{
    stbi_image_free(image.pixels);
}

GLuint CreateTexture2DFromImage(Image image)
{
    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat     = GL_RGB;
    GLenum dataType       = GL_UNSIGNED_BYTE;

    switch (image.nchannels)
    {
        case 3: dataFormat = GL_RGB; internalFormat = GL_RGB8; break;
        case 4: dataFormat = GL_RGBA; internalFormat = GL_RGBA8; break;
        default: ELOG("LoadTexture2D() - Unsupported number of channels");
    }

    GLuint texHandle;
    glGenTextures(1, &texHandle);
    glBindTexture(GL_TEXTURE_2D, texHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, image.size.x, image.size.y, 0, dataFormat, dataType, image.pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texHandle;
}

u32 LoadTexture2D(App* app, const char* filepath)
{
    for (u32 texIdx = 0; texIdx < app->textures.size(); ++texIdx)
        if (app->textures[texIdx]->filepath == filepath)
            return texIdx;

    Image image = LoadImage(filepath);

    if (image.pixels)
    {
        Texture tex = {};
        tex.handle = CreateTexture2DFromImage(image);
        tex.filepath = filepath;

        u32 texIdx = app->textures.size();
        app->textures.emplace_back(new Texture(tex));

        FreeImage(image);
        return texIdx;
    }
    else
    {
        return UINT32_MAX;
    }
}

void Init(App* app)
{
    app->worldMatrix = glm::mat4(1);
    app->worldViewProjectionMatrix = glm::mat4(1);

    if (GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3))
        glDebugMessageCallback(OnGlError, app);

    glGenBuffers(1, &app->uniformBufferHandle);
    glBindBuffer(GL_UNIFORM_BUFFER, app->uniformBufferHandle);
    glBufferData(GL_UNIFORM_BUFFER, app->GetMaxUniformBlockSize(), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    //app->InitTexturedQuad("dice.png", false);
    //app->InitTexturedQuad("color_white.png", false);
    //app->InitTexturedQuad("color_black.png", false);
    //app->InitTexturedQuad("color_normal.png", false);
    //app->InitTexturedQuad("color_magenta.png", false);
    app->InitMesh("Patrick/Patrick.obj", true);
}

void App::InitTexturedQuad(const char* texture, bool draw)
{
    const Vertex vertex[] = {
        Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
        Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
        Vertex(0.5f,  0.5f, 0.0f, 1.0f, 1.0f),
        Vertex(-0.5f,  0.5f, 0.0f, 0.0f, 1.0f)
    };

    const u16 index[] = {
        0, 1, 2,
        0, 2, 3
    };

    TexturedQuad* quad = new TexturedQuad();
    quad->draw = draw;

    // Generar buffer i et retorna id
    glGenBuffers(1, &quad->vertexs);
    // Bind del buffer per editarlo
    glBindBuffer(GL_ARRAY_BUFFER, quad->vertexs);
    // Afegir data al buffer sobre els vertex
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
    // Desbindejar el buffer per no seguir editant-lo
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generar buffer i et retorna id
    glGenBuffers(1, &quad->indexs);
    // Bind del buffer per editarlo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->indexs);
    // Afegir data al buffer sobre els index
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);
    // Desbindejar el buffer per no seguir editant-lo
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // Generar vertex array i et retorna id
    glGenVertexArrays(1, &quad->vao.handle);
    // Bind el vertex array per editarlo
    glBindVertexArray(quad->vao.handle);
    // Bind el buffer dels vertex per editarlo i colocarlo dins del vao
    // Quan bindejes a, i després bindejes b, estàs posant b dins a.
    glBindBuffer(GL_ARRAY_BUFFER, quad->vertexs);
    // Dir-li com llegir aquesta data (pos dels vertex).
    // En aquest cas, 3 floats amb un total d'espai de "Vertex"
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // No offset
    // Activar aquesta manera de llegir (attribut)
    glEnableVertexAttribArray(0);
    // Dir-li com llegir aquesta data (uv dels vertex).
    // En aquest cas, 2 floats amb un total d'espai de "Vertex"
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Avans té 3 floats (pos)
    // Activar aquesta manera de llegir (attribut)
    glEnableVertexAttribArray(1);
    // Bindejar al vao la array d'elements (indexs)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->indexs); // S s'ha de tancar, que normalment no cal, tancar-lo DESPRÉS de tancar el grup (el vao)

    // Tancar el binding del vao (no cal tancar els de dins, ja es tenquen)
    glBindVertexArray(0);

    quad->program = quad->vao.program = LoadProgram(this, "TextureShader.glsl", "TEXTURED_GEOMETRY");
    
    Program& texturedGeometryProgram = *programs[quad->vao.program];
    quad->texUniform = glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");

    quad->texture = LoadTexture2D(this, texture);

    objects.emplace_back(quad);
}

void App::InitMesh(const char* path, bool draw)
{
    u32 program = LoadProgram(this, "MeshShader.glsl", "TEXTURED_GEOMETRY");

    Program* p = programs[program];
    GLuint texUniform = glGetUniformLocation(p->handle, "uTexture");

    GLsizei size = 0;
    glGetProgramiv(p->handle, GL_ACTIVE_ATTRIBUTES, &size);
    for (unsigned int i = 0; i < size; ++i)
    {
        char attribName[200] = {};
        GLsizei attribLength = 0;
        GLint attribSize = 0;
        GLenum attribType = 0;
        glGetActiveAttrib(p->handle, i, ARRAY_COUNT(attribName), &attribLength, &attribSize, &attribType, attribName);

        p->attributes.emplace_back(new VertexShaderAttribute(glGetAttribLocation(p->handle, attribName), attribSize));
    }

    Model* m = LoadModel(this, path);
    m->program = program;
    m->draw = draw;
    m->texUniform = texUniform;

    for (std::vector<Mesh*>::iterator it = m->meshes.begin(); it != m->meshes.end(); ++it)
        for (std::vector<Vao>::iterator ot = (*it)->vaos.begin(); ot != (*it)->vaos.end(); ++ot)
            (*ot).program = m->program;

}

void Update(App* app)
{
    static bool showFps = true;

    ImGuiDockNodeFlags dockspace_flags = (ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_PassthruCentralNode);
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("##Docking", (bool*)0, (ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus));

    ImGui::PopStyleVar();

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & (ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable))
    {
        ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Config"))
        {
            ImGui::Checkbox("Show FPS", &showFps);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::BeginMenu("OpenGL"))
            {
                ImGui::BulletText("Version   -> %s", app->openGLInformation.version);
                ImGui::BulletText("Renderer  -> %s", app->openGLInformation.renderer);
                ImGui::BulletText("Vendor    -> %s", app->openGLInformation.vendor);
                ImGui::BulletText("Num Extns -> %d", app->openGLInformation.numExtensions);
                if (ImGui::BeginMenu(" · Extensions"))
                {
                    for (std::vector<const char*>::iterator it = app->openGLInformation.extensions.begin(); it != app->openGLInformation.extensions.end(); ++it)
                        ImGui::Text("%s", (*it));
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("##Info", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus))
    {
        ImGui::SetWindowSize(ImVec2(app->displaySize.x, app->displaySize.y));
        if (ImGui::Button("Reload")) app->HotReload();
        if (showFps) ImGui::Text("FPS: %f", float(1.0f / app->deltaTime));

        ImGui::End();
    }


}

void Render(App* app)
{
    // Asignar el color base
    glClearColor(0.1f, 0.1f, 0.1, 1.0f);
    // Borrar el buffer de color i el buffer de profunditat
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Defineix el viewport on es renderitza tot
    glViewport(0, 0, app->displaySize.x, app->displaySize.y);

    // Fill Uniform Buffer once per update
    glBindBuffer(GL_UNIFORM_BUFFER, app->uniformBufferHandle);

    u8* bufferData = (u8*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    u32 bufferHead = 0;

    memcpy(bufferData + bufferHead, glm::value_ptr(app->worldMatrix), sizeof(glm::mat4));
    bufferHead += sizeof(glm::mat4);

    memcpy(bufferData + bufferHead, glm::value_ptr(app->worldViewProjectionMatrix), sizeof(glm::mat4));
    bufferHead += sizeof(glm::mat4);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    u32 blockOffset = 0;
    u32 blockSize = sizeof(glm::mat4) * 2;
    glBindBufferRange(GL_UNIFORM_BUFFER, BINDING(1), app->uniformBufferHandle, blockOffset, blockSize);

    ///////////////////

    for (std::vector<Object*>::iterator it = app->objects.begin(); it != app->objects.end(); ++it)
    {
        Object* o = (*it);
        if (!o->draw) continue;

        switch (o->Type())
        {
        case ObjectType::O_TEXTURED_QUAD:
        {
            TexturedQuad* tQ = (TexturedQuad*)o;
            // Get & Set the program to be used
            glUseProgram(app->programs[tQ->vao.program]->handle);
            // Bind the vao vertex array
            glBindVertexArray(tQ->vao.handle);

            // Enable Blend render mode
            glEnable(GL_BLEND);
            // Enable alpha function for blend
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Send the texture as uniform variable to glsl script
            glUniform1i(o->texUniform, 0);
            // Activate slot for a texture
            glActiveTexture(GL_TEXTURE0);


            // Bind the texture of the dice
            glBindTexture(GL_TEXTURE_2D, app->textures[tQ->texture]->handle);

            // Draw the elements to the screen
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

            // Unbind the vertex array
            glBindVertexArray(0);
            // Unuse the program used
            glUseProgram(0);

            break;
        }

        case ObjectType::O_MODEL:
        {
            Model* m = (Model*)o;
            glUseProgram(app->programs[m->program]->handle);

            unsigned int size = m->meshes.size();
            for (u32 i = 0; i < size; ++i)
            {
                GLuint vao = m->FindVAO(i, app->programs[m->program]);
                glBindVertexArray(vao);

                Material* mat = app->materials[m->materials[i]];

                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, app->textures[mat->albedoTex]->handle);
                glUniform1i(m->texUniform, 0);

                Mesh* mesh = m->meshes[i];
                glDrawElements(GL_TRIANGLES, mesh->indexs.size(), GL_UNSIGNED_INT, (void*)(u64)mesh->indexsOffset);

                glBindVertexArray(0);
            }

            glUseProgram(0);

            break;
        }

        default: break;
        }
    }
}

void App::HotReload()
{
    for (std::vector<Program*>::iterator it = programs.begin(); it != programs.end(); ++it)
    {
        Program& p = *(*it);
        u64 currTimestamp = GetFileLastWriteTimestamp(p.filepath.c_str());
        if (currTimestamp <= p.lastWriteTimestamp) continue;

        glDeleteProgram(p.handle);
        p.handle = CreateProgramFromSource(ReadTextFile(p.filepath.c_str()), p.programName.c_str());
        p.lastWriteTimestamp = currTimestamp;
    }
}

void OnGlError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    ELOG("OpenGL debug message: %s", message);

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             ELOG(" - source: GL_DEBUG_SOURCE_API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   ELOG(" - source: GL_DEBUG_SOURCE_WINDOW_SYSTEM"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: ELOG(" - source: GL_DEBUG_SOURCE_SHADER_COMPILER"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     ELOG(" - source: GL_DEBUG_SOURCE_THIRD_PARTY    "); break;
    case GL_DEBUG_SOURCE_APPLICATION:     ELOG(" - source: GL_DEBUG_SOURCE_APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER:           ELOG(" - source: GL_DEBUG_SOURCE_OTHER"); break;
    default: break;
    }

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               ELOG(" - type: GL_TYPE_ERROR"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: ELOG(" - type: GL_TYPE_DEPRECATED_BEHAVIOR"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  ELOG(" - type: GL_TYPE_UNDEFINED_BEHAVIOR"); break;
    case GL_DEBUG_TYPE_PORTABILITY:         ELOG(" - type: GL_TYPE_PORTABILITY"); break;
    case GL_DEBUG_TYPE_PERFORMANCE:         ELOG(" - type: GL_TYPE_PERFORMANCE"); break;
    case GL_DEBUG_TYPE_MARKER:              ELOG(" - type: GL_TYPE_MARKER"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP:          ELOG(" - type: GL_TYPE_PUSH_GROUP"); break;
    case GL_DEBUG_TYPE_POP_GROUP:           ELOG(" - type: GL_TYPE_POP_GROUP"); break;
    case GL_DEBUG_TYPE_OTHER:               ELOG(" - type: GL_TYPE_OTHER"); break;
    default: break;
    }

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:   ELOG(" - severity: GL_DEBUG_SEVERITY_HIGH:"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: ELOG(" - severity: GL_DEBUG_SEVERITY_MEDIUM"); break;
    case GL_DEBUG_SEVERITY_LOW:    ELOG(" - severity: GL_DEBUG_SEVERITY_LOW:"); break;
    default: break;
    }

    ELOG("______________________________________________");
}