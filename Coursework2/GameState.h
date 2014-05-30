#pragma once

#include "Player.h"
#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/NWEntity.h"
#include "DXEngine/NWSystem.h"
#include "DXEngine/Objects/D3DModel.h"
#include "DXEngine/Objects/D3DUI.h"
#include "DXEngine/Objects/D3DCamera.h"
#include "DXEngine/Objects/D3DLight.h"
#include "DXEngine/Objects/D3DParticleEmitter.h"
#include "DXEngine/Objects/D3DSkybox.h"

#include "DXEngine/Shaders/D3DTextureShader.h"

class GameState : public State
{
public:
	GameState();

	// Networking functions
	void AddEntity(unsigned short entID, char endType, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
	void RemoveEntity(unsigned short entID);
	bool GetStateChange( char& state);
	void UpdateEntityState( unsigned short entID, char state);
	bool PositionUpdate(float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ);
	void UpdateEntityPosition(unsigned short entID, float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ);
	void UpdateEntityRotate(unsigned short ID, bool rotate);
	void HandleNewPositionData(float x, float y, float z);

protected:

	virtual bool dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender();

private:

	void Render2D();
	void Render3D();
	void HandleMovement( float dt );
	void UpdateInput( float dt );

	Player* m_Player;
	D3DModel* m_BillBoard;
	D3DModel* m_Floor;
	D3DModel* m_DaveDude;
	D3DLight* m_Light;
	D3DCamera* m_Camera;
	D3DUI* m_GameUI;
	D3DParticleEmitter* m_ParticleSystem;
	D3DSkyBox* m_SkyBox;

	// Networking information
	NWSystem* m_Network;
	NWEntity* m_NetworkEntityList;

	// Networking (client) information
	bool m_stateChange;
	char m_newState;
	unsigned long m_updateTime;
	bool m_positionUpdateReady;

	bool m_TypingMessage;

	bool m_MouseLock;
};