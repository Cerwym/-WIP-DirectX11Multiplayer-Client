#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "DXEngine/Engine.h" // todo remove dependancy on global
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/Objects/D3DModel.h"
#include "DXEngine/Objects/D3DCamera.h"
#include "DXEngine/Objects/D3DLight.h"


class GameState : public State
{
public:
	GameState();

protected:

	virtual void dUpdate( float dt );
	virtual void dLoad();
	virtual void dClose();
	virtual void dRender();

private:

	D3DModel* m_Cube;
	D3DCamera* m_Camera;
};

#endif