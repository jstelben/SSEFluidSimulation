#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <fstream>
#include "TextureClass.h"

class FontClass
{
public:
	FontClass(void);
	~FontClass(void);
	bool Initialize(ID3D11Device*, char*, WCHAR*);
	void Shutdown();
	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	FontType* Font;
	TextureClass* Texture;
};
