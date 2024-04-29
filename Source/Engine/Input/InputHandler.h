#pragma once
#include <windows.h>
#include <bitset>

#define VK_KEY_W 0x57
#define VK_KEY_S 0x53
#define VK_KEY_D 0x44
#define VK_KEY_A 0x41

class InputHandler
{
public:
	InputHandler();
	void SetHWND(HWND aHandle);
	HWND GetHWND();
	~InputHandler();
	bool IsKeyDown(const int aKeyCode) const;
	bool IsKeyPressed(const int aKeyCode) const;
	bool IsKeyReleased(const int aKeyCode)const;

	bool IsMouseButtonDown(const int aKeyCode)const;
	bool IsMouseButtonReleased(const int aKeyCode)const;
	float GetMouseWheelData();
	POINT GetMousePosition() const;
	POINT GetMouseLastPos() const;
	POINT GetMouseDelta() const;
	POINT GetMouseScreenPosition() const;
	POINT GetMouseWindowPosition() const;

	LRESULT UpdateEvents(HWND hwnd,UINT message, WPARAM wParam, LPARAM lParam);

	void Update();

private:
	HWND myWindowHandle;
	float myTentativeMouseWheelDelta;
	float myMouseWheelDelta;

	POINT myMousePosTemp{};
	LPPOINT myMousePosTempPtr{};

	POINT myMousePos;
	POINT myMouseDelta;
	POINT myMouseLastPos;
	std::bitset<256> myKeysCurrentState{};
	std::bitset<256> myKeysPreviousState{};
	std::bitset<256> myKeysTentativeState{};
};
