#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
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

protected:

	virtual bool dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender();

private:

	void Render2D();
	void Render3D();

	D3DModel* m_Cube;
	D3DModel* m_BillBoard;
	D3DModel* m_Floor;
	D3DModel* m_DaveDude;
	D3DLight* m_Light;
	D3DCamera* m_Camera;
	D3DUI* m_GameUI;
	D3DParticleEmitter* m_ParticleSystem;
	D3DSkyBox* m_SkyBox;
	NWSystem* m_Network;

	bool m_MouseLock;
};

#endif