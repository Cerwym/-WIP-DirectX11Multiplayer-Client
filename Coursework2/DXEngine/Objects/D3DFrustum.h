#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include "..\DXEngineMathHelper.h"

using namespace DirectX;

class D3DFrustum
{
public:
	D3DFrustum(){}
	D3DFrustum(const D3DFrustum&){}
	~D3DFrustum(){}

	void ConstructFrustrum(float screenDepth, const XMMATRIX &projMatrix, const XMMATRIX &vMatrix);

	bool CheckPoint(float, float, float);
	bool CheckCube(float, float, float, float);
	bool CheckSphere(float, float, float, float);
	bool CheckRectangle(float, float, float, float, float, float);

private:

	XMFLOAT4 m_Planes[6];
	BoundingFrustum m_Frustrum;
};


