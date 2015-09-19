#include "Player.h"

#include <iostream>

Player::Player()
{
	m_Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_LookAt = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_velocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_ViewMatrix = XMMatrixIdentity();
	m_ProjectionMatrix = XMMatrixIdentity();
	m_WorldMatrix = XMMatrixIdentity();

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

	m_Camera = 0;
}

Player::~Player()
{
	S_DELETE(m_Camera);
}

void Player::Init(float FOV, float aspect, float zNear, float zFar)
{
	m_Camera = new D3DCamera(FOV, aspect, zNear, zFar);
}

void Player::SetPosition(float x, float y, float z)
{
	m_Position.x = x; m_Position.y = y; m_Position.z = z;
	RebuildTransform();
}

void Player::GetPosition(float& x, float& y, float& z)
{
	x = m_Position.x;
	y = m_Position.y;
	z = m_Position.z;
}

void Player::MoveX(float dt)
{
	/*
	m_Position += (m_velocity.x)*m_Right;
	XMMATRIXIdentity( &m_WorldMatrix );

	XMMATRIX temp;
	XMMATRIXIdentity( &temp );

	XMMATRIXTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjectionMatrix;
	*/
	m_Camera->Move_X(m_velocity.x);
	m_Position = m_Camera->GetPosition();
}


void Player::MoveZ(float dt)
{
	/*
	m_Position += (m_velocity.z * dt)*m_LookAt;
	XMMATRIXIdentity( &m_WorldMatrix );

	XMMATRIX temp;
	XMMATRIXIdentity( &temp );

	XMMATRIXTranslation( &temp, m_Position.x, m_Position.y, m_Position.z );

	// Scale the temporary matrix
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjectionMatrix;
	*/
	m_Camera->Move_Z(m_velocity.z);
	m_Position = m_Camera->GetPosition();
}

void Player::SetRotation(float x, float y, float z)
{
	//m_Rotation.x = x; m_Rotation.y = y; m_Rotation.z = z;
}

void Player::RebuildTransform()
{
	m_WorldMatrix = XMMatrixIdentity();
	XMMATRIX temp = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	
	// Scale the world matrix;
	m_WorldMatrix *= temp * m_ViewMatrix * m_ProjectionMatrix;
	m_Camera->SetPosition(m_Position);
}

void Player::SetFrameTime(float time)
{
	m_frameTime = time;
}

void Player::Update(D3DInput* input, float deltaTime)
{
	m_oldVelocity = m_velocity; 
	// Forward & backward
	if (input->isKeyPressed(DIK_W, true))
	{
		m_velocity.z += (PLAYER_ACCELERATION_RATE * deltaTime);
		if (m_velocity.z  > PLAYER_MAX_SPEED)
			m_velocity.z = PLAYER_MAX_SPEED;
	}

	else if (input->isKeyPressed(DIK_S, true))
	{
		m_velocity.z -= (PLAYER_ACCELERATION_RATE * deltaTime);
		if (m_velocity.z < -PLAYER_MAX_SPEED)
			m_velocity.z = -PLAYER_MAX_SPEED;
	}

	// If neither the 'forward' or 'backward' key has been pressed, then slow down acceleration
	else
	{
		std::cout << "No forward or backward movement" << std::endl;
		if (m_velocity.z < 0)
		{
			m_velocity.z += (PLAYER_ACCELERATION_RATE * deltaTime) * 2;
			if (m_velocity.z >= 0)
				m_velocity.z = 0.0f;
		}
		else
		{
			m_velocity.z -= (PLAYER_ACCELERATION_RATE * deltaTime) * 2;
			if (m_velocity.z <= 0)
				m_velocity.z = 0.0f;
		}
	}

	// Strafe left & right
	if (input->isKeyPressed(DIK_A, true))
	{
		m_velocity.x -= (PLAYER_ACCELERATION_RATE * deltaTime);
		if (m_velocity.x  <= -PLAYER_MAX_SPEED)
			m_velocity.x = -PLAYER_MAX_SPEED;
	}

	else if (input->isKeyPressed(DIK_D, true))
	{
		m_velocity.x += (PLAYER_ACCELERATION_RATE * deltaTime);
		if (m_velocity.x > PLAYER_MAX_SPEED)
			m_velocity.x = PLAYER_MAX_SPEED;
	}

	// If neither the 'left' or 'right' key has been pressed, then slow down acceleration
	else
	{
		if (m_velocity.x < 0)
		{
			m_velocity.x += (PLAYER_ACCELERATION_RATE * deltaTime) * 2;
			if (m_velocity.x >= 0)
				m_velocity.x = 0.0f;
		}
		else
		{
			m_velocity.x -= (PLAYER_ACCELERATION_RATE * deltaTime) * 2;
			if (m_velocity.x <= 0)
				m_velocity.x = 0.0f;
		}
	}

	MoveX(deltaTime);
	MoveZ(deltaTime);

	std::cout << "Velocity (" << m_velocity.x << "," << m_velocity.y << "," << m_velocity.z	 << ")" << std::endl;

	// Get Rotation from player's camera
	XMMATRIX world = m_Camera->GetWorld();
	//m_Rotation = HELPER_RotVectorFrom4x4Mat(world);

	// TODO : this properly.
	m_acceleration.x = m_velocity.x - m_oldVelocity.x;
	m_acceleration.y = m_velocity.y - m_oldVelocity.y;
	m_acceleration.z = m_velocity.z - m_oldVelocity.z;

}

/*
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
*/


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

/*
XMFLOAT3 HELPER_RotVectorFrom4x4Mat(const XMMATRIX &mat)
{
	//theta1 = atan2(m01, m11)

	double theta1 = atan2(mat._23, mat._33);
	double c2 = sqrt((mat._11 * mat._11) + (mat._12 * mat._12));
	double theta2 = atan2((double)-mat._13, c2);
	double s1 = sin(theta1); double c1 = cos(theta1);
	double theta3 = atan2( (s1 * mat._31) - (c1 * mat._31), (c1 * mat._22) - (s1 * mat._31) );

	return XMFLOAT3(theta1, theta2, theta3);
}

*/
