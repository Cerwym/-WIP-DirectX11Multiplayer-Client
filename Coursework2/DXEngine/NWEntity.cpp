#include "NWEntity.h"

NWEntity::NWEntity()
{
	m_online = false;
	m_movingForward = false;
	m_movingBackward = false;
	m_turningLeft = false;
	m_turningRight = false;
}

void NWEntity::UpdateState(char state)
{
	if(state == E_STATE_MOVING_FORWARD)       { m_movingForward = true; }
	if(state == E_STATE_NOT_MOVING_FORWARD)   { m_movingForward = false; }
	if(state == E_STATE_MOVING_BACKWARD)      { m_movingBackward = true; }
	if(state == E_STATE_NOT_MOVING_BACKWARD)  { m_movingBackward = false; }
	if(state == E_STATE_TURNING_LEFT)         { m_turningLeft = true; }
	if(state == E_STATE_NOT_TURNING_LEFT)     { m_turningLeft = false; }
	if(state == E_STATE_TURNING_RIGHT)        { m_turningRight = true; }
	if(state == E_STATE_NOT_TURNING_RIGHT)    { m_turningRight = false; }
}

void NWEntity::Update( float deltaTime )
{
	float radians;

	if(m_movingForward)
	{
		radians = m_RotY * 0.0174532925f;
		m_PosX += sinf(radians) * 0.3f;
		m_PosZ += cosf(radians) * 0.3f;
	}

	if(m_movingBackward)
	{
		radians = m_RotY * 0.0174532925f;
		m_PosX -= sinf(radians) * 0.3f;
		m_PosZ -= cosf(radians) * 0.3f;
	}

	if(m_turningLeft)
	{
		m_RotY -= 1.5f;
		if(m_RotY < 0.0f)
		{
			m_RotY += 360.0f;
		}
	}

	if(m_turningRight)
	{
		m_RotY += 1.5f;
		if(m_RotY > 360.0f)
		{
			m_RotY -= 360.0f;
		}
	}
}

void NWEntity::UpdatePosition(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	m_PosX = posX;
	m_PosY = posY;
	m_PosZ = posZ;

	m_RotX = rotX;
	m_RotY = rotY;
	m_RotZ = rotZ;
}