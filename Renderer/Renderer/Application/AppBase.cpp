
#include "AppBase.h"
#include <iostream>
#include <Graphics/Graphics.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace NAMESPACE
{
	using std::cout;

	AppBase::AppBase(int screenWidth, int screenHeight)
		:
		m_screenWidth(screenWidth),
		m_screenHeight(screenHeight),
		window(),
		graphics(nullptr)
	{
	}

	AppBase::~AppBase()
	{
		// Imgui를 닫을 때 renderer backend(DX11)를 platform backend(win32) 보다 먼저 닫아주어야 함.
		graphics->Shutdown();
		window.ShutDownImGUI();
	}

	bool AppBase::Initialize()
	{
		// 윈도우 초기화. + imgui win32 초기화.
		if (window.Initialize(L"Renderer", m_screenWidth, m_screenHeight) == false)
		{
			cout << "Failed to Initialize Window\n";
			__ERRORLINE__
			return false;
		}

		// D3D 초기화. + imgui d3d 초기화.
		graphics = new Graphics();
		if (graphics->Initialize(m_screenWidth, m_screenHeight, window.GetWindowHandle()) == false)
		{
			cout << "Failed : Graphics Initialize.\n";
			__ERRORLINE__
				return false;
		}

		return true;
	}

	int AppBase::Run()
	{
		while (true)
		{
			if (window.ProcessMessages() == 0)
				return 0;

			ImGui_ImplDX11_NewFrame(); // GUI 프레임 시작
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame(); // 어떤 것들을 렌더링 할지 기록 시작
			ImGui::Begin("Scene Control");

			// ImGui가 측정해주는 Framerate 출력
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			UpdateGUI(); // 추가적으로 사용할 GUI
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

			ImGui::End();
			ImGui::Render(); // 렌더링할 것들 기록 끝.
			
			Update(ImGui::GetIO().DeltaTime); // 씬 업데이트.

			graphics->BeginFrame(0.5f, 0.65f, 0.98f, 1.0f);
			// Scene Render.

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI 렌더링.

			graphics->EndFrame(); // 여기서 Swapchain의 Present 호출해 back buffer와 front buffer를 교체.
		}
	}

}


