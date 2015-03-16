#include "GraphicsClass.h"


GraphicsClass::GraphicsClass(void)
{
	D3D = nullptr;
	Camera = nullptr;
	ColorShader = nullptr;
	TextureShader = nullptr;
	LightShader = nullptr;
	Light = nullptr;
	FluidGrid = nullptr;
	PreviousDensity = nullptr;
	PreviousVelocityX = nullptr;
	PreviousVelocityY = nullptr;
	Text = nullptr;
	Iteration = 1;
}


GraphicsClass::~GraphicsClass(void)
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	D3DXMATRIX baseViewMatrix;

	D3D = new D3DClass();
	if(!D3D)
	{
		return false;
	}
	result = D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	Camera = new CameraClass();
	if(!Camera)
	{
		return false;
	}
	Camera->SetPosition(0.0f, -2.0f, -10.0f);
	Camera->Render();
	Camera->GetViewMatrix(baseViewMatrix);

	ColorShader = new ColorShaderClass();
	if(!ColorShader)
	{
		return false;
	}
	result = ColorShader->Initialize(D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object", L"Error", MB_OK);
		return false;
	}

	TextureShader = new TextureShaderClass();
	if(!TextureShader)
	{
		return false;
	}
	result = TextureShader->Initialize(D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	FluidGrid = new FluidGridClass();
	if(!FluidGrid)
	{
		return false;
	}
	result = FluidGrid->Initialize(D3D->GetDevice(), screenWidth, screenHeight, 256, 256);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	int gridSize = FluidGrid->GetGridArraySize();
	PreviousDensity = new float[gridSize];
	PreviousVelocityX = new float[gridSize];
	PreviousVelocityY = new float[gridSize];
	for(int i = 0; i < gridSize; i++)
	{
		PreviousDensity[i] = 0;
		PreviousVelocityX[i] = 0;
		PreviousVelocityY[i] = 0;
	}



	Text = new TextClass();
	if(!Text)
	{
		return false;
	}
	result = Text->Initialize(D3D->GetDevice(), D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}
	return true;
}

void GraphicsClass::Shutdown()
{
	if(Text)
	{
		Text->Shutdown();
		delete Text;
		Text = nullptr;
	}
	if(PreviousVelocityY)
	{
		delete[] PreviousVelocityY;
		PreviousVelocityY = nullptr;
	}
	if(PreviousVelocityX)
	{
		delete[] PreviousVelocityX;
		PreviousVelocityX = nullptr;
	}
	if(PreviousDensity)
	{
		delete[] PreviousDensity;
		PreviousDensity = nullptr;
	}
	if(FluidGrid)
	{
		FluidGrid->Shutdown();
		delete FluidGrid;
		FluidGrid = nullptr;
	}
	if(TextureShader)
	{
		TextureShader->Shutdown();
		delete TextureShader;
		TextureShader = nullptr;
	}
	if(ColorShader)
	{
		ColorShader->Shutdown();
		delete ColorShader;
		ColorShader = nullptr;
	}

	if(Camera)
	{
		delete Camera;
		Camera = nullptr;
	}

	if(D3D)
	{
		D3D->Shutdown();
		delete D3D;
		D3D = nullptr;
	}
}

bool GraphicsClass::Frame(int mouseX, int mouseY, int fps, int cpu, float frameTime)
{
	bool result;
	result = Text->SetFps(fps, D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	result = Text->SetCpu(cpu, D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	result = Text->SetIteration(Iteration, D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	if(mouseX != -1)
	{
		PreviousDensity[FluidGrid->IndexOfMouseClick(mouseX, mouseY)] += 200.0f;
	}
	switch(Iteration)
	{
		case 1:
			{
				FluidGrid->Frame(PreviousDensity, PreviousVelocityX, PreviousVelocityY, frameTime);
				break;
			}
		case 2:
			{
				FluidGrid->BadSSEFrame(PreviousDensity, PreviousVelocityX, PreviousVelocityY, frameTime);
				break;
			}
		case 3:
			{
				FluidGrid->SSEFrame(PreviousDensity, PreviousVelocityX, PreviousVelocityY, frameTime);
				break;
			}
	}
	result = Render();
	if(!result)
	{
		return result;
	}

	Sleep(50);
	return true;
}

bool GraphicsClass::Render()
{
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;
	D3DXMATRIX orthoMatrix;
	bool result;

	D3D->BeginScene(0.4f, 0.611f, 0.94f, 1.0f);

	Camera->Render();

	Camera->GetViewMatrix(viewMatrix);
	D3D->GetWorldMatrix(worldMatrix);
	D3D->GetProjectionMatrix(projectionMatrix);
	D3D->GetOrthoMatrix(orthoMatrix);

	D3D->TurnZBufferOff();
	
	result = FluidGrid->Render(D3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	result = ColorShader->Render(D3D->GetDeviceContext(), FluidGrid->GetIndexCount(), worldMatrix, viewMatrix,
		orthoMatrix);
	if(!result)
	{
		return false;
	}

	D3D->TurnOnAlphaBlending();
	result = Text->Render(D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	D3D->TurnOffAlphaBlending();
	
	D3D->TurnZBufferOn();

	D3D->EndScene();
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	D3DXMATRIX viewMatrix;
	D3DXMATRIX projectionMatrix;
	D3DXMATRIX worldMatrix;

	D3D->BeginScene(0.4f, 0.611f, 0.94f, 1.0f);

	Camera->Render();

	Camera->GetViewMatrix(viewMatrix);
	D3D->GetWorldMatrix(worldMatrix);
	D3D->GetProjectionMatrix(projectionMatrix);
	D3DXMatrixRotationY(&worldMatrix, rotation);

	D3D->EndScene();
	return true;
}

void GraphicsClass::ResetDensity()
{
	int size = FluidGrid->GetGridArraySize();
	for(int i = 0; i < size; i++)
	{
		PreviousDensity[i] = 0;
	}
}

void GraphicsClass::SetIteration(int iter)
{
	Iteration = iter;
}
