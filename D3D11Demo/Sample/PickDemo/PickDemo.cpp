#include "PickDemo.h"
#include "MeshRender.h"
#include "SwapChain.h"
#include "d3dUtil.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "CommonStates.h"

using namespace GeoGen;

PickDemo::PickDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	:D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


PickDemo::~PickDemo()
{

}

void PickDemo::InitResource()
{
	m_CubeModel = std::make_shared<D3D11RendererMesh>();
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "baseMeshPS.hlsl";
	
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	MeshData meshData;
	GeoGen::CreateBox(2, 2, 2, meshData);
	//GeoGen::CreateSphere(1, 50, 50, meshData);
	bool bBuild = m_CubeModel->BuildBuffers(meshData);
	

	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"wall01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);
}

void PickDemo::UpdateScene(float dt)
{

}

void PickDemo::ResetState()
{
	// 获取键盘消息并给予设置相应的填充模式
	if (::GetAsyncKeyState('1') & 0x8000f)         // 若数字键1被按下，进行线框填充
	{
		//线框模式
		m_deviceContext->RSSetState(g_objStates.Wireframe());
	}
	if (::GetAsyncKeyState('2') & 0x8000f)         // 若数字键2被按下，进行实体填充
	{
		//线框模式
		m_deviceContext->RSSetState(g_objStates.CullNone());
	}
	if (::GetAsyncKeyState(VK_RETURN) & 0x8000)//return 切换
	{

	}
}

void PickDemo::DrawScene()
{
	SwapChainPtr->SetBackBufferRenderTarget();
	SwapChainPtr->Clear();
	ResetState();
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();
	ID3D11SamplerState* LinearClamp = g_objStates.LinearClamp();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);
	//m_deviceContext->PSSetSamplers(g_objStates.PointWrap());
	m_Material->SetShaderParameters(Matrix::Identity, viewMatrix, projectionMatrix);
	m_Material->PSSetShaderResources(0,1,srv);

	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);


	m_CubeModel->render(m_Material.get());
	
	//m_Material->SetShaderParameters(Matrix::CreateTranslation(0.5,0,0), viewMatrix, projectionMatrix);
	m_CubeModel->renderHelp(m_Material.get());

	SwapChainPtr->Flip();
	
}

void PickDemo::Pick(int x, int y)
{
	DirectX::SimpleMath::Ray ray = CalcPickingRay(x, y);
	m_CubeModel->Pick(ray);
}


void PickDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	D3D11App::OnMouseUp(btnState,x,y);
	Pick(x, y);
}
