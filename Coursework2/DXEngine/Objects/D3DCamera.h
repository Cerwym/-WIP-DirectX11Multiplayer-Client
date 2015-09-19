#pragma once

#include <DirectXMath.h>
#include "..\Defines.h"
#include "D3DFrustum.h"

class D3DCamera
{
public:
	D3DCamera(float FoVy, float aspect, float zNear, float zFar);
	D3DCamera(const D3DCamera&){}
	~D3DCamera();

	void SetPosition(XMFLOAT3& pos);
	D3DFrustum* GetFrustum(){ return m_Frustum; }
	XMFLOAT3 GetPosition()
	{
		retPos.x = XMVectorGetX(m_Position);
		retPos.y = XMVectorGetY(m_Position);
		retPos.z = XMVectorGetZ(m_Position);
		return retPos;
	}
	XMMATRIX GetViewMatrix()const{return m_ViewMatrix;}
	XMMATRIX GetProjMatrix()const{return m_ProjMatrix;}
	XMMATRIX GetWorld()const{return m_WorldMatrix;}

	void SetFOV(float FoVy, float aspect, float z_near, float z_far);
	void Move_X(float dt);
	void Move_Z(float d);
	void Pitch(float angle);
	void Roll(float angle);

	XMMATRIX World(){return m_WorldMatrix;}

	void RebuildView();

private:

	D3DFrustum* m_Frustum;
	XMVECTOR m_Position;
	XMVECTOR m_LookAt;
	XMVECTOR m_Right;
	XMVECTOR m_Up;

	XMFLOAT3 retPos;

	XMMATRIX m_ViewMatrix;
	XMMATRIX m_ProjMatrix;
	XMMATRIX m_WorldMatrix;

	float m_zNear, m_zFar;
};
