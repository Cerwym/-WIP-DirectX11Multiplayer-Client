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
		D3DXVECTOR3 position;
		D3DXVECTOR3 velocity;
		D3DXVECTOR3 acceleration;
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
	void SetPosition(D3DXVECTOR3 &pos){ m_Position = pos; }
	void SetRotation(D3DXVECTOR3 &rot){ m_Rotation = rot; }
	void SetAcceleration(D3DXVECTOR3 &acc){m_Velocity = acc;}
	bool IsOnline(){return m_online;}
	unsigned short GetID(){return m_EntityID;}
	char GetEntityType(){return m_EntityType;}
	void GetPosition(D3DXVECTOR3& position){ position = m_Position;}
	void GetRotation(D3DXVECTOR3& rotation){ rotation = m_Rotation;}
	void UpdateState(char state);
	void Update( float deltaTime );
	void UpdatePosition(D3DXVECTOR3 position, D3DXVECTOR3 rotation, D3DXVECTOR3 velocity, D3DXVECTOR3 acceleration, unsigned long timestamp);
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

	D3DXVECTOR3 m_Position;
	D3DXVECTOR3 m_Rotation;
	D3DXVECTOR3 m_Velocity;
	D3DXVECTOR3 m_Acceleration;

	bool m_movingForward, m_movingBackward, m_turningLeft, m_turningRight;
};
