#include "engine_pch.h"
#include "WindowContainer.h"
#include <iostream>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

namespace mi
{
	WindowContainer::WindowContainer()
	{
		static bool rawInputInit = false;
		if (!rawInputInit)
		{
			RAWINPUTDEVICE rid;
			rid.usUsagePage = 0x01; // Mouse
			rid.usUsage = 0x02;
			rid.dwFlags = 0;
			rid.hwndTarget = NULL;

			// Useless at the moment as raw input devices isnt being used.
			if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
			{

				ErrorLog::Log(GetLastError(), "failed registering raw input devices lol.");
				exit(-1);
			}
			rawInputInit = true;
		}
	}
	//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	LRESULT WindowContainer::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
		{
			return true;
		}
		return myInput.UpdateEvents(hwnd, uMsg, wParam, lParam);
	}
}
