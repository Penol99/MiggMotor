// InputHandler.cpp : Defines the functions for the static library.
//
#define WIN32_LEAN_AND_MEAN 
#include "InputHandler.h"


InputHandler::InputHandler():
	myMousePos{ 0 },
	myTentativeMouseWheelDelta(0),
	myMouseWheelDelta(0),
	myKeysCurrentState{ 0 },
	myKeysPreviousState{ 0 }
{

}
void InputHandler::SetHWND(HWND aHandle)
{
	myWindowHandle = aHandle;
}
HWND InputHandler::GetHWND()
{
	return myWindowHandle;
}
InputHandler::~InputHandler()
{

}

POINT InputHandler::GetMouseScreenPosition() const
{
	myMousePosTempPtr->x = myMouseLastPos.x;
	myMousePosTempPtr->y = myMouseLastPos.y;

	ClientToScreen(myWindowHandle, myMousePosTempPtr);
	return myMouseLastPos;
}

POINT InputHandler::GetMouseWindowPosition() const
{
	return myMouseLastPos;
}

bool InputHandler::IsKeyDown(const int aKeyCode) const
{
	return myKeysCurrentState[aKeyCode];
}

bool InputHandler::IsKeyPressed(const int aKeyCode) const
{
	return !myKeysPreviousState[aKeyCode] && myKeysCurrentState[aKeyCode];
}

bool InputHandler::IsKeyReleased(const int aKeyCode) const
{
	return myKeysPreviousState[aKeyCode] && !myKeysCurrentState[aKeyCode];
}
float InputHandler::GetMouseWheelData()
{
	return myMouseWheelDelta;
}

bool InputHandler::IsMouseButtonDown(const int aKeyCode) const
{
	return myKeysCurrentState[aKeyCode];
}

bool InputHandler::IsMouseButtonReleased(const int aKeyCode) const
{
	return myKeysPreviousState[aKeyCode] && !myKeysCurrentState[aKeyCode];
}

POINT InputHandler::GetMousePosition() const
{
	return myMousePos;
}

POINT InputHandler::GetMouseLastPos() const
{
	return myMouseLastPos;
}

POINT InputHandler::GetMouseDelta() const
{
	auto m = myMouseDelta;
	
	return m;
}

LRESULT InputHandler::UpdateEvents(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_KEYDOWN:
		myKeysTentativeState[wParam] = true;
		return 0;

	case WM_KEYUP:
		myKeysTentativeState[wParam] = false;
		return 0;

	case WM_LBUTTONDOWN:
		myKeysTentativeState[VK_LBUTTON] = true;
		return 0;

	case WM_LBUTTONUP:
		myKeysTentativeState[VK_LBUTTON] = false;
		return 0;

	case WM_RBUTTONDOWN:
		myKeysTentativeState[VK_RBUTTON] = true;
		return 0;

	case WM_RBUTTONUP:
		myKeysTentativeState[VK_RBUTTON] = false;
		return 0;

	case WM_MBUTTONDOWN:
		myKeysTentativeState[VK_MBUTTON] = true;
		return 0;

	case WM_MBUTTONUP:
		myKeysTentativeState[VK_MBUTTON] = false;
		return 0;

	case WM_SYSKEYDOWN:
		myKeysTentativeState[wParam] = true;
		return 0;

	case WM_SYSKEYUP:
		myKeysTentativeState[wParam] = false;
		return 0;

	case WM_XBUTTONDOWN:
	{
		const int xButton = GET_XBUTTON_WPARAM(wParam);
		if (xButton == 1)
			myKeysTentativeState[VK_XBUTTON1] = true;
		else
			myKeysTentativeState[VK_XBUTTON2] = true;

		return 0;
	}

	case WM_XBUTTONUP:
	{
		const int xButton = GET_XBUTTON_WPARAM(wParam);
		if (xButton == 1)
			myKeysTentativeState[VK_XBUTTON1] = false;
		else
			myKeysTentativeState[VK_XBUTTON2] = false;
		return 0;
	}
	case WM_MOUSEWHEEL:
		myTentativeMouseWheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		return 0;

	case WM_MOUSEMOVE:
		const int xPos = LOWORD(lParam);
		const int yPos = HIWORD(lParam);

		myMousePos.x = static_cast<long>(xPos);
		myMousePos.y = static_cast<long>(yPos);
		//myMouseDelta.x = myMousePos.x - myMouseLastPos.x;
		//myMouseDelta.y = myMousePos.y - myMouseLastPos.y;
		//myMouseLastPos = myMousePos;
		return 0;

	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

void InputHandler::Update()
{
	myMouseWheelDelta = myTentativeMouseWheelDelta / abs(myTentativeMouseWheelDelta);
	myTentativeMouseWheelDelta = 0;

	myKeysPreviousState = myKeysCurrentState;
	myKeysCurrentState = myKeysTentativeState;

	myMouseDelta.x = myMousePos.x - myMouseLastPos.x;
	myMouseDelta.y = myMousePos.y - myMouseLastPos.y;
	myMouseLastPos = myMousePos;
}


