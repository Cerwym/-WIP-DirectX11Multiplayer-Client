#pragma once

#include <DXGI.h>
#include <D3Dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include "Defines.h"

class D3DSys
{
public:
	D3DSys();
	D3DSys(const D3DSys&);
	~D3DSys();

	// int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear
	bool Init( int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenNear, float screenFar );
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11DepthStencilView* GetDepthStencilView(){return m_depthStencilView;}

	D3DXMATRIX GetOrthoMatrix();
	void GetVideoCardInfo(char*, int&);
	void SetBackBufferRenderTarget(){m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);}

	void TurnZBufferOn(){m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);}
	void TurnZBufferOff(){m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);}
	void TurnOnCulling(){m_deviceContext->RSSetState(m_rasterState);}
	void TurnOffCulling(){m_deviceContext->RSSetState(m_RasterStateCULLNONE);}
	void TurnOnWireFrame(){m_deviceContext->RSSetState(m_RasterStateWireFrame);}
	void TurnOffWireFrame(){m_deviceContext->RSSetState(m_rasterState);}
	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

private:
	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];

	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_RasterStateCULLNONE;
	ID3D11BlendState* m_alphaEnableBlendingState;
	ID3D11BlendState* m_alphaDisableBlendingState;
	ID3D11RasterizerState* m_rasterState;
	ID3D11RasterizerState* m_RasterStateWireFrame;

	D3DXMATRIX m_orthoMatrix;
};

