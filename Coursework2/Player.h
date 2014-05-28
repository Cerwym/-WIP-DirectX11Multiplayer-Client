#pragma once

#include <math.h>
#include "DXEngine/Defines.h"

const char STATE_TURNING_LEFT = 0;
const char STATE_NOT_TURNING_LEFT = 1;
const char STATE_TURNING_RIGHT = 2;
const char STATE_NOT_TURNING_RIGHT = 3;
const char STATE_MOVING_FORWARD = 4;
const char STATE_NOT_MOVING_FORWARD = 5;
const char STATE_MOVING_BACKWARD = 6;
const char STATE_NOT_MOVING_BACKWARD = 7;

class Player
{
public:

	Player();
	Player(const Player&){}
	~Player(){}

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void GetPosition(float& x, float& y, float& z);
	void GetRotation(float& x, float& y, float& z);

	void SetFrameTime(float time);

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

	Vector2 m_Position;
	Vector2 m_Rotation;

	float m_frameTime;
	float m_lookUpSpeed, m_lookDownSpeed;

	bool m_turnLeftStateChange, m_turningLeft;
	bool m_turnRightStateChange, m_turningRight;
	bool m_moveForwardStateChange, m_movingForward;
	bool m_moveBackwardStateChange, m_movingBackward;
};

