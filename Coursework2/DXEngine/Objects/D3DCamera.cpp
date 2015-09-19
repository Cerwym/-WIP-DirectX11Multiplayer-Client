#include "D3DCamera.h"

D3DCamera::D3DCamera(float FoVy, float aspect, float zNear, float zFar)
{
	m_Position = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	m_Right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	m_Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	m_LookAt = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjMatrix = XMMatrixIdentity();
	m_WorldMatrix = XMMatrixIdentity();

	m_zNear = zNear;
	m_zFar = zFar;

	SetFOV(FoVy, aspect, m_zNear, m_zFar );
	m_Frustum = new D3DFrustum;
}

D3DCamera::~D3DCamera()
{
	//S_DELETE(m_Frustum);
}

void D3DCamera::SetFOV(float FoVy, float aspect, float z_near, float z_far)
{
	m_ProjMatrix = XMMatrixPerspectiveFovLH(FoVy, aspect, z_near, z_far);
}

void D3DCamera::Move_X(float d)
{
	//m_Position += d*m_Right;

	// Set the current world matrix to the identity matrix
	m_WorldMatrix = XMMatrixIdentity();

	XMMATRIX temp = XMMatrixTranslation(XMVectorGetX(m_Position), XMVectorGetY(m_Position), XMVectorGetZ(m_Position));

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjMatrix;
}

void D3DCamera::SetPosition(XMFLOAT3& pos)
{
	m_Position = XMVectorSet(pos.x, pos.y, pos.z, 0);
}

void D3DCamera::Move_Z(float d)
{
	//m_Position += d*m_LookAt;

	// Set the current world matrix to the identity matrix
	m_WorldMatrix = XMMatrixIdentity();

	XMMATRIX temp = XMMatrixTranslation(XMVectorGetX(m_Position), XMVectorGetY(m_Position), XMVectorGetZ(m_Position));

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjMatrix;
}

void D3DCamera::Pitch(float angle)
{
	XMMATRIX rot = XMMatrixRotationAxis(m_Right, angle);

	m_Up = XMVector3TransformNormal(m_Up, rot);
	m_LookAt = XMVector3TransformNormal(m_LookAt, rot);
}

void D3DCamera::Roll(float angle)
{
	XMMATRIX rot = XMMatrixRotationY(angle);

	m_Right = XMVector3TransformNormal(m_Right, rot);
	m_Up = XMVector3TransformNormal(m_Up, rot);
	m_LookAt = XMVector3TransformNormal(m_LookAt, rot);
}

void D3DCamera::RebuildView()
{
	// Keep camera's axes orthogonal to each other and of unit length.
	m_LookAt = XMVector3Normalize(m_LookAt);

	m_Up = XMVector3Cross(m_LookAt, m_Right);
	m_Up = XMVector3Normalize(m_Up);

	m_Right = XMVector3Cross(m_Up, m_LookAt);
	m_Right = XMVector3Normalize(m_Right);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(m_Position, m_Right));
	float y = -XMVectorGetX(XMVector3Dot(m_Position, m_Up));
	float z = -XMVectorGetX(XMVector3Dot(m_Position, m_LookAt));

	XMFLOAT4X4 tempView;

	tempView(0, 0) = XMVectorGetX(m_Right);
	tempView(1, 0) = XMVectorGetY(m_Right);
	tempView(2, 0) = XMVectorGetZ(m_Right);
	tempView(3, 0) = x;

	tempView(0, 1) = XMVectorGetX(m_Up);
	tempView(1, 1) = XMVectorGetY(m_Up);
	tempView(2, 1) = XMVectorGetZ(m_Up);
	tempView(3, 1) = y;

	tempView(0, 2) = XMVectorGetX(m_LookAt);
	tempView(1, 2) = XMVectorGetX(m_LookAt);
	tempView(2, 2) = XMVectorGetX(m_LookAt);
	tempView(3, 2) = z;

	tempView(0, 3) = 0.0f;
	tempView(1, 3) = 0.0f;
	tempView(2, 3) = 0.0f;
	tempView(3, 3) = 1.0f;

	m_ViewMatrix = XMLoadFloat4x4(&tempView);
	
	// Reconstuct the viewing frustum.
	m_Frustum->ConstructFrustrum( m_zFar, m_ProjMatrix, m_ViewMatrix );
}