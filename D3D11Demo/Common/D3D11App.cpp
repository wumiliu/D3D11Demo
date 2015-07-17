#include "D3D11App.h"
#include <WindowsX.h>
#include <sstream>
#include "SwapChain.h"
#include "DeviceManager.h"
#include "TrackballCameraController.h"
namespace
{
	// This is just used to forward Windows messages from a global window
	// procedure to our member function window procedure because we cannot
	// assign a member function to WNDCLASS::lpfnWndProc.
	D3D11App* gd3dApp = 0;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}


D3D11App::D3D11App(HINSTANCE hInstance)
: mhAppInst(hInstance),
mMainWndCaption(L"D3D11 Application"),
mClientWidth(800),
mClientHeight(600),
mAppPaused(false),
mMinimized(false),
mMaximized(false),
mResizing(false)
{
	gd3dApp = this;
	SwapChainPtr = std::make_shared<SwapChain>();
	bMouseDown = false;
}

D3D11App::~D3D11App()
{

}

bool D3D11App::Init()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;
	InitResource();
	float AspectHByW = (float)mClientWidth / (float)mClientHeight;
	g_objTrackballCameraController.ProjParams(DirectX::XM_PI*0.25f, AspectHByW, 1.0f, 1000.0f);
	mTimer.Start();
	return true;
}

bool D3D11App::InitMainWindow()
{
	WNDCLASS wc;
	//CS_DBLCLKS 双击事件
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindowW(L"D3DWndClassName", mMainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

LRESULT D3D11App::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
		if ((mClientWidthOld != mClientWidth) && (mClientHeightOld!= mClientHeight))
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

int D3D11App::Run()
{
	MSG msg = { 0 };
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
				const DWORD FRAME_INTERVAL = 30; //15 毫秒
				DWORD timerNow = mTimer.GetTime();
				if (timerNow < timeLoop + FRAME_INTERVAL)
				{

				}
				else
				{
					timeLoop = mTimer.GetTime();
					CalculateFrameStats();
					UpdateScene(mTimer.DeltaTime());
					DrawScene();
				}
			}
		}
	}
	return (int)msg.wParam;
}

bool D3D11App::InitDirect3D()
{
	SwapChainPtr->Initialize(mhMainWnd, mClientWidth, mClientHeight);
	m_d3dDevice = g_objDeviecManager.GetDevice();
	m_deviceContext = g_objDeviecManager.GetImmediateContext();

	return true;
}

void D3D11App::OnResize()
{
	SwapChainPtr->OnResize(mClientWidth,mClientHeight);
	printf("OnResize");
}

void D3D11App::CalculateFrameStats()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCnt++;
	currentTime = mTimer.GetTime() * 0.001f;
	// Compute averages over one second period.
	if ((currentTime - lastTime) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		

		fps = (float)frameCnt / (currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		float mspf = 1000.0f / fps;
		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

}

void D3D11App::OnMouseDown(WPARAM btnState, int x, int y)
{
	bMouseDown = true;
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11App::OnMouseUp(WPARAM btnState, int x, int y)
{
	bMouseDown = false;
	mouseLast.X = x;
	mouseLast.Y = y;
}

void D3D11App::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (bMouseDown)
	{
		MousePos _tmp(x, y);
		x -= mouseLast.X;
		y -= mouseLast.Y;
		mouseLast.X = _tmp.X;
		mouseLast.Y = _tmp.Y;
		g_objTrackballCameraController.Rotate((float)x, (float)y);
		printf("%d   %d\n", x, y);
	}
}

void D3D11App::OnMouseWheel(short zDelta, int x, int y)
{
	g_objTrackballCameraController.Zoom(zDelta, 0);
}

DirectX::SimpleMath::Ray D3D11App::CalcPickingRay(int sx, int sy)
{
	XMMATRIX view = g_objTrackballCameraController.View();
	//投影变换
	Matrix P = g_objTrackballCameraController.Proj();

	float vx = (+2.0f*sx / mClientWidth - 1.0f) / (P(0, 0));
	float vy = (-2.0f*sy / mClientHeight + 1.0f) / (P(1, 1));
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
