#pragma once

#define DIRECTINPUT_VERSION 0x800
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <windows.h>

class InputClass
{
public:
	InputClass(void);
	~InputClass(void);

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();
	void SetMouseDown(WPARAM, int, int);
	void SetMouseUp();
	void MouseLocationChanged(WPARAM, int, int);

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);
	bool IsLeftMouseButtonPressed();
	bool IsXPressed();
	bool Is1Pressed();
	bool Is2Pressed();
	bool Is3Pressed();

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* DirectInput;
	IDirectInputDevice8* Keyboard;
	unsigned char KeyboardState[256];
	int ScreenWidth;
	int ScreenHeight;
	int MouseX; int MouseY;
	bool MouseDown;
};

