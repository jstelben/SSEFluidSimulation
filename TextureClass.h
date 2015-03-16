#pragma once

#include <D3D11.h>
#include <D3DX11tex.h>

class TextureClass
{
public:
	TextureClass(void);
	~TextureClass(void);

	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* Texture;
};

