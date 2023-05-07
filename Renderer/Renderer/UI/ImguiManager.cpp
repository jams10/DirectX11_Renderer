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
		IMGUI_CHECKVERSION();     // 버전 체크
		ImGui::CreateContext();   // Imgui 라이브러리를 가지고 작업을 하기 위한 컨텍스트 생성.

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls

		ImGui::StyleColorsDark(); // ui 테마를 검정색으로 설정해줌.
	}

	ImguiManager::~ImguiManager()
	{
		ImGui::DestroyContext();  // 생성한 컨텍스트 제거.
	}
}


