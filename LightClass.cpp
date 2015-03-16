#include "LightClass.h"


LightClass::LightClass(void)
{
}


LightClass::~LightClass(void)
{
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	DiffuseColor = D3DXVECTOR4(red, green, blue, alpha);
}

void LightClass::SetDirection(float x, float y, float z)
{
	Direction = D3DXVECTOR3(x, y, z);
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return DiffuseColor;
}

D3DXVECTOR3 LightClass::GetDirection()
{
	return Direction;
}