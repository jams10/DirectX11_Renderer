#pragma once

#include <Utils/CustomMacros.h>
#include <directxtk/SimpleMath.h>
#include <Model/Material.h>

namespace NAMESPACE
{
    using DirectX::SimpleMath::Matrix;
    using DirectX::SimpleMath::Vector2;
    using DirectX::SimpleMath::Vector3;
    using DirectX::SimpleMath::Vector4;

    struct BasicVertexConstantBuffer 
    {
        Matrix model;
        Matrix invTranspose;
        Matrix view;
        Matrix projection;
    };

    static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
        "Constant Buffer size must be 16-byte aligned");

    struct NormalVertexConstantBuffer {
        // Matrix model;
        // Matrix invTranspose;
        // Matrix view;
        // Matrix projection;
        float scale = 0.1f;
        float dummy[3];
    };

    struct Light {
        Vector3 strength = Vector3(1.0f);              // 12
        float fallOffStart = 0.0f;                     // 4
        Vector3 direction = Vector3(0.0f, 0.0f, 1.0f); // 12
        float fallOffEnd = 10.0f;                      // 4
        Vector3 position = Vector3(0.0f, 0.0f, -2.0f); // 12
        float spotPower = 1.0f;                        // 4
    };

#define MAX_LIGHTS 3

    struct BasicPixelConstantBuffer {
        Vector3 eyeWorld;         // 12
        bool useTexture;          // 4
        Material material;        // 48
        Light lights[MAX_LIGHTS]; // 48 * MAX_LIGHTS
    };

    static_assert((sizeof(BasicPixelConstantBuffer) % 16) == 0,
        "Constant Buffer size must be 16-byte aligned");
}