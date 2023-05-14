#include "engine.h"
#include "TexturedQuad.h"
#include "Model.h"
//#include <imgui.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include "AssimpLoading.h"
#include "BufferManagement.h"

#define BINDING(b) b
#define ALIGN(value, alignment) (value + alignment - 1) & ~(alignment - 1)

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
    app->cam = new Camera(glm::vec3(0, 0, 10), app->displaySize.x/app->displaySize.y, 0.1, 1000);

    if (GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3))
        glDebugMessageCallback(OnGlError, app);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    app->cbuffer = CreateConstantBuffer(app->GetMaxUniformBlockSize());

    //app->InitTexturedQuad("dice.png", true);
    //app->InitTexturedQuad("color_white.png", false);
    //app->InitTexturedQuad("color_black.png", false);
    //app->InitTexturedQuad("color_normal.png", false);
    //app->InitTexturedQuad("color_magenta.png", false);
    app->InitModel("Patrick/Patrick.obj");
    app->InitModel("Primitives/Plane/Plane.obj", glm::vec3(0, -4, 0));

    app->AddDirectLight(glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
    //app->AddPointLight(glm::vec3(1, 1, 1), glm::vec3(0, 0, 5));
    //app->AddSpotLight(glm::vec3(1, 1, 1), glm::vec3(0, 2, 10), glm::vec3(0, -0.3, -1), 15);
}

void App::InitTexturedQuad(const char* texture, glm::vec3 position)
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
    quad->position = position;
    quad->UpdateTransform();

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
    // Quan bindejes a, i despr�s bindejes b, est�s posant b dins a.
    glBindBuffer(GL_ARRAY_BUFFER, quad->vertexs);
    // Dir-li com llegir aquesta data (pos dels vertex).
    // En aquest cas, 3 floats amb un total d'espai de "Vertex"
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // No offset
    // Activar aquesta manera de llegir (attribut)
    glEnableVertexAttribArray(0);
    // Dir-li com llegir aquesta data (uv dels vertex).
    // En aquest cas, 2 floats amb un total d'espai de "Vertex"
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // Avans t� 3 floats (pos)
    // Activar aquesta manera de llegir (attribut)
    glEnableVertexAttribArray(1);
    // Bindejar al vao la array d'elements (indexs)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad->indexs); // S s'ha de tancar, que normalment no cal, tancar-lo DESPR�S de tancar el grup (el vao)

    // Tancar el binding del vao (no cal tancar els de dins, ja es tenquen)
    glBindVertexArray(0);

    quad->program = quad->vao.program = LoadProgram(this, "TextureShader.glsl", "TEXTURED_GEOMETRY");
    
    Program& texturedGeometryProgram = *programs[quad->vao.program];
    quad->texUniform = glGetUniformLocation(texturedGeometryProgram.handle, "uTexture");

    quad->texture = LoadTexture2D(this, texture);

    objects.emplace_back(quad);

    quad->name = "Quad";
}

Object* App::InitModel(const char* path, glm::vec3 position)
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
    m->position = position;
    m->UpdateTransform();
    m->texUniform = texUniform;

    for (std::vector<Mesh*>::iterator it = m->meshes.begin(); it != m->meshes.end(); ++it)
        for (std::vector<Vao>::iterator ot = (*it)->vaos.begin(); ot != (*it)->vaos.end(); ++ot)
            (*ot).program = m->program;

    return m;
}

void App::AddPointLight(glm::vec3 color, glm::vec3 position)
{
    Light* l = new Light(LightType::LT_POINT, color, position, glm::vec3(), 0);
    lights.push_back(l);
    objects.push_back(l);
    l->name = "Point Light";
}

void App::AddDirectLight(glm::vec3 color, glm::vec3 direction)
{
    Light* l = new Light(LightType::LT_DIRECTIONAL, color, glm::vec3(), direction, 0);
    lights.push_back(l);
    objects.push_back(l);
    l->name = "Directional Light";
}

void App::AddSpotLight(glm::vec3 color, glm::vec3 position, glm::vec3 direction, float cutoff)
{
    Light* l = new Light(LightType::LT_SPOT, color, position, direction, cutoff);
    lights.push_back(l);
    objects.push_back(l);
    l->name = "Spot Light";
}

void App::DeleteObject(intptr_t selected)
{
    Object* o = nullptr;
    unsigned int index = 0;
    for (std::vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it)
    {
        o = (*it);
        if (o->id == selected) break;
        index++;
    }

    if (!o) return;

    if (o->Type() == ObjectType::O_LIGHT)
    {
        Light* l = (Light*)o;
        int lIndex = -1;
        for (std::vector<Light*>::iterator it = lights.begin(); it != lights.end(); ++it)
        {
            lIndex++;
            if ((*it)->id == l->id) break;
        }

        if (lIndex != -1) lights.erase(lights.begin() + lIndex);
    }

    objects.erase(objects.begin() + index);
    delete o;

}

void Update(App* app)
{
    app->GUI();

    ImGui::Render();

    //Clear input state if required by ImGui
    if (ImGui::GetIO().WantCaptureKeyboard)
        for (u32 i = 0; i < KEY_COUNT; ++i)
            app->input.keys[i] = BUTTON_IDLE;

    if (ImGui::GetIO().WantCaptureMouse)
        for (u32 i = 0; i < MOUSE_BUTTON_COUNT; ++i)
            app->input.mouseButtons[i] = BUTTON_IDLE;

    app->Input();

    app->cam->Update();

}

void App::GUI()
{
    // GUI
    static bool showFps = true;

    //ImGuiDockNodeFlags dockspace_flags = (ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoResize | ImGuiDockNodeFlags_PassthruCentralNode);
    //ImGuiViewport* viewport = ImGui::GetMainViewport();
    //ImGui::SetNextWindowPos(viewport->Pos);
    //ImGui::SetNextWindowSize(viewport->Size);
    //ImGui::SetNextWindowViewport(viewport->ID);
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    //ImGui::Begin("##Docking", (bool*)0, (ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus));

    //ImGui::PopStyleVar();

    //ImGuiIO& io = ImGui::GetIO();
    //if (io.ConfigFlags & (ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable))
    //{
    //    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    //    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspace_flags);
    //}

    //ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Config"))
        {
            ImGui::Text("Show FPS:"); ImGui::SameLine();
            ImGui::Checkbox("##sfps", &showFps);

            ImGui::PushItemWidth(65);
            ImGui::Text("Ambient:"); ImGui::SameLine();
            ImGui::DragFloat("##amb", &ambient, 0.01, 0, 1, "%.2f");
            ImGui::PopItemWidth();

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::BeginMenu("Models"))
            {
                if (ImGui::MenuItem("Patrick"))
                    InitModel("Patrick/Patrick.obj");

                if (ImGui::MenuItem("Plane"))
                    InitModel("Primitives/Plane/Plane.obj", glm::vec3(0, -4, 0));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights"))
            {
                if (ImGui::MenuItem("Directional"))
                    AddDirectLight(glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));

                if (ImGui::MenuItem("Point Light"))
                    AddPointLight(glm::vec3(1, 1, 1), glm::vec3(0, 0, 5));

                if (ImGui::MenuItem("Spot Light"))
                    AddSpotLight(glm::vec3(1, 1, 1), glm::vec3(0, 2, 10), glm::vec3(0, -0.3, -1), 15);

                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            if (ImGui::BeginMenu("OpenGL"))
            {
                ImGui::BulletText("Version   -> %s", openGLInformation.version);
                ImGui::BulletText("Renderer  -> %s", openGLInformation.renderer);
                ImGui::BulletText("Vendor    -> %s", openGLInformation.vendor);
                ImGui::BulletText("Num Extns -> %d", openGLInformation.numExtensions);
                if (ImGui::BeginMenu(" � Extensions"))
                {
                    for (std::vector<const char*>::iterator it = openGLInformation.extensions.begin(); it != openGLInformation.extensions.end(); ++it)
                        ImGui::Text("%s", (*it));
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        for (std::vector<Object*>::iterator it = objects.begin(); it != objects.end(); ++it)
        {
            Object* o = (*it);
            ImGui::PushID(o->id);
            ImGui::Checkbox("##active", &o->active);
            ImGui::SameLine();
            if (ImGui::Selectable(o->name.c_str(), selected == o->id, ImGuiSelectableFlags_AllowItemOverlap))
            {
                if (selected == o->id) selected = 0;
                else selected = o->id;
            }

            if (selected != o->id)
            {
                ImGui::PopID();
                continue;
            }

            ImGui::SameLine();
            if (ImGui::Button("X")) DeleteObject(o->id);

            ImGui::Separator();

            if (o->DrawGui()) o->UpdateTransform();

            ImGui::Separator();
            ImGui::Spacing();

            ImGui::PopID();
        }
    }
    ImGui::End();

    if (ImGui::Begin("##Info", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize))
    {
        if (ImGui::Button("Reload")) HotReload();
        if (showFps) ImGui::Text("FPS: %f", float(1.0f / deltaTime));
    }
    ImGui::End();

}

void App::Input()
{
    if (!input.Active()) return;

    if (input.GetKey(K_A)) 
        cam->Translate(-0.1, 0, 0);

    if (input.GetKey(K_D))
        cam->Translate( 0.1, 0, 0);

    if (input.GetKey(K_W))
        cam->Translate(0, 0,  0.1);

    if (input.GetKey(K_S))
        cam->Translate(0, 0, -0.1);

    if (input.GetMouseButton(LEFT))
        cam->LookAt(input.mouseDelta, 0.1);

}

void Render(App* app)
{
    glClearColor(0.1f, 0.1f, 0.1, 1.0f); // Asignar el color base
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Borrar color & depth buffer
    glViewport(0, 0, app->displaySize.x, app->displaySize.y); // Defineix viewport on renderitzar

    // Fill Uniform Buffer once per update
    BindBuffer(app->cbuffer);
    MapBuffer(app->cbuffer, GL_WRITE_ONLY);

    // -- Global Parameters
    app->globalParamsOffset = app->cbuffer.head;
    PushVec3(app->cbuffer, app->cam->Position());
    PushFloat(app->cbuffer, app->ambient);
    PushUInt(app->cbuffer, app->lights.size());

    for (std::vector<Light*>::iterator it = app->lights.begin(); it != app->lights.end(); ++it)
    {
        AlignHead(app->cbuffer, sizeof(glm::vec4));

        Light* l = (*it);
        PushUInt(app->cbuffer, (int)l->type);
        PushVec3(app->cbuffer, l->color);
        PushVec3(app->cbuffer, l->direction);
        PushVec3(app->cbuffer, l->position);
        PushFloat(app->cbuffer, l->Cutoff());
        PushFloat(app->cbuffer, l->OuterCuttoff());
        PushFloat(app->cbuffer, l->intensity);
        PushUInt(app->cbuffer, l->active);
    }

    app->globalParamsSize = app->cbuffer.head - app->globalParamsOffset;

    // -- Local Parameters
    u32 localParamsFullSize = 0;
    for (std::vector<Object*>::iterator it = app->objects.begin(); it != app->objects.end(); ++it)
    {
        Object* o = (*it);
        if (o->Type() == ObjectType::O_LIGHT) continue;

        AlignHead(app->cbuffer, app->GetUniformBlockAlignment());
        
        o->localParamsOffset = app->cbuffer.head;
        PushMat4(app->cbuffer, o->world);
        PushMat4(app->cbuffer, app->GlobalMatrix(o->world));
        if (o->Type() == ObjectType::O_MODEL)
        {
            Model* m = (Model*)o;
            PushVec3(app->cbuffer, m->material.ambient);
            PushVec3(app->cbuffer, m->material.diffuse);
            PushVec3(app->cbuffer, m->material.specular);
            PushFloat(app->cbuffer, m->material.bright);
        }

        o->localParamsSize = app->cbuffer.head - o->localParamsOffset;
        localParamsFullSize += o->localParamsSize;
    }

    UnmapBuffer(app->cbuffer);
    UnbindBuffer(app->cbuffer);

    BindBufferRange(app->cbuffer, BINDING(0), 0, app->globalParamsSize); // Binding Global Params

    ///////////////////

    for (std::vector<Object*>::iterator it = app->objects.begin(); it != app->objects.end(); ++it)
    {
        Object* o = (*it);
        if (!o->active) continue;

        switch (o->Type())
        {
        case ObjectType::O_TEXTURED_QUAD:
        {
            TexturedQuad* tQ = (TexturedQuad*)o;
            // Get & Set the program to be used
            glUseProgram(app->programs[tQ->vao.program]->handle);
            // Bind the vao vertex array
            glBindVertexArray(tQ->vao.handle);

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

            BindBufferRange(app->cbuffer, BINDING(1), o->localParamsOffset, o->localParamsSize); // Binding Local Params

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