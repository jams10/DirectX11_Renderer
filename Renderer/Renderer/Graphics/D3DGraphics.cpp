
#include "D3DGraphics.h"               
#include <iostream>
#include <d3dcompiler.h>
#include <vector>
#include <imgui_impl_dx11.h>

#pragma comment(lib,"d3d11.lib")        // Direct3D 함수들이 정의된 라이브러리를 링크해줌.
#pragma comment(lib, "D3DCompiler.lib") // 셰이더를 런타임에 컴파일 해줄 때 사용할 수 있지만, 우리는 셰이더를 불러오는 함수를 사용하기 위해 연결해줬음. 

namespace NAMESPACE
{
    using std::cout;
    using std::vector;

	D3DGraphics::D3DGraphics()
	{
        m_bEnableVsync = false;
        m_screenWidth = 0;
        m_screenHeight = 0;
        m_screenViewport = {};
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
        device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_numQualityLevels);
        if (m_numQualityLevels <= 0) {
            cout << "Warning : MSAA not supported.\n";
        }

        // 생성한 device, device context COM 인터페이스를 가져와 캐싱.
        THROWFAILED(device.As(&m_pDevice));

        THROWFAILED(context.As(&m_pContext));
#pragma endregion

#pragma region Create : Swapchain
        // 최종적으로 모니터에 보여줄 렌더 타겟 버퍼 생성.
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferDesc.Width = screenWidth;                 // set the back buffer width
        sd.BufferDesc.Height = screenHeight;               // set the back buffer height
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 최종적으로 모니터로 보내주는 픽셀 포맷은 UNORM. 보통 모니터들이 내부적으로 RGBA 256색을 사용.
        sd.BufferCount = 2;                                // Double-buffering
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.Windowed = TRUE;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        // swapchain 자체가 가지고 있는 backbuffer는 MSAA를 켜줄 필요가 없음. MSAA 렌더 타겟의 결과가 LDR로 변환되어 스왑 체인으로 들어오기 때문.
        sd.SampleDesc.Count = 1; // _FLIP_은 MSAA 미지원
        sd.SampleDesc.Quality = 0;

        // MSAA를 끈 상태에서 swapchain 생성.
        ThrowIfFailed(D3D11CreateDeviceAndSwapChain(
            0, driverType, 0, createDeviceFlags, featureLevels, 1,
            D3D11_SDK_VERSION, &sd, m_pSwapChain.GetAddressOf(),
            m_pDevice.GetAddressOf(), &featureLevel, m_pContext.GetAddressOf()));

#pragma endregion

        if (CreateBuffers() == false) return false;

        CreateDepthBuffer(m_pDevice, screenWidth, screenHeight, m_numQualityLevels, m_pDepthStencilView);

        // 깊이 스텐실 스테이트 생성. 깊이 값을 어떻게 비교해 버퍼를 업데이트 할지 알려줌.
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true; // false
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
        THROWFAILED(m_pDevice->CreateDepthStencilState(&depthStencilDesc, m_pDepthStencilState.GetAddressOf()));

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
        m_pContext->ClearRenderTargetView(m_pFloatRTV.Get(), clearColor);

        m_pContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        vector<ID3D11RenderTargetView*> renderTargetViews = { m_pFloatRTV.Get() };
        // float 픽셀 포맷 렌더 타겟을 렌더링할 타겟으로 설정함.
        m_pContext->OMSetRenderTargets(UINT(renderTargetViews.size()), renderTargetViews.data(), m_pDepthStencilView.Get());  
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
        // MSAA로 Texture2DMS에 렌더링 된 결과를 Texture2D로 변환.(Resolve)
        // MSAA는 한 픽셀 안에 샘플을 여러 개 취득해 AA(Anti-Aliasing)를 하는 것이기 때문에 MSAA 결과를 저장할 수 있는 Texture2DMS는 픽셀 하나당 색깔 값이 여러개 있음.
        // 그러나 일반적으로 사용하는 이미지의 경우 픽셀 하나 당 색깔 값이 하나만 있어야 하므로 Texture2DMS를 Resolve를 통해 Texture2D로 변환함.
        m_pContext->ResolveSubresource(m_pResolvedBuffer.Get(), 0,
            m_pFloatBuffer.Get(), 0,
            DXGI_FORMAT_R16G16B16A16_FLOAT);

        float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
        m_pContext->ClearRenderTargetView(m_pBackbufferRTV.Get(), clearColor);
        m_pContext->OMSetRenderTargets(1, m_pBackbufferRTV.GetAddressOf(), NULL);
	}

    void D3DGraphics::PresentScene()
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

    bool D3DGraphics::CreateBuffers()
    {
        // Floating 포맷, MSAA를 사용하는 버퍼를 렌더 타겟으로 설정해 렌더링(Texture2DMS) -> Texture2D로 Resolve -> BackBuffer에 Resolve된 Texture2D 렌더링.
        
        // BackBuffer는 화면으로 최종 출력되기 때문에 렌더 타겟 뷰만 있으면 되고, 쉐이더 입력으로 사용할 쉐이더 리소스 뷰는 필요가 없음.
        ComPtr<ID3D11Texture2D> backBuffer;
        ThrowIfFailed(
            m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(backBuffer.GetAddressOf())));
        ThrowIfFailed(m_pDevice->CreateRenderTargetView(
            backBuffer.Get(), NULL, m_pBackbufferRTV.GetAddressOf()));
            
        // MSAA를 사용하는 렌더 타겟용 버퍼를 만들어주기에 앞서서 사용할 수 있는지 체크함.
        ThrowIfFailed(m_pDevice->CheckMultisampleQualityLevels(
            DXGI_FORMAT_R16G16B16A16_FLOAT, 4, &m_numQualityLevels));

        // Floating Point로 MSAA까지 적용해서 m_pFloatBuffer라는 이름의 MSAA 렌더 타겟에 사용할 텍스쳐를 만들어줌. (rasterization 결과가 들어오게 됨.)
        D3D11_TEXTURE2D_DESC desc;
        backBuffer->GetDesc(&desc);
        desc.MipLevels = desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // floating 픽셀 포맷을 사용하도록 함.
        desc.Usage = D3D11_USAGE_DEFAULT;             // GPU에서 읽기/쓰기 가능.
        desc.MiscFlags = 0;
        desc.CPUAccessFlags = 0;
        if (m_useMSAA && m_numQualityLevels) // TODO : MSAA 사용 여부도 UI 파라미터로 만들어주기.
        {
            desc.SampleDesc.Count = 4;
            desc.SampleDesc.Quality = m_numQualityLevels - 1;
        }
        else 
        {
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
        }

        ThrowIfFailed(
            m_pDevice->CreateTexture2D(&desc, NULL, m_pFloatBuffer.GetAddressOf()));

        ThrowIfFailed(m_pDevice->CreateShaderResourceView(
            m_pFloatBuffer.Get(), NULL, m_pFloatSRV.GetAddressOf()));

        ThrowIfFailed(m_pDevice->CreateRenderTargetView(m_pFloatBuffer.Get(), NULL,
            m_pFloatRTV.GetAddressOf()));

        CreateDepthBuffer(m_pDevice, m_screenWidth, m_screenHeight, UINT(m_useMSAA ? m_numQualityLevels : 0), m_pDepthStencilView);

        // FLOAT MSAA를 Relsolve해서 저장할 SRV/RTV
        // 멀티 샘플링꺼서 텍스쳐 자원을 만들어줌.
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        ThrowIfFailed(m_pDevice->CreateTexture2D(&desc, NULL,
            m_pResolvedBuffer.GetAddressOf()));
        ThrowIfFailed(m_pDevice->CreateShaderResourceView(
            m_pResolvedBuffer.Get(), NULL, m_pResolvedSRV.GetAddressOf()));
        ThrowIfFailed(m_pDevice->CreateRenderTargetView(
            m_pResolvedBuffer.Get(), NULL, m_pResolvedRTV.GetAddressOf()));

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
        UINT numQualityLevels, ComPtr<ID3D11DepthStencilView>& depthStencilView)
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


