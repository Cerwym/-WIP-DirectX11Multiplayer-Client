#include "D3DFrustum.h"

void D3DFrustum::ConstructFrustrum(float screenDepth, D3DXMATRIX projMatrix, D3DXMATRIX vMatrix)
{
	float zMin, r;
	D3DXMATRIX mat;

	// Calculate the minimum Z distance in the frustum
	zMin = -projMatrix._43 / projMatrix._33;
	r = screenDepth / (screenDepth - zMin);

	// Calculate the frustum mat from the view mat and updated projection mat.
	D3DXMatrixMultiply(&mat, &vMatrix, &projMatrix);

	// Calculate near plane of frustum.
	m_Planes[0].a = mat._14 + mat._13;
	m_Planes[0].b = mat._24 + mat._23;
	m_Planes[0].c = mat._34 + mat._33;
	m_Planes[0].d = mat._44 + mat._43;
	D3DXPlaneNormalize(&m_Planes[0], &m_Planes[0]);

	// Calculate far plane of frustum.
	m_Planes[1].a = mat._14 - mat._13; 
	m_Planes[1].b = mat._24 - mat._23;
	m_Planes[1].c = mat._34 - mat._33;
	m_Planes[1].d = mat._44 - mat._43;
	D3DXPlaneNormalize(&m_Planes[1], &m_Planes[1]);

	// Calculate left plane of frustum.
	m_Planes[2].a = mat._14 + mat._11; 
	m_Planes[2].b = mat._24 + mat._21;
	m_Planes[2].c = mat._34 + mat._31;
	m_Planes[2].d = mat._44 + mat._41;
	D3DXPlaneNormalize(&m_Planes[2], &m_Planes[2]);

	// Calculate right plane of frustum.
	m_Planes[3].a = mat._14 - mat._11; 
	m_Planes[3].b = mat._24 - mat._21;
	m_Planes[3].c = mat._34 - mat._31;
	m_Planes[3].d = mat._44 - mat._41;
	D3DXPlaneNormalize(&m_Planes[3], &m_Planes[3]);

	// Calculate top plane of frustum.
	m_Planes[4].a = mat._14 - mat._12; 
	m_Planes[4].b = mat._24 - mat._22;
	m_Planes[4].c = mat._34 - mat._32;
	m_Planes[4].d = mat._44 - mat._42;
	D3DXPlaneNormalize(&m_Planes[4], &m_Planes[4]);

	// Calculate bottom plane of frustum.
	m_Planes[5].a = mat._14 + mat._12;
	m_Planes[5].b = mat._24 + mat._22;
	m_Planes[5].c = mat._34 + mat._32;
	m_Planes[5].d = mat._44 + mat._42;
	D3DXPlaneNormalize(&m_Planes[5], &m_Planes[5]);

	return;
}

bool D3DFrustum::CheckPoint(float x, float y, float z)
{
	// Check to see if the given point is inside all six planes of the view frustum
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(x,y,z)) < 0.0f)
			return false;
	}

	return true; // The point is inside the frustum
}

bool D3DFrustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for(int i = 0; i < 6; i++) 
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3(xCenter, yCenter, zCenter)) < -radius)
		{
			return false;
		}
	}

	return true;
}

bool D3DFrustum::CheckCube(float xCentre, float yCentre, float zCentre, float radius)
{
	// Check to see if any point of the cube is in the frustum, if it is, continue execution, if no point is valid, return false
	for (int i = 0; i < 6; i++)
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - radius), (yCentre - radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + radius), (yCentre - radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - radius), (yCentre + radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + radius), (yCentre + radius), (zCentre - radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - radius), (yCentre - radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + radius), (yCentre - radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - radius), (yCentre + radius), (zCentre + radius))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + radius), (yCentre + radius), (zCentre + radius))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

bool D3DFrustum::CheckRectangle(float xCentre, float yCentre, float zCentre, float xSize, float ySize, float zSize)
{
	// Check to see if any point of the cube is in the frustum, if it is, continue execution, if no point is valid, return false
	for(int i = 0; i < 6; i++)
	{
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - xSize), (yCentre - ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + xSize), (yCentre - ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - xSize), (yCentre + ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - xSize), (yCentre - ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + xSize), (yCentre + ySize), (zCentre - zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + xSize), (yCentre - ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre - xSize), (yCentre + ySize), (zCentre + zSize))) >= 0.0f)
			continue;
		if(D3DXPlaneDotCoord(&m_Planes[i], &D3DXVECTOR3((xCentre + xSize), (yCentre + ySize), (zCentre + zSize))) >= 0.0f)
			continue;

		return false;
	}

	return true;
}