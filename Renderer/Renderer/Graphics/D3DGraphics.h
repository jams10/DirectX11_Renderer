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
		// �⺻ ������ �ʿ� �ڿ�.
		ComPtr<ID3D11Device> m_pDevice;
		ComPtr<ID3D11DeviceContext> m_pContext;
		ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		ComPtr<IDXGISwapChain> m_pSwapChain;

		// ����Ÿ���� Texture2DMS�� Texture2D�� �����ϱ� ���� �ӽ� Texture
		ComPtr<ID3D11Texture2D> m_pTempTexture;

		// Picking�� ���� Index�� ������ Texture
		ComPtr<ID3D11Texture2D> m_pIndexTexture;
		ComPtr<ID3D11Texture2D> m_pIndexTempTexture;    // Texture2DMS -> Texture2D
		ComPtr<ID3D11Texture2D> m_pIndexStagingTexture; // 1x1 ���� ũ��
		ComPtr<ID3D11RenderTargetView> m_pIndexRenderTargetView;
		uint8_t m_pickColor[4] = {0,};                  // �� ���� �̿��ؼ� ��ü�� ����(pick)�Ǿ����� �Ǵ�

		// �����Ͷ�����
		ComPtr<ID3D11RasterizerState> m_pRasterizerSate;
		ComPtr<ID3D11RasterizerState> m_pWireRasterizerSate;
		bool m_drawAsWire = false;
		bool m_usePostProcessing = false;

		// Depth Buffer ���� �ڿ�.
		ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		ComPtr<ID3D11DepthStencilState> m_pDepthStencilState;

		D3D11_VIEWPORT m_screenViewport;
		bool m_bEnableVsync;
		UINT numQualityLevels = 0;
		int m_screenWidth;
		int m_screenHeight;
	};
}


