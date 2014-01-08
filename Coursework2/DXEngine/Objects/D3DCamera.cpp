#include "D3DCamera.h"

D3DCamera::D3DCamera(float FoVy, float aspect, float zNear, float zFar)
{
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_Right = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	m_Up = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_LookAt = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	D3DXMatrixIdentity(&m_ViewMatrix);
	D3DXMatrixIdentity(&m_ProjMatrix);
	D3DXMatrixIdentity(&m_WorldMatrix);

	m_zNear = zNear;
	m_zFar = zFar;

	SetFOV(FoVy, aspect, m_zNear, m_zFar );
	m_Frustum = new D3DFrustum;
}

D3DCamera::~D3DCamera()
{
	S_DELETE(m_Frustum);
}

void D3DCamera::SetFOV(float FoVy, float aspect, float z_near, float z_far)
{
	D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, FoVy, aspect, z_near, z_far);
}

void D3DCamera::Move_X(float d)
{
	m_Position += d*m_Right;

	// Set the current world matrix to the identity matrix
	D3DXMatrixIdentity( &m_WorldMatrix );

	D3DXMATRIX temp;
	D3DXMatrixIdentity( &temp );

	D3DXMatrixTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjMatrix;

}

void D3DCamera::SetPosition(D3DXVECTOR3& pos)
{
	m_Position = pos;
}

void D3DCamera::Move_Z(float d)
{
	m_Position += d*m_LookAt;

	// Set the current world matrix to the identity matrix
	D3DXMatrixIdentity( &m_WorldMatrix );

	D3DXMATRIX temp;
	D3DXMatrixIdentity( &temp );

	D3DXMatrixTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjMatrix;
}

void D3DCamera::Pitch(float angle)
{
	D3DXMATRIX rot;
	D3DXMatrixRotationAxis(&rot, &m_Right, angle);

	D3DXVec3TransformNormal(&m_Up, &m_Up, &rot);
	D3DXVec3TransformNormal(&m_LookAt, &m_LookAt, &rot);
}

void D3DCamera::Roll(float angle)
{
	D3DXMATRIX rot;
	D3DXMatrixRotationY(&rot, angle);

	D3DXVec3TransformNormal(&m_Right, &m_Right, &rot);
	D3DXVec3TransformNormal(&m_Up, &m_Up, &rot);
	D3DXVec3TransformNormal(&m_LookAt, &m_LookAt, &rot);
}

void D3DCamera::RebuildView()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	D3DXVec3Normalize(&m_LookAt, &m_LookAt);

	D3DXVec3Cross(&m_Up, &m_LookAt, &m_Right);
	D3DXVec3Normalize(&m_Up, &m_Up);

	D3DXVec3Cross(&m_Right, &m_Up, &m_LookAt);
	D3DXVec3Normalize(&m_Right, &m_Right);

	// Fill in the view matrix entries.
	float x = -D3DXVec3Dot(&m_Position, &m_Right);
	float y = -D3DXVec3Dot(&m_Position, &m_Up);
	float z = -D3DXVec3Dot(&m_Position, &m_LookAt);

	m_ViewMatrix(0,0) = m_Right.x;
	m_ViewMatrix(1,0) = m_Right.y;
	m_ViewMatrix(2,0) = m_Right.z;
	m_ViewMatrix(3,0) = x;

	m_ViewMatrix(0,1) = m_Up.x;
	m_ViewMatrix(1,1) = m_Up.y;
	m_ViewMatrix(2,1) = m_Up.z;
	m_ViewMatrix(3,1) = y;

	m_ViewMatrix(0,2) = m_LookAt.x;
	m_ViewMatrix(1,2) = m_LookAt.y;
	m_ViewMatrix(2,2) = m_LookAt.z;
	m_ViewMatrix(3,2) = z;

	m_ViewMatrix(0,3) = 0.0f;
	m_ViewMatrix(1,3) = 0.0f;
	m_ViewMatrix(2,3) = 0.0f;
	m_ViewMatrix(3,3) = 1.0f;

	// Reconstuct the viewing frustum.
	m_Frustum->ConstructFrustrum( m_zFar, m_ProjMatrix, m_ViewMatrix );
}