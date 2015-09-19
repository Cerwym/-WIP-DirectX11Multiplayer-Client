#include "D3DFrustum.h"

void D3DFrustum::ConstructFrustrum(float screenDepth, const XMMATRIX &projMatrix, const XMMATRIX &vMatrix)
{
	float zMin, r;
	XMFLOAT4X4 tmpProj;
	XMFLOAT4X4 tmpView;

	XMStoreFloat4x4(&tmpProj, projMatrix);
	XMStoreFloat4x4(&tmpView, vMatrix);

	// Calculate the minimum Z distance in the frustum

	zMin = -tmpProj._43 / -tmpProj._33;
	
	//zMin = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth - zMin);

	// Calculate the frustum mat from the view mat and updated projection mat.
	//XMMATRIX mat = XMMATRIXMultiply(vMatrix, projMatrix);

	BoundingFrustum tmp(XMLoadFloat4x4(&tmpProj));
	XMVECTOR det;
	tmp.Transform(m_Frustrum, XMMatrixInverse(&det, XMLoadFloat4x4(&tmpView)));

	return;
}



bool D3DFrustum::CheckPoint(float x, float y, float z)
{
	/*
	// Check to see if the given point is inside all six planes of the view frustum
	for (int i = 0; i < 6; i++)
	{

		if (XMPlaneDotCoord(&m_Planes[i], &XMFLOAT3(x, y, z)) < 0.0f)
			return false;
	}
	*/
	return true; // The point is inside the frustum
}

bool D3DFrustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	/*
	// Check if the radius of the sphere is inside the view frustum.
	for(int i = 0; i < 6; i++) 
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}
	*/
	return true;
}

bool D3DFrustum::CheckCube(float xCentre, float yCentre, float zCentre, float radius)
{
	/*
	// Check to see if any point of the cube is in the frustum, if it is, continue execution, if no point is valid, return false
	for (int i = 0; i < 6; i++)
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - radius), (yCentre - radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + radius), (yCentre - radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - radius), (yCentre + radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + radius), (yCentre + radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - radius), (yCentre - radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + radius), (yCentre - radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - radius), (yCentre + radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + radius), (yCentre + radius), (zCentre + radius))) >= 0.0f)
			continue;

		return false;
	}

	*/

	return true;
}

bool D3DFrustum::CheckRectangle(float xCentre, float yCentre, float zCentre, float xSize, float ySize, float zSize)
{
	/*
	// Check to see if any point of the cube is in the frustum, if it is, continue execution, if no point is valid, return false
	for(int i = 0; i < 6; i++)
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - xSize), (yCentre - ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + xSize), (yCentre - ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - xSize), (yCentre + ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - xSize), (yCentre - ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + xSize), (yCentre + ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + xSize), (yCentre - ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre - xSize), (yCentre + ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &XMFLOAT3((xCentre + xSize), (yCentre + ySize), (zCentre + zSize))) >= 0.0f)
			continue;

		return false;
	}

	*/
	return true;
}