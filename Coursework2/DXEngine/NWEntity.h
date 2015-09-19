#pragma once

#include <math.h>

#include "Defines.h"
#include "D3DSys.h"
#include "D3DShaderManager.h"
#include "Objects/D3DCamera.h"
#include "Objects/D3DModel.h"

const int MAX_ENTITIES = 100;
const char ENTITY_TYPE_USER = 0;
const char ENTITY_TYPE_AI = 1;
const char E_STATE_TURNING_LEFT = 0;
const char E_STATE_NOT_TURNING_LEFT = 1;
const char E_STATE_TURNING_RIGHT = 2;
const char E_STATE_NOT_TURNING_RIGHT = 3;
const char E_STATE_MOVING_FORWARD = 4;
const char E_STATE_NOT_MOVING_FORWARD = 5;
const char E_STATE_MOVING_BACKWARD = 6;
const char E_STATE_NOT_MOVING_BACKWARD = 7;

class NWEntity
{

private:
	struct kinematicState  
	{
		XMFLOAT3 position;
		XMFLOAT3 velocity;
		XMFLOAT3 acceleration;
	};
public:
	NWEntity();
	NWEntity(const NWEntity&){}
	~NWEntity();

	bool Init(D3DSys* sys, char* modelName, WCHAR* textureName);
	void Render(D3DCamera* camera, D3DShaderManager* sm);

	void SetOnlineStatus(bool flag){m_online = flag;}
	void SetID(unsigned short ID){m_EntityID = ID;}
	void SetType(char type){m_EntityType = type;}
	void SetPosition(XMFLOAT3 &pos){ m_Position = pos; }
	void SetRotation(XMFLOAT3 &rot){ m_Rotation = rot; }
	void SetAcceleration(XMFLOAT3 &acc){m_Velocity = acc;}
	bool IsOnline(){return m_online;}
	unsigned short GetID(){return m_EntityID;}
	char GetEntityType(){return m_EntityType;}
	void GetPosition(XMFLOAT3& position){ position = m_Position;}
	void GetRotation(XMFLOAT3& rotation){ rotation = m_Rotation;}
	void UpdateState(char state);
	void Update( float deltaTime );
	void UpdatePosition(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 velocity, XMFLOAT3 acceleration, unsigned long timestamp);
	void UpdateRotateState(bool rotate){m_turningRight = rotate;}
	void HasRecievedMessage(bool flag){m_messageRecieved = flag;}

	void PredictPositionQuadratic(kinematicState *old, kinematicState *predition, float elapsedSeconds);
	void BlendStateLinear(kinematicState *out, kinematicState * oldState, kinematicState *newState, float percentageToNew);


private:
	D3DModel* m_EntityModel;
	D3DSys* m_D3DSys;
	bool m_online;
	bool m_messageRecieved;
	unsigned long m_packetDifference;
	unsigned short m_EntityID;
	char m_EntityType;

	kinematicState oldState, newState;

	XMFLOAT3 m_Position;
	XMFLOAT3 m_Rotation;
	XMFLOAT3 m_Velocity;
	XMFLOAT3 m_Acceleration;

	bool m_movingForward, m_movingBackward, m_turningLeft, m_turningRight;
};
