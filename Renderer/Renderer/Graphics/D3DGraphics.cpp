
#include "D3DGraphics.h"               
#include <iostream>
#include <d3dcompiler.h>
#include <imgui_impl_dx11.h>

#pragma comment(lib,"d3d11.lib")        // Direct3D 함수들이 정의된 라이브러리를 링크해줌.
#pragma comment(lib, "D3DCompiler.lib") // 셰이더를 런타임에 컴파일 해줄 때 사용할 수 있지만, 우리는 셰이더를 불러오는 함수를 사용하기 위해 연결해줬음. 

namespace NAMESPACE
{
    using std::cout;

	D3DGraphics::D3DGraphics()
	{
        m_bEnableVsync = false;
        m_screenWidth = 0;
        m_screenHeight = 0;
	}

	D3DGraphics::~D3DGraphics()
	{
        cout << "Destroy D3DGraphics." << '\n';
	}

	bool D3DGraphics::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hWnd, float screenDepth, float screenNear)
	{
        m_bEnableVsync = vsync;
        m_screenWidth = screenWidth;
        m_screenHeight = screenHeight;

		const D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE;

        UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // 디버깅이 가능하도록 하는 플래그.
#endif

        ComPtr<ID3D11Device> device;
        ComPtr<ID3D11DeviceContext> context;

        const D3D_FEATURE_LEVEL featureLevels[2] = {
            D3D_FEATURE_LEVEL_11_0, // 더 높은 버전이 먼저 오도록 설정
            D3D_FEATURE_LEVEL_9_3 };
        D3D_FEATURE_LEVEL featureLevel;

#pragma region Create : Device, Device Context
        THROWFAILED(D3D11CreateDevice(
            nullptr,                        // Specify nullptr to use the default adapter.
            driverType,                     // Create a device using the hardware graphics driver.
            0,                              // Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
            createDeviceFlags,              // Set debug and Direct2D compatibility flags.
            featureLevels,                  // List of feature levels this app can support.
            ARRAYSIZE(featureLevels),       // Size of the list above.
            D3D11_SDK_VERSION,              // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
            device.GetAddressOf(),          // Returns the Direct3D device created.
            &featureLevel,                  // Returns feature level of device created.
            context.GetAddressOf()          // Returns the device immediate context.
        ));

        if (featureLevel != D3D_FEATURE_LEVEL_11_0) {
            cout << "Failed : D3D Feature Level 11 unsupported.\n";
            __ERRORLINE__
            return false;
        }

        // 4X MSAA 지원하는지 확인
        device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &numQualityLevels);
        if (numQualityLevels <= 0) {
            cout << "Warning : MSAA not supported.\n";
        }

        // 생성한 device, device context COM 인터페이스를 가져와 캐싱.
        THROWFAILED(device.As(&m_pDevice));

        THROWFAILED(context.As(&m_pContext));
#pragma endregion

#pragma region Create : Swapchain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width = screenWidth;                 // set the back buffer width
        sd.BufferDesc.Height = screenHeight;               // set the back buffer height
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
        sd.BufferCount = 2;                                // Double-buffering
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        
        // DXGI_USAGE_SHADER_INPUT : 쉐이더에 입력으로 넣어주기 위해 필요.
        sd.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;                            // the window to be used
        sd.Windowed = TRUE;                                // windowed/full-screen mode
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        if (numQualityLevels > 0) {
            sd.SampleDesc.Count = 4; // how many multisamples
            sd.SampleDesc.Quality = numQualityLevels - 1;
        }
        else {
            sd.SampleDesc.Count = 1; // how many multisamples
            sd.SampleDesc.Quality = 0;
        }

        THROWFAILED(D3D11CreateDeviceAndSwapChain(
            0, // Default adapter
            driverType,
            0, // No software device
            createDeviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &sd,
            m_pSwapChain.GetAddressOf(), m_pDevice.GetAddressOf(), &featureLevel,
            m_pContext.GetAddressOf()));
#pragma endregion

        if (CreateRenderTargetView() == false) return false;

        SetViewport(screenWidth, screenHeight);

#pragma region Create : RasterizerState
        // Create a rasterizer state : Solid, Wireframe 버전 둘 다 만들어줌.
        D3D11_RASTERIZER_DESC rastDesc;
        ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
        // rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
        rastDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
        rastDesc.FrontCounterClockwise = false;
        rastDesc.DepthClipEnable = true; // <- zNear, zFar 확인에 필요

        m_pDevice->CreateRasterizerState(&rastDesc, m_pRasterizerSate.GetAddressOf());

        rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

        m_pDevice->CreateRasterizerState(&rastDesc, m_pWireRasterizerSate.GetAddressOf());
#pragma endregion

        CreateDepthBuffer(m_pDevice, screenWidth, screenHeight, numQualityLevels, m_pDepthStencilView);

        // 깊이 스텐실 스테이트 생성. 깊이 값을 어떻게 비교해 버퍼를 업데이트 할지 알려줌.
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true; // false
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
        THROWFAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, m_pDepthStencilState.GetAddressOf()));

        // imgui dx11 구현 초기화.
        if (ImGui_ImplDX11_Init(m_pDevice.Get(), m_pContext.Get()) == false)
        {
            cout << "Failed : ImGui_ImplDX11_Init()\n";
            __ERRORLINE__
            return false;
        }

        cout << "Success : D3D has initialized!\n";
        return true;
	}

	void D3DGraphics::BeginFrame(float red, float green, float blue, float alpha) noexcept
	{
        // RS: Rasterizer stage
        // OM: Output-Merger stage
        // VS: Vertex Shader
        // PS: Pixel Shader
        // IA: Input-Assembler stage

        SetViewport(m_screenWidth, m_screenHeight);

        float clearColor[4] = { red, green, blue, alpha };
        m_pContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);

        // 마우스 피킹에 사용할 indexRenderTarget도 초기화
        m_pContext->ClearRenderTargetView(m_pIndexRenderTargetView.Get(), clearColor);

        m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        // Multiple render targets
        // 인덱스를 저장할 RenderTarget을 추가
        ID3D11RenderTargetView* targets[] = { m_pRenderTargetView.Get(), m_pIndexRenderTargetView.Get() };
        m_pContext->OMSetRenderTargets(2, targets, m_pDepthStencilView.Get());
        m_pContext->OMSetDepthStencilState(m_pDepthStencilState.Get(), 0);

        if (m_drawAsWire) // wire 프레임 선택한 경우 와이어 프레임 모드로 래스터라이저 스테이트 변경, 와이어 프레임으로 그려줌.
        {
            m_pContext->RSSetState(m_pWireRasterizerSate.Get());
        }
        else 
        {
            m_pContext->RSSetState(m_pRasterizerSate.Get());
        }
	}

	void D3DGraphics::EndFrame()
	{
        if (m_bEnableVsync)
        {
            m_pSwapChain->Present(1u, 0u);
        }
        else
        {
            m_pSwapChain->Present(0u, 0u);
        }
	}

    bool D3DGraphics::CreateRenderTargetView()
    {
        ComPtr<ID3D11Texture2D> backBuffer;
        m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf()));
        if (backBuffer) 
        {
            m_pDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());

            // MSAA를 사용할 경우 backbuffer의 텍스쳐가 Texture2DMS임.
            // backbuffer에 씬을 렌더링한 뒤에 Texture2DMS를 Texture2D로 바꾸어서 후처리 작업을 위한 필터에 ShaderResource로 넣어주어야 함.

            D3D11_TEXTURE2D_DESC desc;
            backBuffer->GetDesc(&desc); // Swapchain 생성시 자동으로 생성된 backbuffer 용도의 텍스쳐 서술자를 그대로 가져옴.

            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.MiscFlags = 0;

            THROWFAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pTempTexture.GetAddressOf()));

            THROWFAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pIndexTempTexture.GetAddressOf()));

            // ShaderResource를 (backBuffer가 아니라) tempTexture로부터 생성
            m_pDevice->CreateShaderResourceView(m_pTempTexture.Get(), nullptr, m_pShaderResourceView.GetAddressOf());

            // 마우스 피킹에 사용할 1x1 크기의 작은 스테이징 텍스쳐 만들기
            desc.BindFlags = 0;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
            desc.Usage = D3D11_USAGE_STAGING;
            desc.Width = 1;
            desc.Height = 1;

            THROWFAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pIndexStagingTexture.GetAddressOf()));

            // 마우스 피킹에 사용할 인덱스에 해당하는 색을 렌더링할 텍스쳐와 렌더타겟 생성
            backBuffer->GetDesc(&desc); // BackBuffer와 동일한 설정
            THROWFAILED(m_pDevice->CreateTexture2D(&desc, nullptr, m_pIndexTexture.GetAddressOf()));

            m_pDevice->CreateRenderTargetView(m_pIndexTexture.Get(), nullptr, m_pIndexRenderTargetView.GetAddressOf());
        }
        else 
        {
            std::cout << "Failed : CreateRenderTargetView()\n";
            __ERRORLINE__
            return false;
        }

        return true;
    }

    void D3DGraphics::SetViewport(int screenWidth, int screenHeight)
    {
        // Set the viewport
        ZeroMemory(&m_screenViewport, sizeof(D3D11_VIEWPORT));
        m_screenViewport.TopLeftX = 0;
        m_screenViewport.TopLeftY = 0;
        m_screenViewport.Width = float(screenWidth);
        m_screenViewport.Height = float(screenHeight);
        m_screenViewport.MinDepth = 0.0f;
        m_screenViewport.MaxDepth = 1.0f; // Note: important for depth buffering

        m_pContext->RSSetViewports(1, &m_screenViewport);
    }

    bool D3DGraphics::CreateDepthBuffer(ComPtr<ID3D11Device>& device, int screenWidth, int screenHeight,
        UINT& numQualityLevels, ComPtr<ID3D11DepthStencilView>& depthStencilView)
    {
        // 깊이 스텐실용 텍스쳐 생성.
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        depthStencilBufferDesc.Width = screenWidth;   // 텍스쳐 크기를 스왑 체인의 프레임 버퍼 크기와 맞춰줌.
        depthStencilBufferDesc.Height = screenHeight;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if (numQualityLevels > 0) 
        {
            depthStencilBufferDesc.SampleDesc.Count = 4; // how many multisamples
            depthStencilBufferDesc.SampleDesc.Quality = numQualityLevels - 1;
        }
        else 
        {
            depthStencilBufferDesc.SampleDesc.Count = 1; // how many multisamples
            depthStencilBufferDesc.SampleDesc.Quality = 0;
        }
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.CPUAccessFlags = 0;
        depthStencilBufferDesc.MiscFlags = 0;

        ComPtr<ID3D11Texture2D> depthStencilBuffer;

        THROWFAILED(device->CreateTexture2D(&depthStencilBufferDesc, 0, depthStencilBuffer.GetAddressOf()));

        // 깊이 스텐실 텍스쳐에 대한 뷰 생성.
        THROWFAILED(device->CreateDepthStencilView(depthStencilBuffer.Get(), 0, depthStencilView.GetAddressOf()));

        return true;
    }
    
    void D3DGraphics::ShutdownImGUI()
    {
        cout << "Shutdown Imgui DX11." << '\n';
        ImGui_ImplDX11_Shutdown();
    }
}


