#include "ModelClass.h"


ModelClass::ModelClass(void)
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;
	Texture = nullptr;
	Model = nullptr;
}


ModelClass::~ModelClass(void)
{
}


bool ModelClass::Initialize(ID3D11Device* device)
{
	bool result;

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* textureFileName)
{
	bool result;

	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	result = LoadTexture(device, textureFileName);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize(ID3D11Device* device, char* modelFileName, WCHAR* textureFileName)
{
	bool result;

	result = LoadBlenderModel(modelFileName);
	if(!result)
	{
		return false;
	}

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

void ModelClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();
	ReleaseModel();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return IndexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	LightVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc;
	D3D11_BUFFER_DESC indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	D3D11_SUBRESOURCE_DATA indexData;
	HRESULT result;

	vertices = new LightVertexType[VertexCount];
	if(!vertices)
	{
		return false;
	}

	indices = new unsigned long[IndexCount];
	if(!indices)
	{
		return false;
	}

	for(int i = 0; i < VertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(Model[i].x, Model[i].y, Model[i].z);
		vertices[i].texture= D3DXVECTOR2(Model[i].tu, Model[i].tv);
		vertices[i].normal= D3DXVECTOR3(Model[i].nx, Model[i].ny, Model[i].nz);

		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(LightVertexType) * VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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


void ModelClass::ShutdownBuffers()
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

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	if(Texture)
	{
		stride = sizeof(LightVertexType);
	}
	else
	{
		stride = sizeof(VertexType);
	}
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	deviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);


}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	Texture = new TextureClass();
	if(!Texture)
	{
		return false;
	}

	result = Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	if(Texture)
	{
		Texture->Shutdown();
		delete Texture;
		Texture = nullptr;
	}
}
bool ModelClass::LoadModel(char* fileName)
{
	std::ifstream stream;
	char input;
	
	stream.open(fileName);
	if(stream.fail())
	{
		return false;
	}

	stream.get(input);
	while(input != ':')
	{
		stream.get(input);
	}

	stream >> VertexCount;
	IndexCount = VertexCount;

	Model = new ModelType[VertexCount];
	if(!Model)
	{
		return false;
	}

	stream.get(input);
	while(input != ':')
	{
		stream.get(input);
	}
	stream.get(input);
	stream.get(input);

	for(int i = 0; i < VertexCount; i++)
	{
		stream >> Model[i].x >> Model[i].y >> Model[i].z;
		stream >> Model[i].tu >> Model[i].tv;
		stream >> Model[i].nx >> Model[i].ny >> Model[i].nz;
	}

	stream.close();

	return true;
}

bool ModelClass::LoadBlenderModel(char* fileName)
{
	std::ifstream stream;
	char input;
	int vertexCount = 0;
	int texCoordCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	int vertexIndex = 0;
	int textureIndex = 0;
	int normalIndex = 0;
	int faceIndex = 0;
	D3DXVECTOR3* vertices;
	D3DXVECTOR2* texCoords;
	D3DXVECTOR3* normals;
	Face* faces;

	stream.open(fileName);
	if(stream.fail())
	{
		return false;
	}

	stream.get(input);
	while(!stream.eof())
	{
		if(input == 'v')
		{
			stream.get(input);
			if(input == ' ') { vertexCount++;}
			else if (input == 't') { texCoordCount++;}
			else if (input == 'n') { normalCount++;}
		}
		else if(input == 'f')
		{
			stream.get(input);
			if(input == ' ') { faceCount++;}
		}

		while(input != '\n')
		{
			stream.get(input);
		}
		stream.get(input);
	}

	stream.close();

	vertices = new D3DXVECTOR3[vertexCount];
	texCoords = new D3DXVECTOR2[texCoordCount];
	normals = new D3DXVECTOR3[normalCount];
	faces = new Face[faceCount];

	stream.open(fileName);
	if(stream.fail())
	{
		return false;
	}

	stream.get(input);
	while(!stream.eof())
	{
		if(input == 'v')
		{
			stream.get(input);

			if(input == ' ')
			{
				//May need to convert here for left hand system.
				stream >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;
				vertices[vertexIndex].z *= -1.0f;
				vertexIndex++;
			}
			else if(input == 't')
			{
				stream >> texCoords[textureIndex].x >> texCoords[textureIndex].y;
				texCoords[textureIndex].y = 1.0f - texCoords[textureIndex].y;
				textureIndex++;
			}
			else if(input == 'n')
			{
				stream >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				normals[normalIndex].z *= -1.0f;
				normalIndex++;
			}
		}
		else if(input == 'f')
		{
			stream.get(input);
			if(input == ' ')
			{
				char buffer;
				stream >> faces[faceIndex].vertexIndex3 >> buffer >> faces[faceIndex].texIndex3 >> buffer >> faces[faceIndex].normalIndex3
					   >> faces[faceIndex].vertexIndex2 >> buffer >> faces[faceIndex].texIndex2 >> buffer >> faces[faceIndex].normalIndex2
					   >> faces[faceIndex].vertexIndex1 >> buffer >> faces[faceIndex].texIndex1 >> buffer >> faces[faceIndex].normalIndex1;
				faceIndex++;
			}
		}
		while(input != '\n')
		{
			stream.get(input);
		}
		stream.get(input);
	}

	stream.close();

	//each face has 3 indices
	IndexCount = faceCount * 3;
	VertexCount = IndexCount;
	Model = new ModelType[IndexCount];
	if(!Model)
	{
		return false;
	}

	for(int i = 0; i < faceCount; i++)
	{
		int index = i * 3;
		int vertexIndex = faces[i].vertexIndex1 - 1;
		int texCoordIndex = faces[i].texIndex1 - 1;
		int normalIndex = faces[i].normalIndex1 - 1;
		Model[index].x = vertices[vertexIndex].x;
		Model[index].y = vertices[vertexIndex].y;
		Model[index].z = vertices[vertexIndex].z;
		Model[index].tu = texCoords[texCoordIndex].x;
		Model[index].tv = texCoords[texCoordIndex].y;
		Model[index].nx = normals[normalIndex].x;
		Model[index].ny = normals[normalIndex].y;
		Model[index].nz = normals[normalIndex].z;
		index++;
		vertexIndex = faces[i].vertexIndex2 - 1;
		texCoordIndex = faces[i].texIndex2 - 1;
		normalIndex = faces[i].normalIndex2 - 1;
		Model[index].x = vertices[vertexIndex].x;
		Model[index].y = vertices[vertexIndex].y;
		Model[index].z = vertices[vertexIndex].z;
		Model[index].tu = texCoords[texCoordIndex].x;
		Model[index].tv = texCoords[texCoordIndex].y;
		Model[index].nx = normals[normalIndex].x;
		Model[index].ny = normals[normalIndex].y;
		Model[index].nz = normals[normalIndex].z;
		index++;
		vertexIndex = faces[i].vertexIndex3 - 1;
		texCoordIndex = faces[i].texIndex3 - 1;
		normalIndex = faces[i].normalIndex3 - 1;
		Model[index].x = vertices[vertexIndex].x;
		Model[index].y = vertices[vertexIndex].y;
		Model[index].z = vertices[vertexIndex].z;
		Model[index].tu = texCoords[texCoordIndex].x;
		Model[index].tv = texCoords[texCoordIndex].y;
		Model[index].nx = normals[normalIndex].x;
		Model[index].ny = normals[normalIndex].y;
		Model[index].nz = normals[normalIndex].z;
	}
	if(vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}
	if(texCoords)
	{
		delete[] texCoords;
		texCoords = nullptr;
	}
	if(normals)
	{
		delete[] normals;
		normals = nullptr;
	}
	if(faces)
	{
		delete[] faces;
		faces = nullptr;
	}
	return true;
}

void ModelClass::ReleaseModel()
{
	if(Model)
	{
		delete[] Model;
		Model = nullptr;
	}
}
