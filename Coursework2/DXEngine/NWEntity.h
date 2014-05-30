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
public:
	NWEntity();
	NWEntity(const NWEntity&){}
	~NWEntity();

	bool Init(D3DSys* sys, char* modelName, WCHAR* textureName);
	void Render(D3DCamera* camera, D3DShaderManager* sm);

	void SetOnlineStatus(bool flag){m_online = flag;}
	void SetID(unsigned short ID){m_EntityID = ID;}
	void SetType(char type){m_EntityType = type;}
	void SetPosition(float x, float y, float z){ m_PosX = x; m_PosY = y; m_PosZ = z; }
	void SetRotation(float x, float y, float z){ m_RotX = x; m_RotY = y; m_RotZ = z; }
	bool IsOnline(){return m_online;}
	unsigned short GetID(){return m_EntityID;}
	char GetEntityType(){return m_EntityType;}
	void GetPosition(float& x, float& y, float& z){ x = m_PosX;y = m_PosY; z = m_PosZ;}
	void GetRotation(float& x, float& y, float& z){ x = m_RotX;y = m_RotY; z = m_RotZ;}
	void UpdateState(char state);
	void Update( float deltaTime );
	void UpdatePosition(float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
	void UpdateRotateState(bool rotate){m_turningRight = rotate;}


private:
	D3DModel* m_EntityModel;
	D3DSys* m_D3DSys;
	bool m_online;
	unsigned short m_EntityID;
	char m_EntityType;
	float m_PosX, m_PosY, m_PosZ;
	float m_RotX, m_RotY, m_RotZ;
	bool m_movingForward, m_movingBackward, m_turningLeft, m_turningRight;
};
