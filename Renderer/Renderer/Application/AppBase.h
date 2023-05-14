#pragma once

#include <Windows/Window.h>
#include <UI/ImguiManager.h>
#include <thread>

namespace NAMESPACE 
{
	class Graphics;
	class FileLoader;
	using std::thread;

	class AppBase
	{
	public:
		AppBase(int screenWidth, int screenHeight);
		AppBase(const AppBase&) = delete;
		AppBase& operator=(const AppBase&) = delete;
		virtual ~AppBase();

		int Run();

		virtual bool Initialize();
		virtual void UpdateGUI() = 0;
		virtual void Update(float dt) = 0;
		virtual void Render() = 0;

	protected:
		Window window;
		ImguiManager imguiManager;
		Graphics* graphics;
		FileLoader* fileLoader;
		int m_screenWidth;
		int m_screenHeight;

		/*
		*	Threads
		*/
		//thread assetLoadThread; // ��, �ؽ��ĵ��� �ε��ϴ� ������.
		//thread renderThread;    // �������� ó���ϴ� ������.
	};

}


