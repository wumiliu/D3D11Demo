#include "LightDemo.h"
#include "MeshRender.h"
#include "SwapChain.h"

using namespace GeoGen;

LightDemo::LightDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	:D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


LightDemo::~LightDemo()
{

}

void LightDemo::InitResource()
{
	m_CubeModel = std::make_shared<MeshRender>();
	m_GroundModel = std::make_shared<MeshRender>();
	m_SphereModel = std::make_shared<MeshRender>();
	MeshData meshData;
	GeoGen::CreateBox(2, 2, 2, meshData);
	bool bBuild = m_CubeModel->BuildBuffers(meshData, L"data\\wall01.dds");
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);
	GeoGen::CreateSphere(1, 50, 50, meshData);
	bBuild = m_SphereModel->BuildBuffers(meshData, L"data\\ice.dds");
	m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);
	GeoGen::CreateGrid(10, 10, 1, 1, meshData);
	bBuild = m_GroundModel->BuildBuffers(meshData, L"data\\metal001.dds");
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);

}

void LightDemo::UpdateScene(float dt)
{

}

void LightDemo::DrawScene()
{
	SwapChainPtr->Begin();

	SwapChainPtr->Flip();
	
}
