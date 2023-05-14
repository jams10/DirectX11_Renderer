#pragma once

#include <Utils/CustomMacros.h>
#include <directxtk/SimpleMath.h>

namespace NAMESPACE
{
    using DirectX::SimpleMath::Vector2;
    using DirectX::SimpleMath::Vector3;

    struct Vertex 
    {
        Vector3 position;
        Vector3 normal;
        Vector2 texcoord;
    };
}