#pragma once

#ifndef __D3D11_APP_H__
#define __D3D11_APP_H__

#include <Windows.h>
#include <windowsx.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <string>

#include "d3dx11Effect.h"
#include "GameTimer.h"
#include "D3D11Utils.h"

class D3D11App
{

public:
	
	D3D11App(HINSTANCE hInstance);
	virtual ~D3D11App();

public:

	HINSTANCE AppInstance() const;
	HWND MainWnd() const;
	float AspectRatio() const;

public:

	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float DeltaTime) = 0;
	virtual void DrawScene() = 0;
	virtual LRESULT MsgProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual void OnMouseDown(WPARAM BtnState, int x, int y);
	virtual void OnMouseUp(WPARAM BtnState, int x, int y);
	virtual void OnMouseMove(WPARAM BtnState, int x, int y);

protected:

	bool InitMainWindow();
	bool InitDirect3D();
	void CalculateFrameStats();

protected:

	HINSTANCE m_hAppInstance;
	HWND      m_hMainWnd;
	bool      m_bAppPaused;
	bool      m_bMinimized;
	bool      m_bMaximized;
	bool      m_bResizing;
	UINT      m_4xMsaaQuality;

	GameTimer m_GameTimer;

	ID3D11Device*				m_D3D11Device;
	ID3D11DeviceContext*		m_D3D11ImmediateContext;
	IDXGISwapChain*				m_SwapChain;
	ID3D11Texture2D*			m_DepthStencilBuffer;
	ID3D11RenderTargetView*		m_RenderTargetView;
	ID3D11DepthStencilView*		m_DepthStencilView;
	D3D11_VIEWPORT				m_ScreenViewport;

protected:

	std::wstring				m_MainWndCaption;
	D3D_DRIVER_TYPE				m_D3D11DriverType;
	int							m_ClientWidth;
	int							m_ClientHeight;
	bool						m_bEnable4xMsaa;

};

#endif