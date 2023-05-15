#pragma once
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Model.h"
#include "engine.h"

void ProcessAssimpMesh(const aiScene* scene, aiMesh* mesh, Model* myModel, unsigned int baseMeshMaterialIndex, std::vector<unsigned int>& submeshMaterialIndices)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    bool hasTexCoords = false;
    bool hasTangentSpace = false;

    // process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        vertices.emplace_back(mesh->mVertices[i].x);
        vertices.emplace_back(mesh->mVertices[i].y);
        vertices.emplace_back(mesh->mVertices[i].z);
        vertices.emplace_back(mesh->mNormals[i].x);
        vertices.emplace_back(mesh->mNormals[i].y);
        vertices.emplace_back(mesh->mNormals[i].z);

        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            hasTexCoords = true;
            vertices.emplace_back(mesh->mTextureCoords[0][i].x);
            vertices.emplace_back(mesh->mTextureCoords[0][i].y);
        }

        if (mesh->mTangents != nullptr && mesh->mBitangents)
        {
            hasTangentSpace = true;
            vertices.emplace_back(mesh->mTangents[i].x);
            vertices.emplace_back(mesh->mTangents[i].y);
            vertices.emplace_back(mesh->mTangents[i].z);

            // For some reason ASSIMP gives me the bitangents flipped.
            // Maybe it's my fault, but when I generate my own geometry
            // in other files (see the generation of standard assets)
            // and all the bitangents have the orientation I expect,
            // everything works ok.
            // I think that (even if the documentation says the opposite)
            // it returns a left-handed tangent space matrix.
            // SOLUTION: I invert the components of the bitangent here.
            vertices.emplace_back(-mesh->mBitangents[i].x);
            vertices.emplace_back(-mesh->mBitangents[i].y);
            vertices.emplace_back(-mesh->mBitangents[i].z);
        }
    }

    // process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }

    // store the proper (previously proceessed) material for this mesh
    submeshMaterialIndices.emplace_back(baseMeshMaterialIndex + mesh->mMaterialIndex);

    // create the vertex format
    VertexBufferLayout vertexBufferLayout = {};
    vertexBufferLayout.AddAttribute<float>(new VertexBufferAttribute(0, 3));
    vertexBufferLayout.AddAttribute<float>(new VertexBufferAttribute(1, 3));
    if (hasTexCoords) vertexBufferLayout.AddAttribute<float>(new VertexBufferAttribute(2, 2));
    if (hasTangentSpace)
    {
        vertexBufferLayout.AddAttribute<float>(new VertexBufferAttribute(3, 3));
        vertexBufferLayout.AddAttribute<float>(new VertexBufferAttribute(4, 3));
    }

    vertexBufferLayout.Bound();


    // add the submesh into the mesh
    Mesh* m = new Mesh();
    m->vertexBufferLayout = vertexBufferLayout;
    m->vertexs.swap(vertices);
    m->indexs.swap(indices);
    myModel->meshes.emplace_back(m);
}

void ProcessAssimpMaterial(App* app, aiMaterial* material, Material* myMaterial, String directory)
{
    aiString name;
    aiColor3D diffuseColor;
    aiColor3D emissiveColor;
    aiColor3D specularColor;
    ai_real shininess;
    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
    material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
    material->Get(AI_MATKEY_SHININESS, shininess);

    myMaterial->name = name.C_Str();
    myMaterial->diffuse = vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
    myMaterial->emissive = vec3(emissiveColor.r, emissiveColor.g, emissiveColor.b);
    myMaterial->shininess = shininess / 256.0f;

    aiString aiFilename;
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial->diffuseTex = LoadTexture2D(app, filepath.str);
        myMaterial->properties.Set(aiTextureType_DIFFUSE, true);
    }
    if (material->GetTextureCount(aiTextureType_SPECULAR) > 0)
    {
        material->GetTexture(aiTextureType_SPECULAR, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial->specularTex = LoadTexture2D(app, filepath.str);
        myMaterial->properties.Set(aiTextureType_EMISSIVE, true);

    }
    if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0)
    {
        material->GetTexture(aiTextureType_EMISSIVE, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial->emissiveTex = LoadTexture2D(app, filepath.str);
        myMaterial->properties.Set(aiTextureType_EMISSIVE, true);
    }
    if (material->GetTextureCount(aiTextureType_NORMALS) > 0)
    {
        material->GetTexture(aiTextureType_NORMALS, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial->normalsTex = LoadTexture2D(app, filepath.str);
        myMaterial->properties.Set(aiTextureType_NORMALS, true);
    }
    if (material->GetTextureCount(aiTextureType_HEIGHT) > 0)
    {
        material->GetTexture(aiTextureType_HEIGHT, 0, &aiFilename);
        String filename = MakeString(aiFilename.C_Str());
        String filepath = MakePath(directory, filename);
        myMaterial->bumpTex = LoadTexture2D(app, filepath.str);
        myMaterial->properties.Set(aiTextureType_HEIGHT, true);
    }

    //myMaterial.createNormalFromBump();
}

void ProcessAssimpNode(const aiScene* scene, aiNode* node, Model* myModel, u32 baseMeshMaterialIndex, std::vector<u32>& submeshMaterialIndices)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessAssimpMesh(scene, mesh, myModel, baseMeshMaterialIndex, submeshMaterialIndices);
    }

    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessAssimpNode(scene, node->mChildren[i], myModel, baseMeshMaterialIndex, submeshMaterialIndices);
    }
}

Model* LoadModel(App* app, const char* filename)
{
    const aiScene* scene = aiImportFile(filename,
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_PreTransformVertices |
        aiProcess_ImproveCacheLocality |
        aiProcess_OptimizeMeshes |
        aiProcess_SortByPType);

    if (!scene)
    {
        ELOG("Error loading mesh %s: %s", filename, aiGetErrorString());
        return nullptr;
    }

    //app->meshes.push_back(Mesh{});
    //Mesh& mesh = app->meshes.back();
    //u32 meshIdx = (u32)app->meshes.size() - 1u;

    Model* m = new Model();
    app->objects.emplace_back(m);
    u32 modelIdx = (u32)app->objects.size() - 1u;

    String directory = GetDirectoryPart(MakeString(filename));
    GetFileName(&m->name, filename);
    

    // Create a list of materials
    u32 baseMeshMaterialIndex = (u32)app->materials.size();
    for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
    {
        Material* mat = new Material();
        app->materials.emplace_back(mat);
        ProcessAssimpMaterial(app, scene->mMaterials[i], mat, directory);
    }

    ProcessAssimpNode(scene, scene->mRootNode, m, baseMeshMaterialIndex, m->materials);

    aiReleaseImport(scene);

    u32 vertexBufferSize = 0;
    u32 indexBufferSize = 0;

    unsigned int size = m->meshes.size();
    for (u32 i = 0; i < size; ++i)
    {
        vertexBufferSize += m->meshes[i]->vertexs.size() * sizeof(float);
        indexBufferSize += m->meshes[i]->indexs.size() * sizeof(u32);
    }

    glGenBuffers(1, &m->vertexs);
    glBindBuffer(GL_ARRAY_BUFFER, m->vertexs);
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);

    glGenBuffers(1, &m->indexs);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->indexs);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, NULL, GL_STATIC_DRAW);

    u32 indicesOffset = 0;
    u32 verticesOffset = 0;

    size = m->meshes.size();
    for (u32 i = 0; i < size; ++i)
    {
        const void* verticesData = m->meshes[i]->vertexs.data();
        const u32   verticesSize = m->meshes[i]->vertexs.size() * sizeof(float);
        glBufferSubData(GL_ARRAY_BUFFER, verticesOffset, verticesSize, verticesData);
        m->meshes[i]->vertexOffset = verticesOffset;
        verticesOffset += verticesSize;

        const void* indicesData = m->meshes[i]->indexs.data();
        const u32   indicesSize = m->meshes[i]->indexs.size() * sizeof(u32);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, indicesOffset, indicesSize, indicesData);
        m->meshes[i]->indexsOffset = indicesOffset;
        indicesOffset += indicesSize;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return m;
}