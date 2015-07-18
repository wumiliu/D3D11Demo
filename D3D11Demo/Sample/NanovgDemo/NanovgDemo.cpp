#include "NanovgDemo.h"
#include "SwapChain.h"
#include "nanovg/nanovg.h"
#include "nanovg/nanovg_d3d11.h"


NanovgDemo::NanovgDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
:D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}



NanovgDemo::~NanovgDemo()
{
}

void NanovgDemo::UpdateScene(float dt)
{

}

void NanovgDemo::DrawScene()
{
	SwapChainPtr->Begin();
	nvgBeginFrame(NVGcontextPtr, mClientWidth, mClientHeight, 1.0f);
	float t = (timeGetTime() % 1501) / 1000.0f;
	t = XM_PI *(::timeGetTime() / 1000.0f / 3);
//	t = mTimer.TotalTime() * XM_PI;

	renderDemo(NVGcontextPtr, (float)0, (float)0, (float)mClientWidth, (float)mClientHeight, (float)t, 0, &data);

	nvgEndFrame(NVGcontextPtr);
	SwapChainPtr->Flip();

}

void NanovgDemo::InitResource()
{
	NVGcontextPtr = nvgCreateD3D11(m_d3dDevice, NVG_ANTIALIAS || NVG_STENCIL_STROKES);
	if (loadDemoData(NVGcontextPtr, &data) == -1)
	{
		return ;
	}
}
