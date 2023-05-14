#pragma once

#include <Utils/CustomMacros.h>
#include <Model/Vertex.h>
#include <Model/ModelData.h>

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <string>
#include <vector>

namespace NAMESPACE
{
    using std::string;
    using std::vector;

	class ModelLoader
	{
    public:
        void Load(string filePath);

        void ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr);

        ModelData ProcessMesh(aiMesh* mesh, const aiScene* scene);

    public:
        string basePath;
        vector<ModelData> models;
	};
}


