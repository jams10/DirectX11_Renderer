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
			if (ImGui::BeginMenu(WideToMultiU8(L"â").c_str()))
			{
				if (ImGui::MenuItem(WideToMultiU8(L"�ý��� ����").c_str()))
					m_openSystemInfoWindow = true;

				if (ImGui::MenuItem(WideToMultiU8(L"��").c_str()))
					m_openModelWindow = true;
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();

		// �ý��� ���� â.
		if (m_openSystemInfoWindow)
		{
			ImGui::Begin(WideToMultiU8(L"�ý��� ����").c_str(), &m_openSystemInfoWindow);
			{
				// ImGui�� �������ִ� Framerate ���.
				ImGui::Text("Average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			}
			ImGui::End();
		}
		// �� ���� â.
		if (m_openModelWindow)
		{
			ImGui::Begin(WideToMultiU8(L"��").c_str(), &m_openModelWindow);
			{
				if (ImGui::Button(WideToMultiU8(L"�� ���� �ҷ�����").c_str()))
				{
					// WINAPI �̿�, ���̾�α� â ����� ���� ����.
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

