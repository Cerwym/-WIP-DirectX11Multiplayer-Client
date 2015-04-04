#pragma once

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

#include "Player.h"

class GameState : public State
{
public:
	GameState();

	// Networking functions
	void AddEntity(unsigned short entID, char endType, D3DXVECTOR3 position, D3DXVECTOR3 rotation);
	void RemoveEntity(unsigned short entID);
	bool GetStateChange( char& state);
	void UpdateEntityState( unsigned short entID, char state);
	bool PositionUpdate(float& posX, float& posY, float& posZ, float& rotX, float& rotY, float& rotZ);
	void UpdateEntityPosition(unsigned short entID, D3DXVECTOR3& pos, D3DXVECTOR3& rot, D3DXVECTOR3& vel, D3DXVECTOR3& acc, unsigned long& timestamp);
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