
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
		// Imgui�� ���� �� renderer backend(DX11)�� platform backend(win32) ���� ���� �ݾ��־�� ��.
		graphics->Shutdown();
		window.ShutDownImGUI();
	}

	bool AppBase::Initialize()
	{
		// ������ �ʱ�ȭ. + imgui win32 �ʱ�ȭ.
		if (window.Initialize(L"Renderer", m_screenWidth, m_screenHeight) == false)
		{
			cout << "Failed to Initialize Window\n";
			__ERRORLINE__
			return false;
		}

		// D3D �ʱ�ȭ. + imgui d3d �ʱ�ȭ.
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

			ImGui_ImplDX11_NewFrame(); // GUI ������ ����
			ImGui_ImplWin32_NewFrame();

			ImGui::NewFrame(); // � �͵��� ������ ���� ��� ����
			ImGui::Begin("Scene Control");

			// ImGui�� �������ִ� Framerate ���
			ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

			UpdateGUI(); // �߰������� ����� GUI
			ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));

			ImGui::End();
			ImGui::Render(); // �������� �͵� ��� ��.
			
			Update(ImGui::GetIO().DeltaTime); // �� ������Ʈ.

			graphics->BeginFrame(0.5f, 0.65f, 0.98f, 1.0f);
			// Scene Render.

			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // GUI ������.

			graphics->EndFrame(); // ���⼭ Swapchain�� Present ȣ���� back buffer�� front buffer�� ��ü.
		}
	}

}


