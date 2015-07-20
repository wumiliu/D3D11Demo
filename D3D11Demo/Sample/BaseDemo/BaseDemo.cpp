#include "BaseDemo.h"
#include "SwapChain.h"

BaseDemo::BaseDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}

BaseDemo::~BaseDemo()
{

}
void BaseDemo::InitResource()
{

}
void BaseDemo::UpdateScene(float dt)
{

}

void BaseDemo::DrawScene()
{
	SwapChainPtr->Begin();
	SwapChainPtr->Flip();
}