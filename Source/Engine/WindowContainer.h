#pragma once
#include "RenderWindow.h"
#include "Input/InputHandler.h"
#include "Graphics/Graphics.h"
namespace mi
{
	class WindowContainer
	{
	public:
		WindowContainer();
		LRESULT WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	protected:
		RenderWindow myRenderWindow;
		Graphics myGfx;
		InputHandler myInput;
	private:

	};
}