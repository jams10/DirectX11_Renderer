#pragma once

#include "WindowHeaders.h"
#include <optional>
#include <string>
#include <Utils/CustomMacros.h>

namespace NAMESPACE {

	using std::wstring;

	class Window
	{
#pragma region WindowClass
	private:
		// 윈도우 클래스를 등록/해제를 관리하는 싱글톤.
		class WindowClass
		{
		public:
			static const wchar_t* GetName() noexcept;
			static HINSTANCE GetInstance() noexcept;
		private:
			WindowClass() noexcept;
			~WindowClass();
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;

			static constexpr const wchar_t* wndClassName = L"WindowClass";
			static WindowClass wndClass; // SINGLETON
			HINSTANCE hInst;
		};
#pragma endregion

	public:
		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		bool Initialize(const wchar_t* name, int width, int height);
		bool SetTitle(const wstring& title);
		static int ProcessMessages() noexcept; // 모든 윈도우에 대한 메시지를 처리해야 하므로 static으로 선언함.
		HWND GetWindowHandle() { return m_hWnd; }
		void ShutDownImGUI();

	private:
		static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

	private:
		int m_screenWidth;
		int m_screenHeight;
		HWND m_hWnd;
	};
}

