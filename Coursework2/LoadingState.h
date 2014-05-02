#ifndef LOADINGSTATE_H 
#define LOADINGSTATE_H

#include "DXEngine/Engine.h"
#include "DXEngine/Defines.h"
#include "DXEngine/D3DText.h"
#include "DXEngine/Objects/D3DBitmap.h"
#include "DXEngine/Objects/D3DCamera.h"

class LoadingState : public State
{
public:
	LoadingState();

protected:

	virtual bool dLoad();
	virtual void dClose();
	virtual void dUpdate( float dt );
	virtual void dRender();

private:

	D3DCamera* m_Camera;
	D3DBitmap* m_LoadingBitmap;
	D3DBitmap* m_Imp;
	D3DMATRIX m_baseViewMatrix;
	bool m_MouseLock;

	int impMX;
};


#endif