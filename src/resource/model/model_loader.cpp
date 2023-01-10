#include "model_loader.h"
#include "resource/texture/texture_loader.h"
#include "platform/file/file_manager.h"
#include "platform/string.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>
#include <stdexcept>

void ProcessModel(const aiScene* scene, const aiNode* node, Model* model);
void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, MeshResource* out_mesh);

Model ModelLoader::Load(const Char* path)
{
    std::vector<byte> data = FileManager::Read(path);

    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFileFromMemory(data.data(), data.size(), aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(importer.GetErrorString());
    }

    Model model;
    model.Path = path;

    ProcessModel(scene, scene->mRootNode, &model);

    return model;
}


void ProcessModel(const aiScene* scene, const aiNode* node, Model* model)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        model->Meshes.push_back({});
        ProcessMesh(scene, mesh, model, &model->Meshes.back());
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessModel(scene, node->mChildren[i], model);
    }
}

void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, MeshResource* out_mesh)
{
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        RawVertex vertex;
        vertex.Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertex.Normal   = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};
        if (mesh->HasTextureCoords(0))
        {
            vertex.TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        }

        out_mesh->Vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            out_mesh->Indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiTextureType type          = aiTextureType_BASE_COLOR;
        aiMaterial*   material      = scene->mMaterials[mesh->mMaterialIndex];
        size_t        texture_count = material->GetTextureCount(type);

        for (size_t i = 0; i < texture_count; i++)
        {
            aiString str;
            material->GetTexture(type, i, &str);

            std::filesystem::path path = model->Path;
            path                       = path.parent_path() / str.C_Str();

            out_mesh->Textures.push_back(TextureLoader::LoadResource(path.string().c_str()));
        }
    }
}
