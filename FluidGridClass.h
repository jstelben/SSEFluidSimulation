#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <iostream>
#include <xmmintrin.h>

#define ROW_LENGTH 50
#define IX(i,j) ((i)+(ROW_LENGTH+2)*(j));
#define SWAP(x0,x) {float* tmp=x0;x0=x;x=tmp;}

class FluidGridClass
{
public:
	FluidGridClass(void);
	~FluidGridClass(void);
	bool Initialize(ID3D11Device*, int, int, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*);
	int GetIndexCount();
	void Frame(float*, float*, float*, float);
	void SSEFrame(float*, float*, float*, float);
	void BadSSEFrame(float*, float*, float*, float);
	int GetGridArraySize();
	int GetIndex(int, int);
	int IndexOfMouseClick(int, int);

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};
	struct FluidBlock
	{
		D3DXVECTOR2 velocity;
		float density;
		float pressure;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);
	
	
	void DensityStep(float*, float*, float*, float*, float, float);
	void AddDensitySource(float*, float*, float);
	void Diffuse(int, float*, float*, float, float);
	void SetBoundries(int, float*);
	void Advect(int, float*, float*, float*, float*, float);
	void VelocityStep(float*, float*, float*, float*, float, float);
	void Project(float*, float*, float*, float*);
	void SSEDensityStep(float*, float*, float*, float*, float, float);
	void SSEAddDensitySource(float*, float*, float);
	void SSEDiffuse(int, float*, float*, float, float);
	void SSESetBoundries(int, float*);
	void SSEAdvect(int, float*, float*, float*, float*, float);
	void SSEVelocityStep(float*, float*, float*, float*, float, float);
	void SSEProject(float*, float*, float*, float*);
	void BadSSEDensityStep(float*, float*, float*, float*, float, float);
	void BadSSEVelocityStep(float*, float*, float*, float*, float, float);
	void BadSSEDiffuse(int, float*, float*, float, float);
	D3DXVECTOR4 GetColorFromDensity(float);


	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	int VertexCount;
	int IndexCount;
	int ScreenWidth, ScreenHeight;
	int BitmapWidth, BitmapHeight;
	int PreviousPositionX, PreviousPositionY;
	
	//Extra length for boundry computations
	static const int size = (ROW_LENGTH + 2) * (ROW_LENGTH + 2);
	_declspec(align(16)) float VelocityX[size];
	_declspec(align(16)) float VelocityY[size];
	_declspec(align(16)) float Density[size];
};

