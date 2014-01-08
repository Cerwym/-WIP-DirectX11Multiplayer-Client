#ifndef D3DLIGHT_H
#define D3DLIGHT_H

// The purpose of this class is to set and maintain the direction and colour of lights

#include <D3DX10math.h>

class D3DLight
{
public:
	D3DLight(){}
	D3DLight(const D3DLight&){}
	~D3DLight(){}

	// RGBA
	void SetDiffuseColour(float, float, float, float);
	void SetAmbientColour(float, float, float, float);
	void SetSpecularColour(float, float, float, float);
	// XYZ
	void SetDirection(float, float, float);

	void SetSpecularPower(float);

	D3DXVECTOR4 GetDiffuseColour(){return m_diffuseColour;}
	D3DXVECTOR4 GetAmbientColour(){return m_ambientColour;}
	D3DXVECTOR3 GetDirection(){return m_direction;}
	D3DXVECTOR4 GetSpecularColour(){return m_specularColour;}
	float GetSpecularPower(){return m_specularPower;}

private:
	D3DXVECTOR4 m_ambientColour;
	D3DXVECTOR4 m_diffuseColour;
	D3DXVECTOR3 m_direction;
	D3DXVECTOR4 m_specularColour;
	float m_specularPower;
};

#endif