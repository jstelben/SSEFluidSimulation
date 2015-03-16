#include "InputClass.h"


InputClass::InputClass(void)
{
	DirectInput = nullptr;
	Keyboard = nullptr;
	MouseDown = false;
}

InputClass::~InputClass(void)
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;
	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;
	MouseX = 0;
	MouseY = 0;

	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	result = DirectInput->CreateDevice(GUID_SysKeyboard, &Keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}
	result = Keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}
	result = Keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}
	result = Keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}
	return true;
}

void InputClass::Shutdown()
{
	if(Keyboard)
	{
		Keyboard->Unacquire();
		Keyboard->Release();
		Keyboard = nullptr;
	}
	if(DirectInput)
	{
		DirectInput->Release();
		DirectInput = nullptr;
	}
}

bool InputClass::Frame()
{
	bool result;

	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = Keyboard->GetDeviceState(sizeof(KeyboardState), (LPVOID)&KeyboardState);
	if(FAILED(result))
	{
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			Keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool InputClass::ReadMouse()
{
	return true;
}

void InputClass::ProcessInput()
{
}

bool InputClass::IsEscapePressed()
{
	if(KeyboardState[DIK_ESCAPE] & 0x80)
	{
		return true;
	}
	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = MouseX;
	mouseY = MouseY;
}

bool InputClass::IsLeftMouseButtonPressed()
{
	if(MouseDown)
	{
		return true;
	}
	return false;
}

void InputClass::SetMouseDown(WPARAM btnState, int x, int y)
{
	MouseDown = true;
}

void InputClass::SetMouseUp()
{
	MouseDown = false;
}

void InputClass::MouseLocationChanged(WPARAM btnState, int x, int y)
{
	MouseX = x;
	MouseY = y;
}

bool InputClass::IsXPressed()
{
	if(KeyboardState[DIK_X] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is1Pressed()
{
	if(KeyboardState[DIK_1] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is2Pressed()
{
	if(KeyboardState[DIK_2] & 0x80)
	{
		return true;
	}
	return false;
}

bool InputClass::Is3Pressed()
{
	if(KeyboardState[DIK_3] & 0x80)
	{
		return true;
	}
	return false;
}