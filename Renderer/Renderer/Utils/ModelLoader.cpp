#include "ModelLoader.h"

#include <assimp/cimport.h>
#include <iostream>
#include <filesystem>

namespace NAMESPACE
{
    using namespace DirectX::SimpleMath;
    using std::string;
    using std::vector;
    using std::cout;

	void ModelLoader::Load(string filePath)
	{
        cout << "Start loading a model..." << '\n';

        std::filesystem::path path(filePath);

        this->basePath = path.parent_path().string();

        Assimp::Importer importer; // importer�� ���� �� ������ �о��.

        const aiScene* pScene = importer.ReadFile(
            filePath,
            aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

        if (!pScene)
        {
            std::cout << importer.GetErrorString();
            return;
        }
        else
        {
            Matrix tr; // Initial transformation
            ProcessNode(pScene->mRootNode, pScene, tr);
        }
	}

    void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Matrix tr)
    {
        cout << "Assimp Model Load - Process Node\n";
        cout << "\tName : " << node->mName.C_Str() << '\n';
        cout << "\tNumberOfMeshes : " << node->mNumMeshes << '\n';
        cout << "\tNumberOfChildren : " << node->mNumChildren << '\n';

        Matrix m;
        ai_real* temp = &node->mTransformation.a1;
        float* mTemp = &m._11;

        for (int t = 0; t < 16; t++) 
        {
            mTemp[t] = float(temp[t]);
        }

        m = m.Transpose() * tr;

        for (UINT i = 0; i < node->mNumMeshes; i++) 
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            auto newMesh = this->ProcessMesh(mesh, scene);

            for (auto& v : newMesh.vertices) 
            {
                v.position = DirectX::SimpleMath::Vector3::Transform(v.position, m);
            }

            models.push_back(newMesh);
        }

        for (UINT i = 0; i < node->mNumChildren; i++) 
        {
            this->ProcessNode(node->mChildren[i], scene, m);
        }
    }

    ModelData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene) 
    {
        // Data to fill
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        // �޽��� �� ������ ��ȸ�ϸ� ������ ��ġ, �븻, �ؽ��� ��ǥ�� ä����.
        for (UINT i = 0; i < mesh->mNumVertices; i++) 
        {
            Vertex vertex;

            vertex.position.x = mesh->mVertices[i].x;
            vertex.position.y = mesh->mVertices[i].y;
            vertex.position.z = mesh->mVertices[i].z;

            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
            vertex.normal.Normalize();

            if (mesh->mTextureCoords[0]) 
            {
                vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
                vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
            }

            vertices.push_back(vertex);
        }

        // ���� ������ ���� �� ������ �ε��� �迭�� ä����.
        for (UINT i = 0; i < mesh->mNumFaces; i++) 
        {
            aiFace face = mesh->mFaces[i];
            for (UINT j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        ModelData newMesh;
        newMesh.vertices = vertices;
        newMesh.indices = indices;

        // http://assimp.sourceforge.net/lib_html/materials.html
        // aiMaterial Ŭ������ ��� ���� �� std::min ���� ������ �߻�����. ��ó���⿡ NOMINMAX ��ũ�θ� �߰��� windows�� min�� ���������� �ʵ��� ��.
        if (mesh->mMaterialIndex >= 0) 
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) 
            {
                aiString filepath;
                material->GetTexture(aiTextureType_DIFFUSE, 0, &filepath);

                std::string fullPath =
                    this->basePath + std::string(std::filesystem::path(filepath.C_Str()).filename().string());

                newMesh.textureFilename = fullPath;
            }
        }

        return newMesh;
    }
}