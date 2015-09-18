#pragma once

#include <DirectXMath.h>
#include "DXEngine/Defines.h"
#include "DXEngine/D3DInput.h"
#include "DXEngine/Objects/D3DCamera.h"

const char STATE_TURNING_LEFT = 0;
const char STATE_NOT_TURNING_LEFT = 1;
const char STATE_TURNING_RIGHT = 2;
const char STATE_NOT_TURNING_RIGHT = 3;
const char STATE_MOVING_FORWARD = 4;
const char STATE_NOT_MOVING_FORWARD = 5;
const char STATE_MOVING_BACKWARD = 6;
const char STATE_NOT_MOVING_BACKWARD = 7;

const float PLAYER_MAX_SPEED = 0.275f;
const float PLAYER_ACCELERATION_RATE = 0.2f;

XMFLOAT3 HELPER_RotVectorFrom4x4Mat(D3DXMATRIX mat);

class Player
{
public:

	Player();
	Player(const Player&){}
	~Player();

	void Init(float FOV, float aspect, float zNear, float zFar);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void MoveX(float d);
	void MoveZ(float d);

	XMFLOAT3 GetPosition(){return m_Position;}
	XMFLOAT3 GetRotation(){return m_Rotation;}
	XMFLOAT3 GetVelocity(){return m_velocity;}
	XMFLOAT3 GetAcceleration(){return m_acceleration;}
	void GetPosition(float &x, float& y, float& z);
	void GetRotation(float& x, float& y, float& z);

	D3DCamera* Camera(){return m_Camera;}

	void SetFrameTime(float time);

	void Update(D3DInput* input, float deltaTime);

	void MoveForward(bool keydown);
	void MoveBackward(bool keydown);
	void TurnLeft(bool keydown);
	void TurnRight(bool keydown);
	void LookUpward(bool keydown);
	void LookDownward(bool keydown);

	bool TurnLeftStateChange(char& newState);
	bool TurnRightStateChange(char& newState);
	bool MoveForwardStateChange(char& newState);
	bool MoveBackwardStateChange(char& newState);

private:

	void RebuildTransform();

	D3DCamera* m_Camera;
	XMMATRIX m_ViewMatrix, m_ProjectionMatrix, m_WorldMatrix;
	XMFLOAT3 m_Position, m_LookAt, m_Right, m_Up, m_velocity, m_oldVelocity, m_acceleration;
	XMFLOAT3 m_Rotation;

	float m_frameTime;
	float m_lookUpSpeed, m_lookDownSpeed;

	bool m_turnLeftStateChange, m_turningLeft;
	bool m_turnRightStateChange, m_turningRight;
	bool m_moveForwardStateChange, m_movingForward;
	bool m_moveBackwardStateChange, m_movingBackward;
};

