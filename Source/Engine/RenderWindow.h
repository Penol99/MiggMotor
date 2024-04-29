#pragma once
#include "ErrorLog.h"
#include <Vector.h>

namespace mi
{
	class WindowContainer;
	class Graphics;
	class RenderWindow
	{
	public:
		bool Init(Graphics& aGFXObject, WindowContainer* pWindowContainer, HINSTANCE aHInstance, std::string aWindowTitle, std::string aWindowClass, int aWidth, int aHeight);
		bool ProcessMessages();
		static void SetFullscreen(bool aValue, HWND& aHwnd);
		static bool GetIsFullscreen() { return myIsFullscreen; }
		HWND GetHWND() const;
		float GetScaleFactor();
		mi::Vector2ui GetScreenSize();
		~RenderWindow();
	private:
		void RegisterWindowClass();
		HWND myHandle = NULL; // Handle to this window;
		HINSTANCE hInstance = NULL; // Handle to this application instance
		std::string myWindowTitle = "";
		std::wstring myWindowTitleWide = L"";
		std::string myWindowClass = "";
		std::wstring myWindowClassWide = L"";
		float myScaleFactor = 0;
		int myWidth = 0;
		int myHeight = 0;
		inline static bool myIsFullscreen = false;
		inline static Graphics* myGfx;

	};
}