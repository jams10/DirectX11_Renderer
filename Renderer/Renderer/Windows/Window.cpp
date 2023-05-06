
#include "Window.h"
#include <iostream>

namespace NAMESPACE {

	using std::cout;

#pragma region WindowClass
	// 윈도우 클래스
	Window::WindowClass Window::WindowClass::wndClass; // static 멤버 변수 정의
	// WindowClass 생성자. 윈도우 클래스 생성을 담당.
	Window::WindowClass::WindowClass() noexcept
		:
		hInst(GetModuleHandle(nullptr)) // exe 파일에서는 이 함수로 얻어오는 인스턴스 핸들이나 WinMain()의 매개변수인 hInstance의 값이 다르지 않다. 
		// 그러나, DLL 안에 윈도우들을 생성할 경우 WinMain의 hInstance는 DLL의 hInstance를, GetModuleHandle은 여전히 DLL을 로드한 exe 파일의 HINSTANCE를 반환 한다.
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(wc);
		wc.style = CS_OWNDC;
		wc.lpfnWndProc = HandleMsgSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = GetInstance();
		wc.hIcon = nullptr;
		wc.hCursor = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszMenuName = nullptr;
		wc.lpszClassName = GetName();
		wc.hIconSm = nullptr;

		RegisterClassEx(&wc);
	}
	// WindowClass 소멸자. 등록한 윈도우 해제 담당.
	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(wndClassName, GetInstance()); // 등록한 윈도우를 해제 해줌.
	}
	// 윈도우 클래스의 이름을 리턴해주는 함수.
	const wchar_t* Window::WindowClass::GetName() noexcept
	{
		return wndClassName;
	}
	// 프로그램 인스턴스에 대한 핸들을 리턴해주는 함수.
	HINSTANCE Window::WindowClass::GetInstance() noexcept
	{
		return wndClass.hInst;
	}
#pragma endregion

#pragma region Window
	// Window 클래스 생성자. 윈도우 생성 및 설정
	Window::Window()
		:
		m_screenWidth(0),
		m_screenHeight(0),
		m_hWnd(nullptr)
	{
	}

	// Window 클래스 소멸자. 생성한 윈도우 파괴.
	Window::~Window()
	{
		DestroyWindow(m_hWnd);
	}

	bool Window::Initialize(const wchar_t* name, int width, int height)
	{
		// 원하는 client 영역 크기에 맞춰서 winodw의 크기를 계산해줌.
		RECT wr;
		wr.left = 100;
		wr.right = width + wr.left;
		wr.top = 100;
		wr.bottom = height + wr.top;

		// 윈도우 사이즈 조절.
		if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
		{
			cout << "Failed : AdjustWindowRect" << '\n';
			__ERRORLINE__
			return false;
		}

		// 윈도우를 생성하고 윈도우에 대한 핸들을 얻어옴.
		m_hWnd = CreateWindow(
			WindowClass::GetName(), name,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, WindowClass::GetInstance(), this
		);

		// 윈도우 생성에 실패한 경우
		if (m_hWnd == nullptr)
		{
			cout << "Failed : CreateWindow" << '\n';
			__ERRORLINE__
			return false;
		}

		// 윈도우 화면에 띄우기.
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);

		cout << "Success : Window has initialized!\n";
		return true;
	}

	// 윈도우 타이틀 이름 변경해주는 함수.
	bool Window::SetTitle(const std::wstring& title)
	{
		if (SetWindowText(m_hWnd, title.c_str()) == 0)
		{
			cout << "Failed : SetWindowText" << '\n';
			__ERRORLINE__
			return false;
		}
		return true;
	}

	// 윈도우 메시지 루프. 윈도우 메시지를 프로시져로 보내주는 함수.
	int Window::ProcessMessages() noexcept
	{
		MSG msg{ 0 };

		// 메시지 큐에 메시지가 있으면, 해당 메시지를 제거하고 프로시져로 보내줌.(큐가 비어있어도 블록 상태에 들어가지 않음.)
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				return 0;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return 1;
	}

	// 직접 만들어준 멤버 함수를 윈도우 프로시져로 사용하기 위한 기본 설정을 담당하는 함수.
	LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// CreateWindow()의 마지막 인자로 넘겨준 this 포인터를 이용해 윈도우 API 쪽에있는 윈도우 클래스 포인터를 저장함.
		if (msg == WM_NCCREATE)
		{
			// 생성 데이터로 부터 윈도우 클래스에 대한 포인터를 추출.
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

			// WinAPI에 의해 관리되는 사용자 데이터를 윈도우 클래스에 대한 포인터를 저장하도록 설정함.
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

			// 메시지 프로시져를 일반 핸들러로 변경하고 메시지를 받아들일 준비를 함.
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

			// window class 핸들러에 메시지를 전달함.
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}

		// WN_NCCREATE 메시지 전에 메시지를 받는다면, 일반 핸들러로 메시지를 처리.
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// 윈도우 메시지를 우리가 작성한 멤버 함수로 전달하기 위한 함수.
	LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// window class에 대한 포인터를 얻어옴.
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		// window class 핸들러에 메시지를 전달함.
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// 실제 윈도우 메시지를 처리해 줄 함수.
	LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		switch (msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}