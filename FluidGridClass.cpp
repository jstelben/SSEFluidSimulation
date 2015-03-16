#include "FluidGridClass.h"


FluidGridClass::FluidGridClass(void)
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}


FluidGridClass::~FluidGridClass(void)
{
}

bool FluidGridClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
							 int bitmapWidth, int bitmapHeight)
{
	bool result;

	ScreenWidth = screenWidth;
	ScreenHeight = screenHeight;
	BitmapWidth = bitmapWidth;
	BitmapHeight = bitmapHeight;
	PreviousPositionX = -1;
	PreviousPositionY = -1;

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	for(int i = 0; i < size; i++)
	{
		Density[i] = 1;
		VelocityX[i] = 0;
		VelocityY[i] = 0;
	}

	return true;
}

void FluidGridClass::Shutdown()
{
	ShutdownBuffers();
}

bool FluidGridClass::Render(ID3D11DeviceContext* deviceContext)
{
	bool result;
	result = UpdateBuffers(deviceContext);
	if(!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);
	return true;
}

int FluidGridClass::GetIndexCount()
{
	return IndexCount;
}

bool FluidGridClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//6 vertices for each grid block
	VertexCount = 6 * (ROW_LENGTH * ROW_LENGTH);
	IndexCount = VertexCount;

	vertices = new VertexType[VertexCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[IndexCount];
	if(!indices)
	{
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * VertexCount));
	for(int i = 0; i < IndexCount; i++)
	{
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &VertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &IndexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void FluidGridClass::ShutdownBuffers()
{
	if(IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}

	if(VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}
}


bool FluidGridClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	float left, right, top, bottom, blockWidth, blockHeight, currentX, currentY;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;
	int vertexIndex;

	left = (float)((ScreenWidth / 2) * -1);
	right = left + (float)BitmapWidth;
	top = (float)(ScreenHeight / 2);
	bottom = top - (float)BitmapHeight;
	vertices = new VertexType[VertexCount];
	if(!vertices)
	{
		return false;
	}
	
	blockWidth = (float)(ScreenWidth / ROW_LENGTH);
	blockHeight = (float)(ScreenHeight / ROW_LENGTH);
	currentX  = (float)((ScreenWidth / 2) * -1);
	currentY  = (float)(ScreenHeight / 2);
	vertexIndex = 0;
	for(int i = 0; i < ROW_LENGTH; i++)
	{
		for(int j = 0; j < ROW_LENGTH; j++)
		{
			D3DXVECTOR4 color = GetColorFromDensity(Density[GetIndex(i,j)]);
			vertices[vertexIndex].position = D3DXVECTOR3(currentX,currentY,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			vertices[vertexIndex].position = D3DXVECTOR3(currentX+blockWidth,currentY-blockHeight,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			vertices[vertexIndex].position = D3DXVECTOR3(currentX,currentY-blockHeight,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			vertices[vertexIndex].position = D3DXVECTOR3(currentX,currentY,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			vertices[vertexIndex].position = D3DXVECTOR3(currentX+blockWidth,currentY,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			vertices[vertexIndex].position = D3DXVECTOR3(currentX+blockWidth,currentY-blockHeight,0.0f);
			vertices[vertexIndex].color = D3DXVECTOR4(color);
			vertexIndex++;
			currentX += blockWidth;
		}
		currentY -= blockHeight;
		currentX  = (float)((ScreenWidth / 2) * -1);
	}


	result = deviceContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * VertexCount));
	deviceContext->Unmap(VertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void FluidGridClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//Navier Stoke algorithm based on http://www.autodeskresearch.com/pdf/GDC03.pdf
void FluidGridClass::Frame(float* previousDensity, float* preciousVelocityX, float* previousVelocityY, float dt)
{
	VelocityStep(VelocityX, VelocityY, preciousVelocityX, previousVelocityY, 1.0f, dt);
	DensityStep(Density, previousDensity, VelocityX, VelocityY, 1, dt);
}

//Unsuccesful optimization
void FluidGridClass::BadSSEFrame(float* previousDensity, float* preciousVelocityX, float* previousVelocityY, float dt)
{
    BadSSEVelocityStep(VelocityX, VelocityY, preciousVelocityX, previousVelocityY, 1.0f, dt);
	BadSSEDensityStep(Density, previousDensity, VelocityX, VelocityY, 1, dt);
}

//Successful optimizations
void FluidGridClass::SSEFrame(float* previousDensity, float* preciousVelocityX, float* previousVelocityY, float dt)
{
    SSEVelocityStep(VelocityX, VelocityY, preciousVelocityX, previousVelocityY, 1.0f, dt);
	SSEDensityStep(Density, previousDensity, VelocityX, VelocityY, 1, dt);
}

//Using single dimension arrays instead of 2d arrays for the grid
int FluidGridClass::GetIndex(int i, int j)
{
	int index = ((i)+(ROW_LENGTH+2)*(j));
	return index;
}

int FluidGridClass::GetGridArraySize()
{
	return size;
}

//Adds density to grid.  Density added from mouseclick
void FluidGridClass::AddDensitySource(float* x, float* s, float dt)
{
	for(int i = 0; i < size; i++)
	{
		int density = s[i];
		x[i] += dt * density;
	}
}

//Spreads density/velocity to adjacent cells
void FluidGridClass::Diffuse(int b, float* x, float* x0, float diff, float dt)
{
	float a = dt * diff * ROW_LENGTH * ROW_LENGTH;

	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i <= ROW_LENGTH; i++)
		{
			for(int j = 1; j <= ROW_LENGTH; j++)
			{
				float b = x0[GetIndex(i,j)];
				float c = x[GetIndex(i-1,j)];
				float d = x[GetIndex(i+1,j)];
				float e = x[GetIndex(i,j-1)];
				float f = x[GetIndex(i,j+1)];
				float val = (b + a * 
					(c
					+d
					+e
					+f))
					/(1+4*a);
				x[GetIndex(i,j)] = val;
			}
		}
		SetBoundries(b, x);
	}
}

//Stops "flow" from leaving screen
void FluidGridClass::SetBoundries(int b, float* x)
{
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		x[GetIndex(0,i)] = b==1 ? -x[GetIndex(1,i)] : x[GetIndex(1,i)];
		x[GetIndex(ROW_LENGTH+1,i)] = b==1 ? -x[GetIndex(ROW_LENGTH,i)] : x[GetIndex(ROW_LENGTH,i)];
		x[GetIndex(i,0)] = b==2 ? -x[GetIndex(i,1)] : x[GetIndex(i,1)];
		x[GetIndex(i,ROW_LENGTH+1)] = b==2 ? -x[GetIndex(i,ROW_LENGTH)] : x[GetIndex(i,ROW_LENGTH)];
	}
	x[GetIndex(0,0)] = 0.5f * (x[GetIndex(1,0)] + x[GetIndex(0,1)]);
	x[GetIndex(0,ROW_LENGTH+1)] = 0.5f * (x[GetIndex(1,ROW_LENGTH+1)] + x[GetIndex(0,ROW_LENGTH)]);
	x[GetIndex(ROW_LENGTH+1,0)] = 0.5f * (x[GetIndex(ROW_LENGTH,0)] + x[GetIndex(ROW_LENGTH+1,1)]);
	x[GetIndex(ROW_LENGTH+1,ROW_LENGTH+1)] = 0.5f * (x[GetIndex(ROW_LENGTH,ROW_LENGTH+1)]
													+x[GetIndex(ROW_LENGTH+1,ROW_LENGTH)]);
}

//Carries density to new cells
void FluidGridClass::Advect(int b, float* d, float* d0, float* u, float* v, float dt)
{
	float x,y;
	int i0, i1;
	int j0, j1;
	float s0, s1;
	float t0, t1;
	float dt0 = dt * ROW_LENGTH;
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH; j++)
		{
			x = i-dt0*u[GetIndex(i,j)];
			y = j-dt0*v[GetIndex(i,j)];
			if(x < 0.5f)
			{
				x = 0.5f;
			}
			else if(x > ROW_LENGTH + 0.5f)
			{
				x = ROW_LENGTH + 0.5f;
			}
			i0 = (int)x;
			i1 = i0 + 1;
			if(y < 0.5f)
			{
				y = 0.5f;
			}
			else if(y > ROW_LENGTH + 0.5f)
			{
				y = ROW_LENGTH + 0.5f;
			}
			j0 = (int)y;
			j1 = j0 + 1;
			s1 = x-i0;
			s0 = 1-s1;
			t1 = y-j0;
			t0 = 1-t1;
			d[GetIndex(i,j)] = s0 * (t0*d0[GetIndex(i0,j0)]+t1*d0[GetIndex(i0,j1)])
							  +s1 * (t0*d0[GetIndex(i1,j0)]+t1*d0[GetIndex(i1,j1)]);
		}
	}
	SetBoundries(b, d);
}

//Computes new density for each cell
void FluidGridClass::DensityStep(float* x, float* x0, float* u, float* v, float diff, float dt)
{
	AddDensitySource(x, x0, dt);
	SWAP(x0, x);
	Diffuse(0, x, x0, diff, dt);
	SWAP(x0, x);
	Advect(0, x, x0, u, v, dt);
}

//Computes new velocities for each cell
void FluidGridClass::VelocityStep(float* u, float*v, float* u0, float* v0, float viscosity, float dt)
{
	AddDensitySource(u, u0, dt);
	AddDensitySource(v, v0, dt);
	SWAP(u0, u);
	Diffuse(1, u, u0, viscosity, dt);
	SWAP(v0, v);
	Diffuse(2, v, v0, viscosity, dt);
	Project(u, v, u0, v0);
	SWAP(u0, u);
	SWAP(v0, v);
	Advect(1, u, u0, u0, v0, dt);
	Advect(2, v, v0, u0, v0, dt);
	Project(u, v, u0, v0);
}

//Computes change in velocity based on Poisson equation
void FluidGridClass::Project(float* u, float* v, float* p, float* div)
{
	float h = 1.0f / ROW_LENGTH;
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH; j++)
		{
			div[GetIndex(i,j)] = -0.5f * h * (u[GetIndex(i+1,j)]-u[GetIndex(i-1,j)]
											 +v[GetIndex(i,j+1)]-v[GetIndex(i,j-1)]);
			p[GetIndex(i,j)] = 0;
		}
	}
	SetBoundries(0, div);
	SetBoundries(0, p);
	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i <= ROW_LENGTH; i++)
		{
			for(int j = 1; j <= ROW_LENGTH; j++)
			{
				p[GetIndex(i,j)]= (div[GetIndex(i,j)]
								  +p[GetIndex(i-1,j)]
								  +p[GetIndex(i+1,j)]
								  +p[GetIndex(i,j-1)]
					              +p[GetIndex(i,j+1)]) / 4;
			}
		}
		SetBoundries(0, p);
	}
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH; j++)
		{
			u[GetIndex(i,j)] -= 0.5f * (p[GetIndex(i+1,j)]-p[GetIndex(i-1,j)]) / h;
			v[GetIndex(i,j)] -= 0.5f * (p[GetIndex(i,j+1)]-p[GetIndex(i,j-1)]) / h;
		}
	}
	SetBoundries(1, u);
	SetBoundries(2, v);
}

//This optimization is able to calculate new densities, four spaces at a time
void FluidGridClass::SSEAddDensitySource(float* x, float* s, float dt)
{
	_declspec(align(16))  float values[4];
	__m128 dtRegister = _mm_load1_ps(&dt);
	for(int i = 0; i < size; i+=4)
	{
		__m128 previousDensities = _mm_set_ps(s[i], s[i+1], s[i+2], s[i+3]);
		__m128 dtDensityProduct = _mm_mul_ps(previousDensities, dtRegister);
		__m128 densities = _mm_set_ps(x[i], x[i+1], x[i+2], x[i+3]);
		__m128 newDensity = _mm_add_ps(densities, previousDensities);
		_mm_store_ps(&values[0], newDensity);
		for(int j = 0; j < 4; j++)
		{
			x[i + j] = values[j];
		}
	}
}

//Tried to find a way to process 4 rows at the same time.
//When this isn't computed one space at a time, it starts to make a weird checkered pattern
void FluidGridClass::SSEDiffuse(int b, float* x, float* x0, float diff, float dt)
{
	float a = dt * diff * ROW_LENGTH * ROW_LENGTH;

	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i <= ROW_LENGTH; i++)
		{
			for(int j = 1; j <= ROW_LENGTH; j++)
			{
				float b = x0[GetIndex(i,j)];
				float c = x[GetIndex(i-1,j)];
				float d = x[GetIndex(i+1,j)];
				float e = x[GetIndex(i,j-1)];
				float f = x[GetIndex(i,j+1)];
				float val = (b + a * 
					(c
					+d
					+e
					+f))
					/(1+4*a);
				x[GetIndex(i,j)] = val;
			}
		}
		SetBoundries(b, x);
	}
}

//The optimization here processes the boundries at the same time
void FluidGridClass::SSESetBoundries(int b, float* x)
{
	_declspec(align(16)) float values[4];
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		x[GetIndex(0,i)] = b==1 ? -x[GetIndex(1,i)] : x[GetIndex(1,i)];
		x[GetIndex(ROW_LENGTH+1,i)] = b==1 ? -x[GetIndex(ROW_LENGTH,i)] : x[GetIndex(ROW_LENGTH,i)];
		x[GetIndex(i,0)] = b==2 ? -x[GetIndex(i,1)] : x[GetIndex(i,1)];
		x[GetIndex(i,ROW_LENGTH+1)] = b==2 ? -x[GetIndex(i,ROW_LENGTH)] : x[GetIndex(i,ROW_LENGTH)];
	}
	__m128 sums = _mm_set_ps(x[GetIndex(1,0)] + x[GetIndex(0,1)], x[GetIndex(1,ROW_LENGTH+1)] + x[GetIndex(0,ROW_LENGTH)], x[GetIndex(ROW_LENGTH,0)] + x[GetIndex(ROW_LENGTH+1,1)], x[GetIndex(ROW_LENGTH,ROW_LENGTH+1)] +x[GetIndex(ROW_LENGTH+1,ROW_LENGTH)]);
	__m128 constant = _mm_load_ps(new float(0.5f));
	__m128 product = _mm_mul_ps(sums, constant);
	_mm_store_ps(&values[0], product);
	x[GetIndex(0,0)] = values[0];
	x[GetIndex(0,ROW_LENGTH+1)] = values[1];
	x[GetIndex(ROW_LENGTH+1,0)] = values[2];
	x[GetIndex(ROW_LENGTH+1,ROW_LENGTH+1)] = values[3];
}

void FluidGridClass::SSEAdvect(int b, float* d, float* d0, float* u, float* v, float dt)
{
	float x,y;
	int i0, i1;
	int j0, j1;
	float s0, s1;
	float t0, t1;
	float dt0 = dt * ROW_LENGTH;
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH; j++)
		{
			x = i-dt0*u[GetIndex(i,j)];
			y = j-dt0*v[GetIndex(i,j)];
			if(x < 0.5f)
			{
				x = 0.5f;
			}
			else if(x > ROW_LENGTH + 0.5f)
			{
				x = ROW_LENGTH + 0.5f;
			}
			i0 = (int)x;
			i1 = i0 + 1;
			if(y < 0.5f)
			{
				y = 0.5f;
			}
			else if(y > ROW_LENGTH + 0.5f)
			{
				y = ROW_LENGTH + 0.5f;
			}
			j0 = (int)y;
			j1 = j0 + 1;
			s1 = x-i0;
			s0 = 1-s1;
			t1 = y-j0;
			t0 = 1-t1;
			d[GetIndex(i,j)] = s0 * (t0*d0[GetIndex(i0,j0)]+t1*d0[GetIndex(i0,j1)])
							  +s1 * (t0*d0[GetIndex(i1,j0)]+t1*d0[GetIndex(i1,j1)]);
		}
	}
	SetBoundries(b, d);
}

void FluidGridClass::SSEDensityStep(float* x, float* x0, float* u, float* v, float diff, float dt)
{
	SSEAddDensitySource(x, x0, dt);
	SWAP(x0, x);
	SSEDiffuse(0, x, x0, diff, dt);
	SWAP(x0, x);
	SSEAdvect(0, x, x0, u, v, dt);
}

void FluidGridClass::SSEVelocityStep(float* u, float*v, float* u0, float* v0, float viscosity, float dt)
{
	//change name to addsource
	SSEAddDensitySource(u, u0, dt);
	SSEAddDensitySource(v, v0, dt);
	SWAP(u0, u);
	SSEDiffuse(1, u, u0, viscosity, dt);
	SWAP(v0, v);
	SSEDiffuse(2, v, v0, viscosity, dt);
	SSEProject(u, v, u0, v0);
	SWAP(u0, u);
	SWAP(v0, v);
	SSEAdvect(1, u, u0, u0, v0, dt);
	SSEAdvect(2, v, v0, u0, v0, dt);
	SSEProject(u, v, u0, v0);
}

//Was able to process 4 spaces at once during Projection
//During initial system analysis, this was the heaviest computation.
void FluidGridClass::SSEProject(float* u, float* v, float* p, float* div)
{
	_declspec(align(16)) float values[4];
	float h = 1.0f / ROW_LENGTH;
	float h1 = h * -0.5f;
	float fourth = 1.0f / 4.0f;
	__m128 fourConstant = _mm_load1_ps(&fourth);
	__m128 constant = _mm_load1_ps(&h1);
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH - 4; j+=4)
		{
			float sum = u[GetIndex(i+1,j)]-u[GetIndex(i-1,j)]+v[GetIndex(i,j+1)]-v[GetIndex(i,j-1)];
			float sum1 = u[GetIndex(i+1,j+1)]-u[GetIndex(i-1,j+1)]+v[GetIndex(i,j+2)]-v[GetIndex(i,j)];
			float sum2 = u[GetIndex(i+1,j+2)]-u[GetIndex(i-1,j+2)]+v[GetIndex(i,j+3)]-v[GetIndex(i,j+1)];
			float sum3 = u[GetIndex(i+1,j+3)]-u[GetIndex(i-1,j+3)]+v[GetIndex(i,j+4)]-v[GetIndex(i,j+2)];
			__m128 sums = _mm_set_ps(sum, sum1, sum2, sum3);
			__m128 product = _mm_mul_ps(sums, constant);
			_mm_store_ps(&values[0], product);
			div[GetIndex(i,j)] = values[0];
			div[GetIndex(i,j+1)] = values[1];
			div[GetIndex(i,j+2)] = values[2];
			div[GetIndex(i,j+3)] = values[3];
			p[GetIndex(i,j)] = 0;
			p[GetIndex(i,j+1)] = 0;
			p[GetIndex(i,j+2)] = 0;
			p[GetIndex(i,j+3)] = 0;
		}
	}
	SetBoundries(0, div);
	SetBoundries(0, p);
	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i <= ROW_LENGTH; i++)
		{
			for(int j = 1; j <= ROW_LENGTH - 4; j+=4)
			{
				float sum = p[GetIndex(i-1,j)]+p[GetIndex(i+1,j)]+p[GetIndex(i,j-1)]+p[GetIndex(i,j+1)];
				float sum1 = p[GetIndex(i-1,j+1)]+p[GetIndex(i+1,j+1)]+p[GetIndex(i,j)]+p[GetIndex(i,j+2)];
				float sum2 = p[GetIndex(i-1,j+2)]+p[GetIndex(i+1,j+2)]+p[GetIndex(i,j+1)]+p[GetIndex(i,j+3)];
				float sum3 = p[GetIndex(i-1,j+3)]+p[GetIndex(i+1,j+3)]+p[GetIndex(i,j+2)]+p[GetIndex(i,j+4)];
				__m128 sums = _mm_set_ps(sum, sum1, sum2, sum3);
				__m128 divValues = _mm_set_ps(div[GetIndex(i,j)], div[GetIndex(i,j+1)], div[GetIndex(i,j+2)], div[GetIndex(i,j+3)]); 
				__m128 divSums = _mm_add_ps(sums, divValues);
				__m128 product = _mm_mul_ps(divSums, fourConstant);
				_mm_store_ps(&values[0], product);
				p[GetIndex(i,j)] = values[0];
				p[GetIndex(i,j+1)] = values[1];
				p[GetIndex(i,j+2)] = values[2];
				p[GetIndex(i,j+3)] = values[3];
			}
		}
		SetBoundries(0, p);
	}
	for(int i = 1; i <= ROW_LENGTH; i++)
	{
		for(int j = 1; j <= ROW_LENGTH; j++)
		{
			u[GetIndex(i,j)] -= 0.5f * (p[GetIndex(i+1,j)]-p[GetIndex(i-1,j)]) / h;
			v[GetIndex(i,j)] -= 0.5f * (p[GetIndex(i,j+1)]-p[GetIndex(i,j-1)]) / h;
		}
	}
	SetBoundries(1, u);
	SetBoundries(2, v);
}

void FluidGridClass::BadSSEDensityStep(float* x, float* x0, float* u, float* v, float diff, float dt)
{
	SSEAddDensitySource(x, x0, dt);
	SWAP(x0, x);
	BadSSEDiffuse(0, x, x0, diff, dt);
	SWAP(x0, x);
	SSEAdvect(0, x, x0, u, v, dt);
}

void FluidGridClass::BadSSEVelocityStep(float* u, float*v, float* u0, float* v0, float viscosity, float dt)
{
	SSEAddDensitySource(u, u0, dt);
	SSEAddDensitySource(v, v0, dt);
	SWAP(u0, u);
	BadSSEDiffuse(1, u, u0, viscosity, dt);
	SWAP(v0, v);
	BadSSEDiffuse(2, v, v0, viscosity, dt);
	SSEProject(u, v, u0, v0);
	SWAP(u0, u);
	SWAP(v0, v);
	SSEAdvect(1, u, u0, u0, v0, dt);
	SSEAdvect(2, v, v0, u0, v0, dt);
	SSEProject(u, v, u0, v0);
}

//My attempt at using intrinsics to run parallel computations.
//The issue occurs that each computation takes the previous solution as a factor.
//When the algorithm runs in parallel, the computed solutions are not factored in.
void FluidGridClass::BadSSEDiffuse(int b, float* x, float* x0, float diff, float dt)
{
	_declspec(align(16)) float values[4];
	float a = dt * diff * ROW_LENGTH * ROW_LENGTH;
	float aInverse = 1.0f / (1 + 4 * a);
	__m128 diffusion = _mm_load1_ps(&a);
	__m128 diffusionInverse = _mm_load1_ps(&aInverse);
	for(int k = 0; k < 10; k++)
	{
		for(int i = 1; i <= ROW_LENGTH; i++)
		{
			for(int j = 1; j <= ROW_LENGTH - 4; j+=4)
			{
				__m128 initialX = _mm_set_ps(x0[GetIndex(i,j)], x0[GetIndex(i,j+1)], x0[GetIndex(i,j+2)], x0[GetIndex(i,j+3)]);
				__m128 c = _mm_set_ps(x[GetIndex(i-1,j)], x[GetIndex(i-1,j+1)], x[GetIndex(i-1,j+2)], x[GetIndex(i-1,j+3)]);
				__m128 d = _mm_set_ps(x[GetIndex(i+1,j)], x[GetIndex(i+1,j+1)], x[GetIndex(i+1,j+2)], x[GetIndex(i+1,j+3)]);
				__m128 e = _mm_set_ps(x[GetIndex(i,j-1)], x[GetIndex(i,j)], x[GetIndex(i,j+1)], x[GetIndex(i,j+2)]);
				__m128 f = _mm_set_ps(x[GetIndex(i,j+1)], x[GetIndex(i,j+2)], x[GetIndex(i,j+3)], x[GetIndex(i,j+4)]);

				__m128 sum1 = _mm_add_ps(c,d);
				__m128 sum2 = _mm_add_ps(e,f);
				__m128 sum3 = _mm_add_ps(sum1, sum2);

				__m128 product1 = _mm_mul_ps(diffusion, sum3);
				__m128 sum4 = _mm_add_ps(product1, initialX);
				__m128 product2 = _mm_mul_ps(sum4, diffusionInverse);
				_mm_store_ps(&values[0], product2);
				x[GetIndex(i,j)] = values[0];
				x[GetIndex(i,j+1)] = values[1];
				x[GetIndex(i,j+2)] = values[2];
				x[GetIndex(i,j+3)] = values[3];
			}
		}
		SetBoundries(b, x);
	}
}

//Low density returns a blue, higher densities are more red
D3DXVECTOR4 FluidGridClass::GetColorFromDensity(float density)
{
	float red = density / 1.0f;
	if(red > 1.0f)
	{
		red = 1.0f;
	}
	float blue = 1.0f - density / 1.0f;
	return D3DXVECTOR4(red, 0.0f, blue, 1.0f);
}

//Gets grid space based on mouse coords
int FluidGridClass::IndexOfMouseClick(int x, int y)
{
	float blockWidth = (float)(ScreenWidth / ROW_LENGTH);
	float blockHeight = (float)(ScreenHeight / ROW_LENGTH);
	int column = (int) (x / blockWidth);
	int row = (int) (y / blockHeight);
	int index = GetIndex(row, column);
	return index;
}