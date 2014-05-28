#pragma once

#include <D3DX10math.h>
#include "..\Defines.h"
#include "D3DFrustum.h"

class D3DCamera
{
public:
	D3DCamera(float FoVy, float aspect, float zNear, float zFar);
	D3DCamera(const D3DCamera&){}
	~D3DCamera();

	void SetPosition(D3DXVECTOR3& pos);
	D3DFrustum* GetFrustum(){ return m_Frustum; }
	D3DXVECTOR3& GetPosition(){return m_Position;}
	D3DXMATRIX GetViewMatrix()const{return m_ViewMatrix;}
	D3DXMATRIX GetProjMatrix()const{return m_ProjMatrix;}
	D3DXMATRIX GetWorld()const{return m_WorldMatrix;}

	void SetFOV(float FoVy, float aspect, float z_near, float z_far);
	void Move_X(float dt);
	void Move_Z(float d);
	void Pitch(float angle);
	void Roll(float angle);

	float GetX(){ return m_Position.x;}
	float GetY(){ return m_Position.y;}
	float GetZ(){ return m_Position.z;}

	D3DXMATRIX World(){return m_WorldMatrix;}

	void RebuildView();

private:

	D3DFrustum* m_Frustum;
	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_LookAt;
	D3DXVECTOR3 m_Right;
	D3DXVECTOR3 m_Up;
	D3DXMATRIX m_ViewMatrix;
	D3DXMATRIX m_ProjMatrix;
	D3DXMATRIX m_WorldMatrix;

	float m_zNear, m_zFar;
};
