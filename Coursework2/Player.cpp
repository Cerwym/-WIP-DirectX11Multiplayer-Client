#include "Player.h"

Player::Player()
{
	m_Position.x = 0.0f;
	m_Position.y = 0.0f;
	m_Position.z = 0.0f;

	m_Rotation.x = 0.0f;
	m_Rotation.y = 0.0f;
	m_Rotation.z = 0.0f;

	m_frameTime = 0.0f;

	m_lookDownSpeed = 0.0f;
	m_lookUpSpeed = 0.0f;

	m_turnLeftStateChange = false;
	m_turningLeft = false;
	m_turnRightStateChange = false;
	m_turningRight = false;
	m_moveForwardStateChange = false;
	m_movingForward = false;
	m_moveBackwardStateChange = false;
	m_movingBackward = false;
}

void Player::SetPosition(float x, float y, float z)
{
	m_Position.x = x; m_Position.y = y; m_Position.z = z;
}

void Player::SetRotation(float x, float y, float z)
{
	m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z;
}

void Player::GetPosition(float& x, float& y, float& z)
{
	x = m_Position.x; y = m_Position.y; z = m_Position.z;
}

void Player::GetRotation(float& x, float& y, float& z)
{
	x = m_Rotation.x; y = m_Rotation.y; z = m_Rotation.z;
}

void Player::SetFrameTime(float time)
{
	m_frameTime = time;
}

void Player::MoveForward(bool keydown)
{
	float radians;


	// Update the forward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Convert degrees to radians.
		radians = m_Rotation.y * 0.0174532925f;

		// Update the position.
		m_Position.x += sinf(radians) * 0.3f;
		m_Position.z += cosf(radians) * 0.3f;
	}

	// Update the state.
	if((keydown == true) && (m_movingForward == false))
	{
		m_moveForwardStateChange = true;
		m_movingForward = true;
	}

	if((keydown == false) && (m_movingForward == true))
	{
		m_moveForwardStateChange = true;
		m_movingForward = false;
	}

	return;
}

void Player::MoveBackward(bool keydown)
{
	float radians;


	// Update the backward speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Convert degrees to radians.
		radians = m_Rotation.y * 0.0174532925f;

		// Update the position.
		m_Position.x -= sinf(radians) * 0.3f;
		m_Position.z -= cosf(radians) * 0.3f;
	}

	// Update the state.
	if((keydown == true) && (m_movingBackward == false))
	{
		m_moveBackwardStateChange = true;
		m_movingBackward = true;
	}

	if((keydown == false) && (m_movingBackward == true))
	{
		m_moveBackwardStateChange = true;
		m_movingBackward = false;
	}

	return;
}

void Player::TurnLeft(bool keydown)
{
	// Update the left turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Update the rotation.
		m_Rotation.y -= 1.5f;

		// Keep the rotation in the 0 to 360 range.
		if(m_Rotation.y < 0.0f)
		{
			m_Rotation.y += 360.0f;
		}
	}

	// Update the state.
	if((keydown == true) && (m_turningLeft == false))
	{
		m_turnLeftStateChange = true;
		m_turningLeft = true;
	}

	if((keydown == false) && (m_turningLeft == true))
	{
		m_turnLeftStateChange = true;
		m_turningLeft = false;
	}

	return;
}


void Player::TurnRight(bool keydown)
{
	// Update the right turn speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		// Update the rotation.
		m_Rotation.y += 1.5f;

		// Keep the rotation in the 0 to 360 range.
		if(m_Rotation.y > 360.0f)
		{
			m_Rotation.y -= 360.0f;
		}
	}

	// Update the state.
	if((keydown == true) && (m_turningRight == false))
	{
		m_turnRightStateChange = true;
		m_turningRight = true;
	}

	if((keydown == false) && (m_turningRight == true))
	{
		m_turnRightStateChange = true;
		m_turningRight = false;
	}

	return;
}

void Player::LookUpward(bool keydown)
{
	// Update the upward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookUpSpeed += m_frameTime * 0.01f;

		if(m_lookUpSpeed > (m_frameTime * 0.15f))
		{
			m_lookUpSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookUpSpeed -= m_frameTime* 0.005f;

		if(m_lookUpSpeed < 0.0f)
		{
			m_lookUpSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_Rotation.x -= m_lookUpSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_Rotation.x > 90.0f)
	{
		m_Rotation.x = 90.0f;
	}

	return;
}


void Player::LookDownward(bool keydown)
{
	// Update the downward rotation speed movement based on the frame time and whether the user is holding the key down or not.
	if(keydown)
	{
		m_lookDownSpeed += m_frameTime * 0.01f;

		if(m_lookDownSpeed > (m_frameTime * 0.15f))
		{
			m_lookDownSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_lookDownSpeed -= m_frameTime* 0.005f;

		if(m_lookDownSpeed < 0.0f)
		{
			m_lookDownSpeed = 0.0f;
		}
	}

	// Update the rotation.
	m_Rotation.x += m_lookDownSpeed;

	// Keep the rotation maximum 90 degrees.
	if(m_Rotation.x < -90.0f)
	{
		m_Rotation.x = -90.0f;
	}

	return;
}


bool Player::TurnLeftStateChange(char& newState)
{
	bool result;


	// Check if there has been a state change to turning left.
	if(m_turnLeftStateChange)
	{
		result = true;
		m_turnLeftStateChange = false;

		// If there has been check whether it was to start or to stop turning left.
		if(m_turningLeft)
		{
			newState = STATE_TURNING_LEFT;
		}
		else
		{
			newState = STATE_NOT_TURNING_LEFT;
		}
	}
	else
	{
		result = false;
	}

	return result;
}


bool Player::TurnRightStateChange(char& newState)
{
	bool result;


	// Check if there has been a state change to turning right.
	if(m_turnRightStateChange)
	{
		result = true;
		m_turnRightStateChange = false;

		// If there has been check whether it was to start or to stop turning right.
		if(m_turningRight)
		{
			newState = STATE_TURNING_RIGHT;
		}
		else
		{
			newState = STATE_NOT_TURNING_RIGHT;
		}
	}
	else
	{
		result = false;
	}

	return result;
}


bool Player::MoveForwardStateChange(char& newState)
{
	bool result;


	// Check if there has been a state change to moving forward.
	if(m_moveForwardStateChange)
	{
		result = true;
		m_moveForwardStateChange = false;

		// If there has been check whether it was to start or to stop moving forward.
		if(m_movingForward)
		{
			newState = STATE_MOVING_FORWARD;
		}
		else
		{
			newState = STATE_NOT_MOVING_FORWARD;
		}
	}
	else
	{
		result = false;
	}

	return result;
}


bool Player::MoveBackwardStateChange(char& newState)
{
	bool result;


	// Check if there has been a state change to moving backward.
	if(m_moveBackwardStateChange)
	{
		result = true;
		m_moveBackwardStateChange = false;

		// If there has been check whether it was to start or to stop moving backward.
		if(m_movingBackward)
		{
			newState = STATE_MOVING_BACKWARD;
		}
		else
		{
			newState = STATE_NOT_MOVING_BACKWARD;
		}
	}
	else
	{
		result = false;
	}

	return result;
}