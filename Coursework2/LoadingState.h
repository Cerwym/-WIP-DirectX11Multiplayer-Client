#pragma once

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

	struct CBNeverChanges
	{
		XMMATRIX mView;
	};

	struct CBChangesEveryFrame
	{
		XMMATRIX mWorld;
	};

	struct CBChangesOnResize
	{
		XMMATRIX mProjection;
	};
	
	bool m_MouseLock;

	CBNeverChanges cbNever;
	CBChangesEveryFrame cbEvery;
	CBChangesOnResize cbResize;

	int impMX;
};


