#pragma once

#include <Utils/CustomMacros.h>
#include <vector>

#include "Vertex.h"

namespace NAMESPACE
{
	struct ModelData
	{
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::string textureFilename; 
	};
}


