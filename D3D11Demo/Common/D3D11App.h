#pragma once
#include <windows.h>
#include <d3d11.h>
#include <string>
#include <memory>
#include "GameTimer.h"
//////////////////////////////////////////////////////////////////////////
//应用程序框架类
//////////////////////////////////////////////////////////////////////////
class D3D11App
{
public:
	D3D11App(HINSTANCE hInstance);
	~D3D11App();
	// Framework methods.  Derived client class overrides these methods to 
	// implement specific application requirements.
	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void UpdateScene(float dt)=0;
	virtual void DrawScene()=0;


	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	// Convenience overrides for handling mouse input.
	virtual void OnMouseDown(WPARAM btnState, int x, int y){ }
	virtual void OnMouseUp(WPARAM btnState, int x, int y)  { }
	virtual void OnMouseMove(WPARAM btnState, int x, int y){ }
	virtual void OnMouseWheel(short zDelta, int x, int y){ }


protected:
	bool InitMainWindow();
	bool InitDirect3D();

	void CalculateFrameStats();

protected:

	HINSTANCE mhAppInst;
	HWND      mhMainWnd;
	bool      mAppPaused;
	bool      mMinimized;
	bool      mMaximized;
	bool      mResizing;

	GameTimer mTimer;

	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring mMainWndCaption;
	//D3D_DRIVER_TYPE md3dDriverType;
	int mClientWidth;
	int mClientHeight;

	int mClientWidthOld;
	int mClientHeightOld;
	std::shared_ptr<class SwapChain> SwapChainPtr;
};

