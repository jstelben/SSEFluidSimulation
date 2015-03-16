#pragma once
#include "FontClass.h"
#include "FontShaderClass.h"
class TextClass
{
public:
	TextClass(void);
	~TextClass(void);
	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);
	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetIteration(int, ID3D11DeviceContext*);

private:
	struct SentenceType
	{
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, D3DXMATRIX, D3DXMATRIX);

	FontClass* Font;
	FontShaderClass* FontShader;
	int ScreenWidth, ScreenHeight;
	D3DXMATRIX BaseMatrixView;
	SentenceType* Sentence1;
	SentenceType* Sentence2;
	SentenceType* Sentence3;
};

