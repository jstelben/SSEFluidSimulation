#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include "TextureClass.h"

class ParticleSystemClass
{
public:
	ParticleSystemClass(void);
	~ParticleSystemClass(void);
	bool Initialize(ID3D11Device*, WCHAR*);
	void Shutdown();
	bool Frame(float, ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	ID3D11ShaderResourceView* GetTexture();
	int GetIndexCount();

private:
	struct ParticleType
	{
		float positionX, positionY, positionZ;
		float red, green, blue;
		float velocity;
		bool active;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR4 color;
	};

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();
	bool InitializeParticleSystem();
	void ShutdownParticleSystem();
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void EmitParticles(float);
	void UpdateParticles(float);
	void KillParticles();
	bool UpdateBuffers(ID3D11DeviceContext*);
	void RenderBuffers(ID3D11DeviceContext*);

	float ParticleDeviationX, ParticleDeviationY, ParticleDeviationZ;
	float ParticleVelocity, ParticleVelocityVariation;
	float ParticleSize, ParticlesPerSecond;
	int MaxParticles;
	int CurrentParticleCount;
	float AccumulatedTime;
	TextureClass* Texture;
	ParticleType* ParticleList;
	int VertexCount, IndexCount;
	VertexType* Vertices;
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;


};

