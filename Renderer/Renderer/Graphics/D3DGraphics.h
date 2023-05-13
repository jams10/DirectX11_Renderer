#pragma once

#include <Utils/CustomMacros.h>
#include <Windows/WindowHeaders.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <exception>
#include <wrl.h>

namespace NAMESPACE
{
	using Microsoft::WRL::ComPtr;

#define THROWFAILED(x) ThrowIfFailed(x)

	inline void ThrowIfFailed(HRESULT hr) 
	{
		if (FAILED(hr)) 
		{
			throw std::exception();
		}
	}

	class D3DGraphics
	{
		friend class Graphics;

	public:
		D3DGraphics();
		D3DGraphics(const D3DGraphics&) = delete;
		D3DGraphics& operator=(const D3DGraphics&) = delete;
		~D3DGraphics();

		bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hWnd, float screenDepth, float screenNear);

		void BeginFrame(float red, float green, float blue, float alpha) noexcept;
		void EndFrame();

		ID3D11Device* GetDevice() const { return m_pDevice.Get(); }
		ID3D11DeviceContext* GetContext() const { return m_pContext.Get(); }
		IDXGISwapChain* GetSwapChain() const { return m_pSwapChain.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView.Get(); }

		bool CreateRenderTargetView();
		void SetViewport(int screenWidth, int screenHeight);
		bool CreateDepthBuffer(ComPtr<ID3D11Device>& device, int screenWidth, int screenHeight,
			UINT& numQualityLevels, ComPtr<ID3D11DepthStencilView>& depthStencilView);

		void ShutdownImGUI();

	private:
		// 기본 렌더링 필요 자원.
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pContext;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		ComPtr<IDXGISwapChain> m_pSwapChain;

		// 렌더타겟의 Texture2DMS를 Texture2D로 복사하기 위한 임시 Texture
		ComPtr<ID3D11Texture2D> m_pTempTexture;

		// Picking을 위한 Index를 저장할 Texture
		ComPtr<ID3D11Texture2D> m_pIndexTexture;
		ComPtr<ID3D11Texture2D> m_pIndexTempTexture;    // Texture2DMS -> Texture2D
		ComPtr<ID3D11Texture2D> m_pIndexStagingTexture; // 1x1 작은 크기
		ComPtr<ID3D11RenderTargetView> m_pIndexRenderTargetView;
		uint8_t m_pickColor[4] = {0,};                  // 이 색을 이용해서 물체가 선택(pick)되었는지 판단

		// 래스터라이저
		ComPtr<ID3D11RasterizerState> m_pRasterizerSate;
		ComPtr<ID3D11RasterizerState> m_pWireRasterizerSate;
		bool m_drawAsWire = false;
		bool m_usePostProcessing = false;

		// Depth Buffer 관련 자원.
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

		D3D11_VIEWPORT m_screenViewport;
		bool m_bEnableVsync;
		UINT numQualityLevels = 0;
		int m_screenWidth;
		int m_screenHeight;
	};
}


