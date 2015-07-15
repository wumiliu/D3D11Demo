
// Test_3DDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Test_3D.h"
#include "Test_3DDlg.h"
#include "afxdialogex.h"
#include <afxpriv.h>
#include <MMSystem.h>
#include <WindowsX.h>
#include <sstream>

using namespace DirectX;
#pragma comment(lib, "winmm.lib") 
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace DirectX;

// CTest_3DDlg 对话框



CTest_3DDlg::CTest_3DDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CTest_3DDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
#if defined(DEBUG) || defined(_DEBUG)
	FILE *f(NULL);
	if (AllocConsole())
	{
		freopen_s(&f, "CONOUT$", "w", stdout);
	}
#endif
	//_CrtSetBreakAlloc(738);
}

void CTest_3DDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest_3DDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)			// The fucking MFC trick
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CTest_3DDlg 消息处理程序

BOOL CTest_3DDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);


	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码


	this->MoveWindow(0, 0, 1024 + GetSystemMetrics(SM_CXFIXEDFRAME) * 2, 600 + GetSystemMetrics(SM_CYFIXEDFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION), TRUE);


	CRect rcClient;
	GetClientRect(&rcClient);
	int nSceneWidth = rcClient.Width();
	int nSceneHeight = rcClient.Height();

	//	PlaySound(L"D:\\VS2010\\GameMedia\\Music\\Eternal Love (Short Version).wav", NULL, SND_FILENAME | SND_ASYNC|SND_LOOP);   //循环播放背景音乐
	if (g_objBase.Initialize(m_hWnd, nSceneWidth, nSceneHeight) != S_OK)
	{
		this->MessageBox(L"3D 初始化失败!", L"error", MB_OK | MB_ICONERROR);
		this->EndDialog(IDCANCEL);
		return TRUE;
	}


	g_objCamera.SetPosition(0.0f, 0.0f, -10.0f);
	float  aspect = static_cast<float>(nSceneWidth) / nSceneHeight;
	g_objCamera.SetLens(0.25f*XM_PI, aspect, 1.0f, 3000.0f);
	m_Curve = AnimationCurve::Cubic(-507+20, 280, 507-20, -280);
	//m_Curve->AddKey(-180, 200);
	m_Curve->AddKey(-400, -100);

	//m_Curve->AddKey(-200, 0);
	//m_Curve->AddKey(0, 0);
	m_Curve->AddKey(100, 200);
	m_bMove = false;
	XMVECTOR Eay = XMLoadFloat3(&XMFLOAT3(4.0f,3.0f,3.0f));
	XMVECTOR f= XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR l = XMLoadFloat3(&XMFLOAT3(.0f, -1.0f, 0.0f));

	XMMATRIX m = XMMatrixLookAtLH(Eay ,f, l);
	float AspectHByW = 743.0f / 474.0f;
	m = XMMatrixPerspectiveFovLH(XM_PI* 45.0f/180.0f, AspectHByW,0.1f, 100.0f);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTest_3DDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTest_3DDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CTest_3DDlg::OnKickIdle(WPARAM wp, LPARAM lCount)
{
	static DWORD timeLoop = 0;
	const DWORD FRAME_INTERVAL = 30; //30 毫秒

	DWORD timerNow = timeGetTime();

	if (timerNow < timeLoop + FRAME_INTERVAL)
	{
		::Sleep(timeLoop + FRAME_INTERVAL - timerNow);
	}
	else
	{
		////一帧需要的时间，乘以 速度
		float deltaTime = (timerNow - timeLoop)*0.001f;
		if (GetAsyncKeyState('W') & 0x8000)
			g_objCamera.Walk(10.0f*deltaTime);

		if (GetAsyncKeyState('S') & 0x8000)
			g_objCamera.Walk(-10.0f*deltaTime);

		if (GetAsyncKeyState('A') & 0x8000)
			g_objCamera.Strafe(-10.0f*deltaTime);
		//一帧需要的时间，乘以 速度
		if (GetAsyncKeyState('D') & 0x8000)
			g_objCamera.Strafe(10.0f*deltaTime);

		timeLoop = timeGetTime();
		CalculateFrameStats();
		g_objCamera.UpdateViewMatrix();
		this->OnTimer(1);
	}
	return 1;
}

void CTest_3DDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		g_objBase.ClearBuffer(0);
	//	g_objSprite.ShowBlock(400, 100, 500, 500, XMFLOAT4(1.0f, 0.5f, 0.5f, 1));
			CRect rcClient;
			GetClientRect(&rcClient);
			double nSceneWidth = rcClient.Width();
			double nSceneHeight = rcClient.Height();

# define NDIM 1
# define NDATA 3
			int left;
			double xdata[NDATA];
			double xval;
			double ydata[NDIM*NDATA];
			double yval[NDIM];
			//y = -(x)**2
			xdata[0] = -507;
			ydata[0] = 300;
			xdata[1] = 0;
			ydata[1] = 0;
			xdata[2] = 107;
			ydata[2] = 300;
			xval = -200.0;
			DirectX::VertexPositionColor vertices[6000];

			int i = 0;
			for (double x = -nSceneWidth / 2; (x < nSceneWidth / 2) && (i < 1445); x++)
			{
				xval =x;
				left = i;
				if (NDATA - 2 < left)
				{
					left = NDATA - 2;
				}
				if (left < 1)
				{
					left = 1;
				}
				double yval1 = m_Curve->Evaluate(xval);
				parabola_val2(NDIM, NDATA, xdata, ydata, left, (double)xval, yval);
				double xtmp = xval;// xval / 28 * 10.f;
				double ytmp = yval[0];// yval[0] / nSceneHeight / 28 * 10.0f;
				ytmp = yval1 ;
	     		xtmp = xval + nSceneWidth / 2;
				ytmp =  nSceneHeight / 2 - ytmp;

				vertices[i] = DirectX::VertexPositionColor(XMFLOAT3((float)xtmp, ytmp, 0.0f), XMFLOAT4(0.5f, 1.0f, 0.5f, 1));
				//vertices[i] = DirectX::VertexPositionColor(XMFLOAT3((float)xtmp, 253, 0.0f), XMFLOAT4(0.5f, 1.0f, 0.5f, 1));
				i++;
			}

			static int kkkkkk = 0;
			if (kkkkkk > nSceneWidth - 1)
			{
				kkkkkk = 0;
			}
			g_objBase.TurnZBufferOff();
			//vertices[kkkkkk].color = XMFLOAT4(DirectX::Colors::Red);
			g_objSprite.DrawPrimitiveUP(PRIMITIVE_TYPE::PRIMITIVE_LINESTRIP, nSceneWidth, vertices, false);

			int nPosX = vertices[kkkkkk].position.x;// +nSceneWidth / 2;
			int nPosY = vertices[kkkkkk].position.y;

			g_objSprite.ShowBlock(nPosX-5, nPosY - 5, nPosX + 5, nPosY + 5, XMFLOAT4(DirectX::Colors::Red));
			nPosX = 200;
		//	nPosY = nSceneHeight / 2 - vertices[kkkkkk].position.y;
			//g_objSprite.ShowBlock(nPosY, nPosX, nPosY + 100, nPosX + 100, XMFLOAT4(DirectX::Colors::Red));
			kkkkkk++;

			for (int i = 0; i < m_Curve->Size(); ++i)
			{
				int xPos = m_Curve->Key(i) + nSceneWidth / 2;
				int yPos = nSceneHeight / 2 -  m_Curve->Value(i);
				g_objSprite.ShowBlock(xPos - 5, yPos - 5, xPos + 5, yPos + 5, XMFLOAT4(DirectX::Colors::Blue));
			}
			CTexture* pTexture1111 = g_objTextureMgr.GetTexture("Texture\\4.dds");
			pTexture1111->Show(10, 10);
			CRect pSrcTmp1;
			pSrcTmp1.left = 0;
			pSrcTmp1.top = 0;

			pSrcTmp1.right = pTexture1111->GetWidth() - 15;
			pSrcTmp1.bottom = pTexture1111->GetHegith() / 2;
			g_objSprite.ShowTextureEx(20, 20, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, true);

	
			pSrcTmp1.left = 0;
			pSrcTmp1.top = pTexture1111->GetHegith() / 2;

			pSrcTmp1.right = pTexture1111->GetWidth();
			pSrcTmp1.bottom = pTexture1111->GetHegith();
		//	g_objSprite.ShowTextureEx(50, 100, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, true);

		
	//	 g_objBase.Flip();
	//	return;
		CTexture* pTexture1 = g_objTextureMgr.GetTexture("Texture\\4.dds");

		float mm = (timeGetTime() % 1501) / 1000.0f;
		//	mm /= 5.0f;
		int nStopPos = 1024;
		int nStartPos = 0;
		static int nCurrentPos = nStartPos;
		RECT pSrcTmp;
		//if (mm >= 1.0f)
		{
		//	printf("%f\n", mm);
		}
		nCurrentPos = nStopPos*mm + nStartPos;
		pSrcTmp.left = 0;
		pSrcTmp.top = 0;
	
		pSrcTmp.right = pTexture1->GetWidth() -15;
		pSrcTmp.bottom = pTexture1->GetHegith() / 2;
		for (int i = 0; i < 30; i++)
		{
		//	pTexture1->ShowEx(nCurrentPos, 10 + 30 * i, &pSrcTmp, pSrcTmp.right, pSrcTmp.bottom);
		}

		nCurrentPos = nCurrentPos + pSrcTmp.right - 0;
		pSrcTmp.left = 15;
		pSrcTmp.top = pTexture1->GetHegith() / 2;
		pSrcTmp.right = pTexture1->GetWidth();
		pSrcTmp.bottom = pTexture1->GetHegith();
		for (int i = 0; i < 30; i++)
		{
		//	pTexture1->ShowEx(nCurrentPos, 10 + 30 * i, &pSrcTmp, pSrcTmp.right, pTexture1->GetHegith() / 2);
		}
		mm = (timeGetTime() % 1501) / 1000.0f;

		nCurrentPos = nStopPos*mm + nStartPos;
		pSrcTmp.left = 0;
		pSrcTmp.top = 0;
		pSrcTmp.right = pTexture1->GetWidth() - 15;
		pSrcTmp.bottom = pTexture1->GetHegith() / 2;

		g_objSprite.ShowBlock(nPosY, nPosX, nPosY + 100, nPosX + 100, XMFLOAT4(DirectX::Colors::Red));

		nCurrentPos = vertices[kkkkkk].position.y;;
		bool bRotate = false;
		float fRotate = 0.5f;
		if( vertices[kkkkkk].position.y -   vertices[kkkkkk+1].position.y > 0 )
		{
			bRotate = true;
			fRotate = 1.5f;
			g_objSprite.ShowTextureEx(200, nCurrentPos + pSrcTmp.right, &pSrcTmp, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, fRotate);
			nCurrentPos = nCurrentPos ;
			pSrcTmp.left = 15;
			pSrcTmp.top = pTexture1->GetHegith() / 2;
			pSrcTmp.right = pTexture1->GetWidth();
			pSrcTmp.bottom = pTexture1->GetHegith();
			g_objSprite.ShowTextureEx(200, nCurrentPos, &pSrcTmp, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, fRotate);
		}
		else
		{

			g_objSprite.ShowTextureEx(200, nCurrentPos, &pSrcTmp, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, fRotate);
			nCurrentPos = nCurrentPos + pSrcTmp.right - 0;
			pSrcTmp.left = 15;
			pSrcTmp.top = pTexture1->GetHegith() / 2;
			pSrcTmp.right = pTexture1->GetWidth();
			pSrcTmp.bottom = pTexture1->GetHegith();
			g_objSprite.ShowTextureEx(200, nCurrentPos, &pSrcTmp, pSrcTmp1.Width(), pSrcTmp1.Height(), pTexture1111, fRotate);

		}
		CTexture* pTmpTexture = g_objTextureMgr.GetTexture("NPC1.png");
		pSrcTmp1.top = 0;
		pSrcTmp1.left = 0;
		pSrcTmp1.right = pTmpTexture->GetWidth();
		pSrcTmp1.bottom = pTmpTexture->GetHegith();
		float fff = XM_PI *(::timeGetTime() / 1000.0f / 3);
		g_objSprite.ShowTextureEx(300, 200, &pSrcTmp1, 100, 250 , pTmpTexture, fff);
	/*	g_objSprite.ShowTextureEx(300, 200, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height() + 50, pTmpTexture, 0.5f);
														 
		g_objSprite.ShowTextureEx(300, 200, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height(), pTmpTexture, 1.0f);
		g_objSprite.ShowTextureEx(300, 200, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height(), pTmpTexture, 1.5f);*/
	

		g_objSprite.ShowBlock(300, 200, 300 + 10, 200 + 50, XMFLOAT4(DirectX::Colors::Peru),fff);
		g_objSprite.ShowRect(300, 200, 415, 300, XMFLOAT4(DirectX::Colors::Peru), true);
	//	g_objSprite.ShowTextureEx(100, 100, &pSrcTmp1, pSrcTmp1.Width(), pSrcTmp1.Height(), pTmpTexture, 0.0f);
	//	pTmpTexture->Show(100, 100);
		g_objBase.TurnZBufferOn();
		g_objBase.Flip();
	}
}


void CTest_3DDlg::OnDestroy()
{
	
}

void CTest_3DDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
#if defined(DEBUG) || defined(_DEBUG)
	//std::cout << std::endl;
	//	std::cout << m;
	CRect rcClient;
	GetClientRect(&rcClient);
	double nSceneWidth = rcClient.Width();
	double nSceneHeight = rcClient.Height();
	int nPosX = point.x - nSceneWidth / 2;
	int nPosY = nSceneHeight / 2 - point.y;
	int nInsertIndex = 0;

 	for (int i = 0; i < m_Curve->Size(); ++i)
	{
		int xPos = m_Curve->Key(i) + nSceneWidth / 2;;
		int yPos = nSceneHeight / 2 - m_Curve->Value(i);
		CRect _rect;
		_rect.left = xPos - 5;
		_rect.top = yPos - 5;
		_rect.right = _rect.left + 10;
		_rect.bottom = _rect.top + 10;
		POINT _point;
		_point.x = nPosX;
		_point.y = nPosY;

		if (_rect.PtInRect(point))
		{
			std::cout << "OK" << std::endl;
			m_bMove = true;
			m_Pos = point;
			m_index = i;
			return;
		}

	}
//	return;
	//m_Curve->AddKey(nPosX, nPosY);
	std::cout << "X坐标: " << point.x  << "   " << "Y坐标: " <<  point.y << std::endl;
#endif

}

float	Get_FPS()
{
	//定义四个静态变量
	static float  fps = 0; //我们需要计算的FPS值
	static int    frameCount = 0;//帧数
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

	//如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
	if (currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount / (currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		frameCount = 0;//将本次帧数frameCount值清零
	}

	return fps;
}


void CTest_3DDlg::CalculateFrameStats()
{

	//定义四个静态变量
	static float  fps = 0; //我们需要计算的FPS值
	static int    frameCount = 0;//帧数
	static float  currentTime = 0.0f;//当前时间
	static float  lastTime = 0.0f;//持续时间

	frameCount++;//每调用一次Get_FPS()函数，帧数自增1
	currentTime = timeGetTime()*0.001f;//获取系统时间，其中timeGetTime函数返回的是以毫秒为单位的系统时间，所以需要乘以0.001，得到单位为秒的时间

	//如果当前时间减去持续时间大于了1秒钟，就进行一次FPS的计算和持续时间的更新，并将帧数值清零
	if (currentTime - lastTime > 1.0f) //将时间控制在1秒钟
	{
		fps = (float)frameCount / (currentTime - lastTime);//计算这1秒钟的FPS值
		lastTime = currentTime; //将当前时间currentTime赋给持续时间lastTime，作为下一秒的基准时间
		std::wstring mMainWndCaption = L"D3DDemo";
		std::wostringstream outs;
		outs.precision(4);
		outs << mMainWndCaption << L"    "
			<< L"Frame Time: " << frameCount << L" (ms)";

		::SetWindowTextW(m_hWnd, outs.str().c_str());

		frameCount = 0;//将本次帧数frameCount值清零
	}
}

void CTest_3DDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_bMove)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		int nSceneWidth = rcClient.Width();
		int nSceneHeight = rcClient.Height();

		std::cout << "OKKK";
		m_Curve->RemoveKey(m_index);
		int nPosX = point.x - nSceneWidth / 2;
		int nPosY = nSceneHeight / 2 - point.y;
		
		m_Curve->AddKey(nPosX, nPosY);
	}
	CDialogEx::OnMouseMove(nFlags, point);
}


void CTest_3DDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_bMove = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void CTest_3DDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect rcClient;
	GetClientRect(&rcClient);
	int nSceneWidth = rcClient.Width();
	int nSceneHeight = rcClient.Height();

	int nPosX = point.x - nSceneWidth / 2;
	int nPosY = nSceneHeight / 2 - point.y;

	m_Curve->AddKey(nPosX, nPosY);
	CDialogEx::OnLButtonDblClk(nFlags, point);
}
