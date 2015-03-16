#include "BitmapClass.h"


BitmapClass::BitmapClass(void)
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
	Texture = nullptr;
}


BitmapClass::~BitmapClass(void)
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight,
							 WCHAR* textureFileName, int bitmapWidth, int bitmapHeight)
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

	result = LoadTexture(device, textureFileName);
	if(!result)
	{
		return false;
	}

	return true;
}


void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if(!result)
	{
		return false;
	}

	RenderBuffers(deviceContext);
	return true;
}

int BitmapClass::GetIndexCount()
{
	return IndexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return Texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	VertexCount = 6;
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

void BitmapClass::ShutdownBuffers()
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

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	if((positionX == PreviousPositionX) && (positionY == PreviousPositionY))
	{
		return true;
	}

	PreviousPositionX = positionX;
	PreviousPositionY = positionY;

	left = (float)((ScreenWidth / 2) * -1) + (float)positionX;
	right = left + (float)BitmapWidth;
	top = (float)(ScreenHeight / 2) - (float)positionY;
	bottom = top - (float)BitmapHeight;

	vertices = new VertexType[VertexCount];
	if(!vertices)
	{
		return false;
	}

	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);
	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

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

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* fileName)
{
	bool result;
	Texture = new TextureClass();
	if(!Texture)
	{
		return false;
	}
	result = Texture->Initialize(device, fileName);
	if(!result)
	{
		return false;
	}
	return true;
}

void BitmapClass::ReleaseTexture()
{
	if(Texture)
	{
		Texture->Shutdown();
		delete Texture;
		Texture = nullptr;
	}
}

