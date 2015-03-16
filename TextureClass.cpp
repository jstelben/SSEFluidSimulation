#include "TextureClass.h"


TextureClass::TextureClass(void)
{
	Texture = nullptr;
}


TextureClass::~TextureClass(void)
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &Texture, NULL);
	if(FAILED(result))
	{
		return false;
	}


	return true;
}

void TextureClass::Shutdown()
{
	if(Texture)
	{
		Texture->Release();
		Texture = nullptr;
	}
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return Texture;
}