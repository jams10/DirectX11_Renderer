
#include "Window.h"
#include <iostream>

namespace NAMESPACE {

	using std::cout;

#pragma region WindowClass
	// ������ Ŭ����
	Window::WindowClass Window::WindowClass::wndClass; // static ��� ���� ����
	// WindowClass ������. ������ Ŭ���� ������ ���.
	Window::WindowClass::WindowClass() noexcept
		:
		hInst(GetModuleHandle(nullptr)) // exe ���Ͽ����� �� �Լ��� ������ �ν��Ͻ� �ڵ��̳� WinMain()�� �Ű������� hInstance�� ���� �ٸ��� �ʴ�. 
		// �׷���, DLL �ȿ� ��������� ������ ��� WinMain�� hInstance�� DLL�� hInstance��, GetModuleHandle�� ������ DLL�� �ε��� exe ������ HINSTANCE�� ��ȯ �Ѵ�.
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
	// WindowClass �Ҹ���. ����� ������ ���� ���.
	Window::WindowClass::~WindowClass()
	{
		UnregisterClass(wndClassName, GetInstance()); // ����� �����츦 ���� ����.
	}
	// ������ Ŭ������ �̸��� �������ִ� �Լ�.
	const wchar_t* Window::WindowClass::GetName() noexcept
	{
		return wndClassName;
	}
	// ���α׷� �ν��Ͻ��� ���� �ڵ��� �������ִ� �Լ�.
	HINSTANCE Window::WindowClass::GetInstance() noexcept
	{
		return wndClass.hInst;
	}
#pragma endregion

#pragma region Window
	// Window Ŭ���� ������. ������ ���� �� ����
	Window::Window()
		:
		m_screenWidth(0),
		m_screenHeight(0),
		m_hWnd(nullptr)
	{
	}

	// Window Ŭ���� �Ҹ���. ������ ������ �ı�.
	Window::~Window()
	{
		DestroyWindow(m_hWnd);
	}

	bool Window::Initialize(const wchar_t* name, int width, int height)
	{
		// ���ϴ� client ���� ũ�⿡ ���缭 winodw�� ũ�⸦ �������.
		RECT wr;
		wr.left = 100;
		wr.right = width + wr.left;
		wr.top = 100;
		wr.bottom = height + wr.top;

		// ������ ������ ����.
		if (!AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
		{
			cout << "Failed : AdjustWindowRect" << '\n';
			__ERRORLINE__
			return false;
		}

		// �����츦 �����ϰ� �����쿡 ���� �ڵ��� ����.
		m_hWnd = CreateWindow(
			WindowClass::GetName(), name,
			WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
			CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
			nullptr, nullptr, WindowClass::GetInstance(), this
		);

		// ������ ������ ������ ���
		if (m_hWnd == nullptr)
		{
			cout << "Failed : CreateWindow" << '\n';
			__ERRORLINE__
			return false;
		}

		// ������ ȭ�鿡 ����.
		ShowWindow(m_hWnd, SW_SHOWDEFAULT);

		cout << "Success : Window has initialized!\n";
		return true;
	}

	// ������ Ÿ��Ʋ �̸� �������ִ� �Լ�.
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

	// ������ �޽��� ����. ������ �޽����� ���ν����� �����ִ� �Լ�.
	int Window::ProcessMessages() noexcept
	{
		MSG msg{ 0 };

		// �޽��� ť�� �޽����� ������, �ش� �޽����� �����ϰ� ���ν����� ������.(ť�� ����־ ��� ���¿� ���� ����.)
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

	// ���� ������� ��� �Լ��� ������ ���ν����� ����ϱ� ���� �⺻ ������ ����ϴ� �Լ�.
	LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// CreateWindow()�� ������ ���ڷ� �Ѱ��� this �����͸� �̿��� ������ API �ʿ��ִ� ������ Ŭ���� �����͸� ������.
		if (msg == WM_NCCREATE)
		{
			// ���� �����ͷ� ���� ������ Ŭ������ ���� �����͸� ����.
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

			// WinAPI�� ���� �����Ǵ� ����� �����͸� ������ Ŭ������ ���� �����͸� �����ϵ��� ������.
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));

			// �޽��� ���ν����� �Ϲ� �ڵ鷯�� �����ϰ� �޽����� �޾Ƶ��� �غ� ��.
			SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

			// window class �ڵ鷯�� �޽����� ������.
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}

		// WN_NCCREATE �޽��� ���� �޽����� �޴´ٸ�, �Ϲ� �ڵ鷯�� �޽����� ó��.
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	// ������ �޽����� �츮�� �ۼ��� ��� �Լ��� �����ϱ� ���� �Լ�.
	LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
	{
		// window class�� ���� �����͸� ����.
		Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		// window class �ڵ鷯�� �޽����� ������.
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	// ���� ������ �޽����� ó���� �� �Լ�.
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