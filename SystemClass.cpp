#include "SystemClass.h"


SystemClass::SystemClass(void)
{
	Input = nullptr;
	Graphics = nullptr;
	Fps = nullptr;
	Cpu = nullptr;
	Timer = nullptr;
}

SystemClass::SystemClass(const SystemClass& other)
{
	Input = other.Input;
	Graphics = other.Graphics;
}

SystemClass::~SystemClass(void)
{
}

bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;
	bool result;
	InitializeWindows(screenWidth, screenHeight);
	ShowCursor(true);

	Input = new InputClass();
	if(!Input)
	{
		return false;
	}
	result = Input->Initialize(HInstance, Hwnd, screenWidth, screenHeight);
	if(!result)
	{
		MessageBox(Hwnd, L"Could not initialize input object.", L"Error", MB_OK);
		return false;
	}
	Graphics = new GraphicsClass();
	if(!Graphics)
	{
		return false;
	}
	result = Graphics->Initialize(screenWidth, screenHeight, Hwnd);
	if(!result)
	{
		return false;
	}

	Fps = new FpsClass();
	if(!Fps)
	{
		return false;
	}
	Fps->Initialize();

	Cpu = new CpuClass();
	if(!Cpu)
	{
		return false;
	}
	Cpu->Initialize();

	Timer = new TimerClass();
	if(!Timer)
	{
		return Timer;
	}
	result = Timer->Initialize();
	if(!result)
	{
		MessageBox(Hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}
	return result;
}

void SystemClass::Shutdown()
{
	if(Timer)
	{
		delete Timer;
		Timer = nullptr;
	}
	if(Cpu)
	{
		Cpu->Shutdown();
		delete Cpu;
		Cpu = nullptr;
	}
	if(Fps)
	{
		delete Fps;
		Fps = nullptr;
	}
	if(Graphics)
	{
		Graphics->Shutdown();
		delete Graphics;
		Graphics = nullptr;
	}

	if(Input)
	{
		Input->Shutdown();
		delete Input;
		Input = nullptr;
	}

	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done = false;
	bool result;

	ZeroMemory(&msg, sizeof(MSG));

	while(!done)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}
		if(Input->IsEscapePressed())
		{
			done = true;
		}
		if(Input->IsXPressed())
		{
			Graphics->ResetDensity();
		}
		if(Input->Is1Pressed())
		{
			Graphics->SetIteration(1);
		}
		if(Input->Is2Pressed())
		{
			Graphics->SetIteration(2);
		}
		if(Input->Is3Pressed())
		{
			Graphics->SetIteration(3);
		}
	}
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;

	Fps->Frame();
	Cpu->Frame();
	Timer->Frame();


	result = Input->Frame();
	if(!result)
	{
		return false;
	}

	if(Input->IsLeftMouseButtonPressed())
	{
		Input->GetMouseLocation(mouseX, mouseY);
		Input->SetMouseUp();
	}
	else
	{
		mouseX = -1;
		mouseY = -1;
	}
	result = Graphics->Frame(mouseX, mouseY, Fps->GetFps(), Cpu->GetCpuPercentage(), Timer->GetTime());
	if(!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
		switch(msg)
		{
		case WM_LBUTTONDOWN:
			{
				Input->SetMouseDown(wparam, ((int)(short)LOWORD(lparam)), ((int)(short)HIWORD(lparam)));
			}
		case WM_MOUSEMOVE:
			{
				Input->MouseLocationChanged(wparam, ((int)(short)LOWORD(lparam)), ((int)(short)HIWORD(lparam)));
			}
		}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX;
	int posY;

	ApplicationHandle = this;

	HInstance = GetModuleHandle(NULL);

	ApplicationName = L"DirectX11Demo";

	wc.style =  CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = HInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ApplicationName;
	wc. cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if(FULL_SCREEN == true)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long) screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long) screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = 0;
		posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	Hwnd = CreateWindowEx(WS_EX_APPWINDOW, ApplicationName, ApplicationName, 
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, 
		screenWidth, screenHeight, NULL, NULL, HInstance, NULL);

	ShowWindow(Hwnd, SW_SHOW);
	SetForegroundWindow(Hwnd);
	SetFocus(Hwnd);

	ShowCursor(false);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(Hwnd);
	Hwnd = NULL;
	
	UnregisterClass(ApplicationName, HInstance);
	HInstance = NULL;

	ApplicationHandle = nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch(message)
	{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
		case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}
		case WM_LBUTTONDOWN:
			{
				
			}
		case WM_MOUSEMOVE:
			{

			}
		default:
			{
				return ApplicationHandle->MessageHandler(hwnd, message, wparam, lparam);
			}
	}
}