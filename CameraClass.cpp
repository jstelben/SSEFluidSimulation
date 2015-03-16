#include "CameraClass.h"


CameraClass::CameraClass(void)
{
	PositionX = 0.0f;
	PositionY = 0.0f;
	PositionZ = 0.0f;
	RotationX = 0.0f;
	RotationY = 0.0f;
	RotationZ = 0.0f;
}


CameraClass::~CameraClass(void)
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	PositionX = x;
	PositionY = y;
	PositionZ = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	RotationX = x;
	RotationY = y;
	RotationZ = z;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(PositionX, PositionY, PositionZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(RotationX, RotationY, RotationZ);
}

void CameraClass::Render()
{
	D3DXVECTOR3 up;
	D3DXVECTOR3 position;
	D3DXVECTOR3 lookAt;
	float yaw;
	float pitch;
	float roll;
	D3DXMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	position.x = PositionX;
	position.y = PositionY;
	position.z = PositionZ;

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	pitch = FloatToRadians(RotationX);
	yaw   = FloatToRadians(RotationY);
	roll  = FloatToRadians(RotationZ);

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	lookAt = position + lookAt;

	D3DXMatrixLookAtLH(&ViewMatrix, &position, &lookAt, &up);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = ViewMatrix;
}


float CameraClass::FloatToRadians(float f)
{
	return f * 0.017453292f;
}