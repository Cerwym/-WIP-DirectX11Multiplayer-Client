#pragma once

#include <D3DX10math.h>

class D3DFrustum
{
public:
	D3DFrustum(){}
	D3DFrustum(const D3DFrustum&){}
	~D3DFrustum(){}

	void ConstructFrustrum(float, D3DXMATRIX, D3DXMATRIX);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:

	D3DXPLANE m_Planes[6];
};


