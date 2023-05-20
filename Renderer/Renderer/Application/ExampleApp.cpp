#include "ExampleApp.h"

#include <iostream>
#include <imgui.h>
#include <Graphics/Graphics.h>
#include <Utils/FileLoader.h>
#include <Utils/StringEncode.h>

namespace NAMESPACE
{
	using std::cout;

	ExampleApp::ExampleApp(int screenWidth, int screenHeight) : AppBase(screenWidth, screenHeight) 
	{
	}

	bool ExampleApp::Initialize()
	{
		if (AppBase::Initialize() == false)
			return false;

		return true;
	}

	void ExampleApp::UpdateGUI()
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu(WideToMultiU8(L"창").c_str()))
			{
				if (ImGui::MenuItem(WideToMultiU8(L"시스템 정보").c_str()))
					m_openSystemInfoWindow = true;

				if (ImGui::MenuItem(WideToMultiU8(L"씬").c_str()))
				{
					m_openSceneSelectWindow = true;
				}

				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		// 씬 선택 창.
		if (m_openSceneSelectWindow)
		{
			ImGui::Begin(WideToMultiU8(L"Select Scene").c_str(), &m_openSceneSelectWindow);
			{
				ImGui::Text(WideToMultiU8(L"모든 씬은 HDR 파이프라인을 사용합니다.").c_str());
				if (ImGui::Button(WideToMultiU8(L"Blinn-Phong Shading Model").c_str()))
				{
					
				}
			}
			ImGui::End();
		}
	}

	void ExampleApp::Update(float dt)
	{
		if (currentScene != nullptr)
		{
			currentScene->Update(dt);
		}
	}

	void ExampleApp::Render()
	{
		if (currentScene != nullptr)
		{
			currentScene->Render();
		}
	}
}

