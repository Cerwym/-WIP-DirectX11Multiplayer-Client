#include "D3DLight.h"

void D3DLight::SetDiffuseColour(float red, float green, float blue, float alpha)
{
	m_diffuseColour = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void D3DLight::SetAmbientColour(float r, float g, float b, float a)
{
	m_ambientColour = D3DXVECTOR4(r,g,b,a);
	return;
}

void D3DLight::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x,y,z);
	return;
}

void D3DLight::SetSpecularColour(float r, float g, float b, float a)
{
	m_specularColour = D3DXVECTOR4(r,g,b,a);
	return;
}

void D3DLight::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}