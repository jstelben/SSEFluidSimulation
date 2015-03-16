#include "ParticleSystemClass.h"


ParticleSystemClass::ParticleSystemClass(void)
{
	Texture = nullptr;
	ParticleList = nullptr;
	Vertices = nullptr;
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
}


ParticleSystemClass::~ParticleSystemClass(void)
{
}

bool ParticleSystemClass::Initialize(ID3D11Device* device, WCHAR* textureFileName)
{
	bool result;
	result = LoadTexture(device, textureFileName);
	if(!result)
	{
		return false;
	}
	result = InitializeParticleSystem();
	if(!result)
	{
		return false;
	}
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}
	return true;
}

void ParticleSystemClass::Shutdown()
{
	ShutdownBuffers();
	ShutdownParticleSystem();
	ReleaseTexture();
}

bool ParticleSystemClass::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;

	KillParticles();
	EmitParticles(frameTime);
	UpdateParticles(frameTime);
	result = UpdateBuffers(deviceContext);
	if(!result)
	{
		return false;
	}
	return true;
}

void ParticleSystemClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

ID3D11ShaderResourceView* ParticleSystemClass::GetTexture()
{
	return Texture->GetTexture();
}

int ParticleSystemClass::GetIndexCount()
{
	return IndexCount;
}

bool ParticleSystemClass::LoadTexture(ID3D11Device* device, WCHAR* fileName)
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

void ParticleSystemClass::ReleaseTexture()
{
	if(Texture)
	{
		Texture->Shutdown();
		delete Texture;
		Texture = nullptr;
	}
}

//Overload this when making new particle systems
bool ParticleSystemClass::InitializeParticleSystem()
{
	ParticleDeviationX = 0.5f;
	ParticleDeviationY = 0.1f;
	ParticleDeviationZ = 2.0f;
	ParticleVelocity = 1.0f;
	ParticleVelocityVariation = 0.2f;
	ParticleSize = 0.2f;
	ParticlesPerSecond = 250.0f;
	MaxParticles = 5000;
	ParticleList = new ParticleType[MaxParticles];
	if(!ParticleList)
	{
		return false;
	}
	for(int i = 0; i < MaxParticles; i++)
	{
		ParticleList[i].active = false;
	}

	CurrentParticleCount = 0;
	AccumulatedTime = 0;

	return true;
}

void ParticleSystemClass::ShutdownParticleSystem()
{
	if(ParticleList)
	{
		delete[] ParticleList;
		ParticleList = nullptr;
	}
}

bool ParticleSystemClass::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	VertexCount = MaxParticles * 6;
	IndexCount = VertexCount;
	Vertices = new VertexType[VertexCount];
	if(Vertices)
	{
		return false;
	}

	indices = new unsigned long[IndexCount];
	if(!indices)
	{
		return false;
	}

	memset(Vertices, 0, (sizeof(VertexType) * VertexCount));
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
	vertexData.pSysMem = Vertices;
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

	delete[] indices;
	indices = nullptr;

	return true;
}

void ParticleSystemClass::ShutdownBuffers()
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

void ParticleSystemClass::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;

	AccumulatedTime += frameTime;
	emitParticle = false;
	if(AccumulatedTime > (1000.0f / ParticlesPerSecond))
	{
		AccumulatedTime = 0.0f;
		emitParticle = true;
	}

	if(emitParticle && (CurrentParticleCount < (MaxParticles - 1)))
	{
		CurrentParticleCount++;

		//gets a random number between -1 and 1, then multiplies by deviation
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * ParticleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * ParticleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * ParticleDeviationZ;

		velocity = ParticleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * ParticleVelocityVariation;

		red =  (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green =  (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue =  (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;

		index = 0;
		found = false;
		while(!found)
		{
			if((ParticleList[index].active == false) || (ParticleList[index].positionZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}
		i = CurrentParticleCount;
		j = i - 1;

		while(i != index)
		{
			ParticleList[i].positionX = ParticleList[j].positionX;
			ParticleList[i].positionY = ParticleList[j].positionY;
			ParticleList[i].positionZ = ParticleList[j].positionZ;
			ParticleList[i].red = ParticleList[j].red;
			ParticleList[i].green = ParticleList[j].green;
			ParticleList[i].blue = ParticleList[j].blue;
			ParticleList[i].velocity = ParticleList[j].velocity;
			ParticleList[i].active = ParticleList[j].active;
			i--;
			j--;
		}

			ParticleList[i].positionX = positionX;
			ParticleList[i].positionY = positionY;
			ParticleList[i].positionZ = positionZ;
			ParticleList[i].red = red;
			ParticleList[i].green = green;
			ParticleList[i].blue = blue;
			ParticleList[i].velocity = velocity;
			ParticleList[i].active = true;
	}
}

void ParticleSystemClass::UpdateParticles(float frameTime)
{
	for(int i = 0; i < CurrentParticleCount; i++)
	{
		ParticleList[i].positionY = ParticleList[i].positionX - (ParticleList[i].velocity * frameTime * 0.001f);
	}
}

void ParticleSystemClass::KillParticles()
{
	for(int i  = 0; i < MaxParticles; i++)
	{
		if((ParticleList[i].active) && (ParticleList[i].positionY < -3.0f))
		{
			ParticleList[i].active = false;
			CurrentParticleCount--;

			for(int j = i; j < MaxParticles -1; j++)
			{
				ParticleList[j].positionX = ParticleList[j + 1].positionX;
				ParticleList[j].positionY = ParticleList[j + 1].positionY;
				ParticleList[j].positionZ = ParticleList[j + 1].positionZ;
				ParticleList[j].red = ParticleList[j + 1].red;
				ParticleList[j].green = ParticleList[j + 1].green;
				ParticleList[j].blue = ParticleList[j + 1].blue;
				ParticleList[j].velocity = ParticleList[j + 1].velocity;
				ParticleList[j].active = ParticleList[j + 1].active;
			}
		}
	}
}

bool ParticleSystemClass::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index = 0;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;

	memset(Vertices, 0, (sizeof(VertexType) * VertexCount));

	for(int i = 0; i < CurrentParticleCount; i++)
	{
		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX - ParticleSize,
			ParticleList[i].positionY - ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;

		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX - ParticleSize,
			ParticleList[i].positionY + ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;

		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX + ParticleSize,
			ParticleList[i].positionY - ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;

		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX + ParticleSize,
			ParticleList[i].positionY - ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;

		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX - ParticleSize,
			ParticleList[i].positionY + ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;

		Vertices[index].position = D3DXVECTOR3(ParticleList[i].positionX + ParticleSize,
			ParticleList[i].positionY + ParticleSize, ParticleList[i].positionZ);
		Vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		Vertices[index].color = D3DXVECTOR4(ParticleList[i].red, ParticleList[i].green, ParticleList[i].blue, 1.0f);
		index++;
	}

	result = deviceContext->Map(VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)Vertices, (sizeof(VertexType) * VertexCount));

	deviceContext->Unmap(VertexBuffer, 0);

	return true;
}

void ParticleSystemClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
