#ifndef MAINMENUSTATE
#define MAINMENUSTATE

#include "DXEngine/Engine.h" // todo remove dependancy on global
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/Objects/D3DModel.h"
#include "DXEngine/Objects/D3DCamera.h"
#include "DXEngine/Objects/D3DLight.h"

class MainMenuState : public State
{
public:
	MainMenuState();

protected:
	virtual void dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender(/* = 0 */);
private:

	void Render2D();
	void Render3D();

	D3DCamera* m_Camera;
	D3DModel* m_MarbleCube;
	D3DModel* m_MetalCube;
	D3DModel* m_StoneCube;
	D3DLight* m_Light;
	float m_cubeRotation;
	bool m_MouseLock;

	D3DXMATRIX m_WorldMatrix, m_ViewMatrix, m_ProjectionMatrix, m_OrthoMatrix;
	D3DText* m_TextBatch;
};

#endif