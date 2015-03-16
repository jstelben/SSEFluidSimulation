#pragma once

#include <Windows.h>
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorShaderClass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"
#include "FluidGridClass.h"
#include "TextClass.h"
#include <xmmintrin.h>

static const bool FULL_SCREEN = false;
static const bool VSYNC_ENABLED = false;
static const float SCREEN_DEPTH = 1000.0f;
static const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass(void);
	~GraphicsClass(void);

	bool Initialize(int, int, HWND);
	void Shutdown(void);
	bool Frame(int, int, int, int, float);
	void ResetDensity();
	void SetIteration(int);

private:
	bool Render(void);
	bool Render(float);

	D3DClass* D3D;
	CameraClass* Camera;
	ColorShaderClass* ColorShader;
	TextureShaderClass* TextureShader;
	LightShaderClass* LightShader;
	LightClass* Light;
	FluidGridClass* FluidGrid;
	_declspec(align(16)) float* PreviousDensity;
	_declspec(align(16)) float* PreviousVelocityX;
	_declspec(align(16)) float* PreviousVelocityY;
	TextClass* Text;
	int Iteration;
};
