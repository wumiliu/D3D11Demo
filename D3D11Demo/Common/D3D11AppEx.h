#pragma once
#include "HpD3D9Type.h"
#include "GameTimer.h"
class D3D11AppEx
{
public:
	D3D11AppEx(HINSTANCE hInstance);
	~D3D11AppEx();
	int Run();

	virtual bool Init();
	virtual void OnResize();
	virtual void DrawScene() = 0;
	virtual void InitResource() = 0;
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnMouseWheel(short zDelta, int x, int y);
	virtual void OnLButtonDblClk(WPARAM btnState, int x, int y){};

	DirectX::SimpleMath::Ray CalcPickingRay(int x, int y);
protected:
	bool InitMainWindow();
	bool InitDirect3D();


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
	std::shared_ptr<class CameraComponent> cameraComponent;

	bool bMouseDown;
	MousePos mouseLast;


};

