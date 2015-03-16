#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include "TextureClass.h"

class BitmapClass
{
public:
	BitmapClass(void);
	~BitmapClass(void);
	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);
	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	int VertexCount;
	int IndexCount;
	TextureClass* Texture;
	int ScreenWidth, ScreenHeight;
	int BitmapWidth, BitmapHeight;
	int PreviousPositionX, PreviousPositionY;
};

