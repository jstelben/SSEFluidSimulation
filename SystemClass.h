#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include "InputClass.h"
#include "GraphicsClass.h"
#include "FpsClass.h"
#include "CpuClass.h"
#include "TimerClass.h"

class SystemClass
{
public:
	SystemClass(void);
	SystemClass::SystemClass(const SystemClass&);
	~SystemClass(void);

	bool Initialize(void);
	void Run(void);
	void Shutdown(void);

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame(void);
	void InitializeWindows(int&, int&);
	void ShutdownWindows(void);

private:
	LPCWSTR ApplicationName;
	HINSTANCE HInstance;
	HWND Hwnd;

	InputClass* Input;
	GraphicsClass* Graphics;
	FpsClass* Fps;
	CpuClass* Cpu;
	TimerClass* Timer;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = nullptr;

