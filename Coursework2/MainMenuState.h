#pragma once

#include "DXEngine/Engine.h" // todo remove dependancy on global
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/Objects/D3DModel.h"
#include "DXEngine/Objects/D3DCamera.h"
#include "DXEngine/Objects/D3DLight.h"
#include "DXEngine/Objects/D3DBitmap.h"
#include "DXEngine/Objects/D3DParticleEmitter.h"
#include "DXEngine/Objects/D3DSkybox.h"

class MainMenuState : public State
{
public:
	MainMenuState();

protected:
	virtual bool dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender(/* = 0 */);
private:

	void Render2D();
	void Render3D( bool renderParticles = true, bool renderSkybox = true);

	D3DCamera* m_Camera;	
	D3DBitmap* m_DebugView;
	D3DBitmap* m_DebugView2;
	D3DBitmap* m_DebugView3;
	D3DModel* m_MarbleCube;
	D3DModel* m_MetalCube;
	D3DModel* m_StoneCube;

	D3DBitmap* m_TestBitmap;

	D3DSkyBox* m_SkyBox;
	D3DParticleEmitter* m_RainEmitter;

	//D3DModel* m_Sphere;
	D3DLight* m_Light;
	float m_cubeRotation;
	bool m_MouseLock;

	XMMATRIX m_WorldMatrix, m_baseViewMatrix, m_ProjectionMatrix, m_OrthoMatrix;
	D3DText* m_TextBatch;
};

