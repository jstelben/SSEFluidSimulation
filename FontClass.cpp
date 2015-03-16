#include "FontClass.h"


FontClass::FontClass(void)
{
	Font = nullptr;
	Texture = nullptr;
}


FontClass::~FontClass(void)
{
}

bool FontClass::Initialize(ID3D11Device* device, char* fontFileName, WCHAR* textureFileName)
{
	bool result;

	result = LoadFontData(fontFileName);
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

void FontClass::Shutdown()
{
	ReleaseTexture();
	ReleaseFontData();
}

bool FontClass::LoadFontData(char* fileName)
{
	std::ifstream fin;
	char temp;

	Font = new FontType[95];
	if(!Font)
	{
		return false;
	}

	fin.open(fileName);
	if(fin.fail())
	{
		return false;
	}

	for(int i = 0; i < 95; i++)
	{
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}
		fin.get(temp);
		while(temp != ' ')
		{
			fin.get(temp);
		}

		fin >> Font[i].left;
		fin >> Font[i].right;
		fin >> Font[i].size;
	}

	fin.close();
	return true;
}

void FontClass::ReleaseFontData()
{
	if(Font)
	{
		delete[] Font;
		Font = nullptr;
	}
}

bool FontClass::LoadTexture(ID3D11Device* device, WCHAR* fileName)
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

void FontClass::ReleaseTexture()
{
	if(Texture)
	{
		Texture->Shutdown();
		delete Texture;
		Texture = nullptr;
	}
}

ID3D11ShaderResourceView* FontClass:: GetTexture()
{
	return Texture->GetTexture();
}

void FontClass::BuildVertexArray(void* vertices, char* sentence, float drawX, float drawY)
{
	VertexType* vertexPtr;
	int numLetters;
	int index;
	int letter;

	vertexPtr = (VertexType*)vertices;
	numLetters = (int) std::strlen(sentence);
	index = 0;

	for(int i = 0; i < numLetters; i++)
	{
		letter = ((int)sentence[i]) - 32;
		if(letter == 0)
		{
			drawX = drawX + 3.0f;
		}
		else
		{
			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].right, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY - 16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].left, 1.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].left, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + Font[letter].size, drawY, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].right, 0.0f);
			index++;

			vertexPtr[index].position = D3DXVECTOR3(drawX + Font[letter].size, drawY - 16, 0.0f);
			vertexPtr[index].texture = D3DXVECTOR2(Font[letter].right, 1.0f);
			index++;

			drawX = drawX + Font[letter].size + 1.0f;
		}
	}
}