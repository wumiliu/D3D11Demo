#include "D3D11AppEx.h"
#include <WindowsX.h>
#include <sstream>
#include "SwapChain.h"
#include "DeviceManager.h"
#include "TrackballCameraController.h"
#include "Camera/CameraComponent.h"
namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3D11AppEx* gd3dApp1 = 0;
}

LRESULT CALLBACK	WndProc2(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	return gd3dApp1->MsgProc(hWnd, message, wParam, lParam);
}

D3D11AppEx::D3D11AppEx(HINSTANCE hInstance) :mhAppInst(hInstance),
mMainWndCaption(L"D3D11 Application"),
mClientWidth(800),
mClientHeight(600),
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false)
{
	gd3dApp1 = this;
	SwapChainPtr = std::make_shared<SwapChain>();
	bMouseDown = false;
}

D3D11AppEx::~D3D11AppEx()
{

}

bool D3D11AppEx::Init()
{
	InitMainWindow();
	InitDirect3D();
	InitResource();
	return true;
}

bool D3D11AppEx::InitMainWindow()
{
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = WndProc2;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = mhAppInst;
	wcex.hIcon = LoadIcon(0, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.lpszClassName = L"D3DWndClassName";;

	RegisterClassEx(&wcex);
	mhMainWnd = CreateWindow(L"D3DWndClassName", L"Win32Project", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, mhAppInst, NULL);
	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return TRUE;
}

bool D3D11AppEx::InitDirect3D()
{
	RECT  rcClient;
	GetClientRect(mhMainWnd, &rcClient);
	int nSceneWidth = rcClient.right - rcClient.left;
	int nSceneHeight = rcClient.bottom - rcClient.top;
	float AspectHByW = (float)nSceneWidth / (float)nSceneHeight;
	g_objTrackballCameraController.ProjParams(DirectX::XM_PI*0.25f, AspectHByW, 1.0f, 1000.0f);
	SwapChainPtr->Initialize(mhMainWnd, nSceneWidth, nSceneHeight);

	return true;
}
int D3D11AppEx::Run()
{
	MSG msg = { 0 };
	//MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			mTimer.Tick();
			if (!mAppPaused)
			{
				static DWORD timeLoop = 0;
				const DWORD FRAME_INTERVAL = 30; //15 ∫¡√Î
				DWORD timerNow = mTimer.GetTime();
				if (timerNow < timeLoop + FRAME_INTERVAL)
				{
					Sleep(timeLoop + FRAME_INTERVAL - timerNow);
				}
				else
				{
					timeLoop = mTimer.GetTime();
			//		CalculateFrameStats();
				//	UpdateScene(mTimer.DeltaTime());
					DrawScene();
				}
			}
		}
	}
	return (int)msg.wParam; 
}

LRESULT D3D11AppEx::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		/*
		if (LOWORD(wParam) == WA_INACTIVE)
		{
		mAppPaused = true;
		mTimer.Stop();
		}
		else
		{
		mAppPaused = false;
		mTimer.Start();
		}*/
		return 0;
		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		if (SwapChainPtr->GetState())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				mAppPaused = true;
				mMinimized = true;
				mMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				mAppPaused = false;
				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (mMinimized)
				{
					mAppPaused = false;
					mMinimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (mMaximized)
				{
					mAppPaused = false;
					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;
		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		mClientWidthOld = mClientWidth;
		mClientHeightOld = mClientHeight;
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:

		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		if ((mClientWidthOld != mClientWidth) && (mClientHeightOld != mClientHeight))
		{
			OnResize();
		}
		return 0;

		/*
		case WM_PAINT:
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
		break;*/
		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case  WM_MOUSEWHEEL:
	{
		short zDelta = (short)HIWORD(wParam);
		OnMouseWheel(zDelta, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	case  WM_LBUTTONDBLCLK:
		OnLButtonDblClk(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

void D3D11AppEx::OnResize()
{
	SwapChainPtr->OnResize(mClientWidth, mClientHeight);
}

void D3D11AppEx::OnMouseDown(WPARAM btnState, int x, int y)
{
	bMouseDown = true;
	if (btnState == 1)
	{
		mouseLast.bLeftDown = true;
	}
	else
	{
		mouseLast.bLeftDown = false;
	}
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11AppEx::OnMouseUp(WPARAM btnState, int x, int y)
{
	bMouseDown = false;
	if (btnState == 1)
	{
		mouseLast.bLeftDown = true;
	}
	else
	{
		mouseLast.bLeftDown = false;
	}
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11AppEx::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (bMouseDown)
	{
		MousePos _tmp(x, y);
		x -= mouseLast.X;
		y -= mouseLast.Y;
		mouseLast.X = _tmp.X;
		mouseLast.Y = _tmp.Y;
		if (mouseLast.bLeftDown)
		{
			g_objTrackballCameraController.Rotate((float)x, (float)y);
			cameraComponent->Rotate((float)x, (float)y);
		}
		else
		{
			g_objTrackballCameraController.Move((float)x, (float)y);
			cameraComponent->Move((float)x, (float)y);
		}

	}
}

void D3D11AppEx::OnMouseWheel(short zDelta, int x, int y)
{
	g_objTrackballCameraController.Zoom(zDelta, 0);
	cameraComponent->Zoom(zDelta, 0);
}

DirectX::SimpleMath::Ray D3D11AppEx::CalcPickingRay(int x, int y)
{
	XMMATRIX view = g_objTrackballCameraController.View();
	//Õ∂”∞±‰ªª
	Matrix P = g_objTrackballCameraController.Proj();

	float vx = (+2.0f*x / mClientWidth - 1.0f) / (P(0, 0));
	float vy = (-2.0f*y / mClientHeight + 1.0f) / (P(1, 1));
	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX invP = XMMatrixInverse(&XMMatrixDeterminant(P), P);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = view;
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);


	XMMATRIX W = Matrix::Identity;
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);


	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	DirectX::SimpleMath::Ray ray(rayOrigin, rayDir);
	return ray;
}

