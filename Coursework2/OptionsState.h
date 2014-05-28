#pragma once

#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/Objects/D3DBitmap.h"
#include "DXEngine/Objects/D3DCamera.h"

class OptionsState : public State
{
public:
	OptionsState();

protected:

	virtual bool dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender();

private:

	D3DCamera* m_Camera;
	D3DBitmap* m_BackgroundTexture;
	D3DBitmap* m_Button;

	D3DXMATRIX m_baseViewMatrix;
};

