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

				if (ImGui::MenuItem(WideToMultiU8(L"모델").c_str()))
					m_openModelWindow = true;
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		// 시스템 정보 창.
		if (m_openSystemInfoWindow)
		{
			ImGui::Begin(WideToMultiU8(L"시스템 정보").c_str(), &m_openSystemInfoWindow);
			{
				// ImGui가 측정해주는 Framerate 출력.
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}
		// 모델 정보 창.
		if (m_openModelWindow)
		{
			ImGui::Begin(WideToMultiU8(L"모델").c_str(), &m_openModelWindow);
			{
				if (ImGui::Button(WideToMultiU8(L"모델 파일 불러오기").c_str()))
				{
					// WINAPI 이용, 다이얼로그 창 띄워서 파일 선택.
					fileLoader->LoadModel();
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

