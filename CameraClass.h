#pragma once

#include <D3DX10math.h>

class CameraClass
{
public:
	CameraClass(void);
	~CameraClass(void);

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float PositionX;
	float PositionY;
	float PositionZ;
	float RotationX;
	float RotationY;
	float RotationZ;
	D3DXMATRIX ViewMatrix;

	float FloatToRadians(float);
};

