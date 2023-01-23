#include "render_resource_loader.h"
#include "core/log/log_system.h"
#include "resource/asset/asset_manager.h"
#include "platform/file/file_manager.h"

#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

std::shared_ptr<TextureData> RenderResourceLoader::LoadTexture(const String& file_path, bool is_srgb)
{
    std::shared_ptr<TextureData> cached_asset = Registry->GetAsset<TextureData>(file_path);
    if (cached_asset)
    {
        return cached_asset;
    }

    std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

    std::vector<byte> data = FileManager::Read(file_path.c_str());

    int   channels;
    texture->Pixels = stbi_load_from_memory(
        data.data(), data.size(), (int*)&texture->Width, (int*)&texture->Height, &channels, STBI_rgb_alpha);
    if (!texture)
    {
        throw std::runtime_error("failed to load texture!");
    }

    texture->Depth     = 1;
    texture->ArrayLayers = 1;
    texture->MipLevels = std::floor(std::log2(std::max(texture->Width, texture->Height))) + 1;
    texture->Format      = is_srgb ? RHIFormat::eR8G8B8A8Srgb : RHIFormat::eR8G8B8A8Unorm;

    Registry->Register(file_path, texture);

    return texture;
}

std::shared_ptr<TextureData> RenderResourceLoader::LoadTextureHDR(const String& file_path, int desired_channels)
{
    std::shared_ptr<TextureData> cached_asset = Registry->GetAsset<TextureData>(file_path);
    if (cached_asset)
    {
        return cached_asset;
    }

    std::shared_ptr<TextureData> texture = std::make_shared<TextureData>();

    std::vector<byte> data = FileManager::Read(file_path.c_str());

    int channels;
    texture->Pixels = stbi_load_from_memory(
        data.data(), data.size(), (int*)&texture->Width, (int*)&texture->Height, &channels, STBI_rgb_alpha);
    if (!texture)
    {
        throw std::runtime_error("failed to load texture!");
    }

    texture->Depth       = 1;
    texture->ArrayLayers = 1;
    texture->MipLevels   = std::floor(std::log2(std::max(texture->Width, texture->Height))) + 1;
    switch (desired_channels)
    {
        case 2:
            texture->Format = RHIFormat::eR32G32Sfloat;
            break;
        case 4:
            texture->Format = RHIFormat::eR32G32B32A32Sfloat;
            break;
        default:
            throw std::runtime_error("unsupported channels number");
            break;
    }

    Registry->Register(file_path, texture);

    return texture;
}

std::shared_ptr<RenderMaterialData> RenderResourceLoader::LoadMaterialData(const MaterialResource& material_res)
{
    std::shared_ptr<RenderMaterialData> material = std::make_shared<RenderMaterialData>();

    material->BaseColorTexture         = LoadTexture(material_res.BaseColorFile, true);
    material->NormalTexture            = LoadTexture(material_res.NormalFile);
    material->MetallicRoughnessTexture = LoadTexture(material_res.MetallicRoughnessFile);
    material->OcclusionTexture         = LoadTexture(material_res.OcclusionFile);
    material->EmissiveTexture          = LoadTexture(material_res.EmissiveFile);

    return material;
}

std::shared_ptr<RenderMeshData> RenderResourceLoader::LoadMeshData(const MeshResource&   mesh_res,
                                                                   const AxisAlignedBox& bounding_box)
{
    std::shared_ptr<RenderMeshData> cached_asset = Registry->GetAsset<RenderMeshData>(mesh_res.MeshFile);
    if (cached_asset)
    {
        return cached_asset;
    }

    std::shared_ptr<RenderMeshData> mesh = std::make_shared<RenderMeshData>();
    mesh->MeshData                       = LoadStaticMesh(mesh_res.MeshFile, bounding_box);

    Registry->Register(mesh_res.MeshFile, mesh);

    return mesh;
}

struct Model
{
    struct SubMesh
    {
        std::vector<MeshVertexData> Vertices;
        std::vector<uint32_t>       Indices;
        std::vector<String>         Textures;
    };

    std::vector<SubMesh> Meshes;

    SubMesh MergeSubMesh() {
        SubMesh merged_mesh;

        uint32_t index_offset = 0;
        for (SubMesh& sub_mesh : Meshes)
        {
            merged_mesh.Vertices.insert(merged_mesh.Vertices.end(), sub_mesh.Vertices.begin(), sub_mesh.Vertices.end());

            for (uint32_t index : sub_mesh.Indices)
            {
                merged_mesh.Indices.push_back(index + index_offset);
            }

            index_offset += sub_mesh.Indices.size();

            // TODO: process texture
        }

        return merged_mesh;
    }
};

void ProcessModel(const aiScene* scene, const aiNode* node, Model* model);
void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, Model::SubMesh* out_mesh);

StaticMeshData RenderResourceLoader::LoadStaticMesh(const String& mesh_file, const AxisAlignedBox& bounding_box)
{
    std::vector<byte> data = FileManager::Read(mesh_file.c_str());

    Assimp::Importer importer;
    const aiScene*   scene = importer.ReadFileFromMemory(data.data(), data.size(), aiProcess_Triangulate);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOG_ERROR("load static mesh '{}' failed, error: {}", mesh_file, importer.GetErrorString());
    }

    StaticMeshData mesh_data;

    Model model;
    ProcessModel(scene, scene->mRootNode, &model);
    Model::SubMesh merged_mesh = model.MergeSubMesh();

    mesh_data.VertexBuffer = std::make_shared<BufferData>(merged_mesh.Vertices.size() * sizeof(MeshVertexData));
    mesh_data.IndexBuffer  = std::make_shared<BufferData>(merged_mesh.Indices.size() * sizeof(uint16_t));

    ASSERT(merged_mesh.Vertices.size() <= std::numeric_limits<uint16_t>::max()); // check if need uint32_t 

    std::copy(merged_mesh.Vertices.begin(), merged_mesh.Vertices.end(), (MeshVertexData*)mesh_data.VertexBuffer->Data);
    std::copy(merged_mesh.Indices.begin(), merged_mesh.Indices.end(), (uint16_t*)mesh_data.IndexBuffer->Data);

    return mesh_data;
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

void ProcessMesh(const aiScene* scene, const aiMesh* mesh, Model* model, Model::SubMesh* out_mesh)
{
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertexData vertex;
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
}
