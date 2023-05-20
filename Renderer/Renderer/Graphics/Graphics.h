#pragma once

#include <Utils/CustomMacros.h>
#include <Windows/WindowHeaders.h>

namespace NAMESPACE
{
	const bool VSYNC_ENABLED = true;
	const float SCREEN_DEPTH = 100.0f;
	const float SCREEN_NEAR = 1.0f;

	class D3DGraphics;

	class Graphics
	{
	public:
		Graphics();
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;
		~Graphics();

		bool Initialize(int, int, HWND);
		void Shutdown();
		bool BeginFrame(float red, float green, float blue, float alpha);
		bool EndFrame();
		void PresentScene();

	private:
		D3DGraphics* m_pD3D;
	};
}

