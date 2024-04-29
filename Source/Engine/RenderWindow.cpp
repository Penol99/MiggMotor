#include <engine_pch.h>
#include "WindowContainer.h"
#define WINVER 0x0605

namespace mi {
	RenderWindow::~RenderWindow()
	{
		if (this->myHandle != NULL)
		{
			UnregisterClass(this->myWindowClassWide.c_str(), this->hInstance);
			DestroyWindow(this->myHandle);

		}
	}

	bool RenderWindow::Init(Graphics& aGFXObject, WindowContainer* pWindowContainer, HINSTANCE aHInstance, std::string aWindowTitle, std::string aWindowClass, int aWidth, int aHeight)
	{
		pWindowContainer;
		myGfx = &aGFXObject;
		hInstance = aHInstance;

		myWindowTitle = aWindowTitle;
		myWindowTitleWide = StringConverter::StringToWide(this->myWindowTitle);
		myWindowClass = aWindowClass;
		myWindowClassWide = StringConverter::StringToWide(this->myWindowClass);

		this->RegisterWindowClass();


		RECT windowRect;
		windowRect.left = GetSystemMetrics(SM_CXSCREEN) / 2 - aWidth / 2;
		windowRect.top = GetSystemMetrics(SM_CYSCREEN) / 2 - aHeight / 2;
		windowRect.right = windowRect.left + aWidth;
		windowRect.bottom = windowRect.top + aHeight;

		AdjustWindowRect(&windowRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_MAXIMIZEBOX, FALSE);


		this->myHandle = CreateWindowEx
		(
			0,
			myWindowClassWide.c_str(),
			myWindowTitleWide.c_str(),
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			windowRect.left, // Window X 
			windowRect.top, // Window Y 
			windowRect.right - windowRect.left, // Window Width
			windowRect.bottom - windowRect.top, // Window Height
			NULL,
			NULL,
			hInstance,
			pWindowContainer
		);

		if (this->myHandle == NULL)
		{
			ErrorLog::Log(GetLastError(), "Failed to Create window with CreateWindowEX: " + this->myWindowTitle);
			return false;
		}


		UINT dpi = GetDpiForWindow(myHandle);
		myScaleFactor = (float)dpi / 96.0f;
		ShowWindow(this->myHandle, SW_SHOW);
		SetForegroundWindow(this->myHandle);
		SetFocus(this->myHandle);

		//SetFullscreen(false, myHandle);

		// Adjust the window position and size to cover the entire screen
		myWidth = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
		myHeight = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

		SetWindowPos(myHandle, NULL, 0, 0, myWidth, myHeight, SWP_FRAMECHANGED);
		return true;
	}

	bool RenderWindow::ProcessMessages()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg, this->myHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_NULL)
		{
			if (!IsWindow(this->myHandle))
			{
				this->myHandle = NULL;
				UnregisterClass(this->myWindowClassWide.c_str(), this->hInstance);
				return false;
			}
		}
		return true;
	}

	void RenderWindow::SetFullscreen(bool aValue, HWND& aHwnd)
	{
		static WINDOWPLACEMENT wpc;
		myIsFullscreen = aValue;
		DWORD dwStyle = GetWindowLong(aHwnd, GWL_STYLE);
		if (!myIsFullscreen)
		{
			SetWindowLong(aHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(aHwnd, &wpc);
			SetWindowPos(aHwnd, NULL, 0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
		}
		else
		{
			MONITORINFO mi = { sizeof(mi) };
			if (GetWindowPlacement(aHwnd, &wpc) &&
				GetMonitorInfo(MonitorFromWindow(aHwnd,
					MONITOR_DEFAULTTOPRIMARY), &mi))
			{
				SetWindowLong(aHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(aHwnd, HWND_TOP,
					mi.rcMonitor.left, mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
			}
		}
		RECT rect;
		GetClientRect(aHwnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		//myGfx->Resize(width, height);

		std::cout << "Width: " << width << " Height: " << height << std::endl;
	}

	HWND RenderWindow::GetHWND() const
	{
		return myHandle;
	}

	float RenderWindow::GetScaleFactor()
	{
		return myScaleFactor;
	}

	Vector2ui RenderWindow::GetScreenSize()
	{
		mi::Vector2ui test(myWidth, myHeight);
		return test;
	}

	LRESULT CALLBACK HandleMsgRedirect(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		switch (uMsg)
		{
		case WM_CREATE:
		{
			RenderWindow::SetFullscreen(true, hwnd);
			break;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			RenderWindow::SetFullscreen(false, hwnd);
			break;
		}
		case WM_SYSCOMMAND:
		{
			switch (wParam & 0xfff0)  // Extract the system command code
			{
			case SC_MAXIMIZE:  // Window is being maximized
			{
				RenderWindow::SetFullscreen(true, hwnd);
				break;
			}
			case SC_RESTORE:  // Window is being restored
			{
				if (RenderWindow::GetIsFullscreen())
				{
					RenderWindow::SetFullscreen(false, hwnd);
				}
				break;
			}
			}
			break;
		}
		case WM_SIZE:
		{
			if (ImGui::GetCurrentContext() != NULL)
			{
				ImGuiIO& io = ImGui::GetIO();
				io.DisplaySize.x = (float)LOWORD(lParam);
				io.DisplaySize.y = (float)HIWORD(lParam);
			}
			break;
		}
		default:
		{
			WindowContainer* const pWindow = reinterpret_cast<WindowContainer*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}
		}
		// handle default behavior for messages not handled in switch-case
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	LRESULT CALLBACK HandleMessageSetup(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{

		switch (uMsg)
		{
		case WM_NCCREATE:
		{
			const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
			WindowContainer* pWindow = reinterpret_cast<WindowContainer*>(pCreate->lpCreateParams);
			if (pWindow == nullptr)
			{
				ErrorLog::Log("Error: Pointer to window container is null during WM_NCCREATE.");
				exit(-1);
			}
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(HandleMsgRedirect));
			return pWindow->WindowProc(hwnd, uMsg, wParam, lParam);
		}

		default:
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	void RenderWindow::RegisterWindowClass()
	{
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = HandleMessageSetup;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = this->hInstance;
		wc.hIcon = NULL;
		wc.hIconSm = NULL;
		wc.hCursor = NULL/*LoadCursor(NULL, IDC_ARROW)*/;
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = this->myWindowClassWide.c_str();
		wc.cbSize = sizeof(WNDCLASSEX);
		RegisterClassEx(&wc);
	}
}