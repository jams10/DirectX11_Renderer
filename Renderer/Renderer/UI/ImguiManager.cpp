#include "ImguiManager.h"

#include <imgui.h>
#include <iostream>
#include <Windows/Window.h>
#include <Graphics/D3DGraphics.h>

namespace NAMESPACE
{
	using std::cout;

	ImguiManager::ImguiManager()
	{
		IMGUI_CHECKVERSION();     // ���� üũ
		ImGui::CreateContext();   // Imgui ���̺귯���� ������ �۾��� �ϱ� ���� ���ؽ�Ʈ ����.

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

		ImGui::StyleColorsDark(); // ui �׸��� ���������� ��������.
	}

	ImguiManager::~ImguiManager()
	{
		ImGui::DestroyContext();  // ������ ���ؽ�Ʈ ����.
	}
}


