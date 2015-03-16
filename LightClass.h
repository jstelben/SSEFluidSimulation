#pragma once

#include <D3DX10math.h>

class LightClass
{
public:
	LightClass(void);
	~LightClass(void);

	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();

public:
	D3DXVECTOR4 DiffuseColor;
	D3DXVECTOR3 Direction;
};

