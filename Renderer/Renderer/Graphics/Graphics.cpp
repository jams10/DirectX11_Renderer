#include "Graphics.h"
#include "D3DGraphics.h"
#include <iostream>

namespace NAMESPACE
{
    using std::cout;

    Graphics::Graphics()
    {
        m_pD3D = nullptr;
    }

    Graphics::~Graphics()
    {
        Shutdown();
    }

    bool Graphics::Initialize(int screenWidth, int screenHeight, HWND wnd)
    {
        m_pD3D = new D3DGraphics();
        if (m_pD3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, wnd, SCREEN_DEPTH, SCREEN_NEAR) == false)
        {
            cout << "Failed : D3D Initialize.\n";
            __ERRORLINE__
            return false;
        }

        return true;
    }

    void Graphics::Shutdown()
    {
        m_pD3D->ShutdownImGUI();
        SAFE_RELEASE(m_pD3D)
    }

    bool Graphics::BeginFrame(float red, float green, float blue, float alpha)
    {
        m_pD3D->BeginFrame(red, green, blue, alpha);
        return true;
    }

    bool Graphics::EndFrame()
    {
        m_pD3D->EndFrame();
        return true;
    }
}

