#include "PickDemo.h"
#include "MeshRender.h"
#include "SwapChain.h"
#include "d3dUtil.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "CommonStates.h"
#include "DynamicPrimitive.h"

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
	m_Cylinder = std::make_shared<D3D11RendererMesh>();


	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "baseMeshPS.hlsl";
	desc.vecPass.push_back("main");
	desc.vecPass.push_back("mainPick");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	MeshData meshData;
	GeoGen::CreateBox(2, 2, 2, meshData);
	//GeoGen::CreateSphere(1, 50, 50, meshData);

	GeoGen::CreateCylinder(0.0, 0.5f, 1, 20,20,meshData);
	GeoGen::AddCylinderBottomCap(0.0, 0.5f, 1, 20, 20, meshData);
	bool bBuild = m_CubeModel->BuildBuffers(meshData);
	GeoGen::CreateCylinder(0.25f, 0.25f, 5, 20, 20, meshData);
	GeoGen::AddCylinderBottomCap(0.25f, 0.25f, 5, 20, 20, meshData);
	m_Cylinder->BuildBuffers(meshData);

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

	m_Material->PSSetShaderResources(0,1,srv);
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	Matrix world = Matrix::CreateScale(0.5, 0.5, 0.5) * Matrix::CreateTranslation(0, 5.5f*0.5f, 0);

	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get());
	world *= Matrix::CreateRotationX(XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get());
	
	world = Matrix::CreateScale(0.5, 0.5, 0.5) * Matrix::CreateTranslation(0, 5.5f*0.5f, 0);
	world *= Matrix::CreateRotationZ(-XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get());

	//m_Material->SetShaderParameters(Matrix::CreateTranslation(0.5,0,0), viewMatrix, projectionMatrix);
	m_CubeModel->renderHelp(m_Material.get());
	world = Matrix::CreateScale(0.5f, 0.5f, 0.5f) * Matrix::CreateTranslation(0.0f, 2.5f*0.5f, 0.0f);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get());
	world *= Matrix::CreateRotationX(XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get());

	world = Matrix::CreateScale(0.5f, 0.5f, 0.5f) * Matrix::CreateTranslation(0.0f, 2.5f*0.5f, 0.0f);
	world *= Matrix::CreateRotationZ(-XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get());

	g_objSprite.ResetSize(mClientWidth, mClientHeight);
	g_objSprite.ShowBlock(100, 100, 300, 200, { 0, 0, 1, 1 }, mTimer.TotalTime());
	VertexPositionColor vertexs[] =
	{
		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(XMFLOAT3(1, 0, 0), XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1)),
		VertexPositionColor(XMFLOAT3(0, 1, 0), XMFLOAT4(0, 1, 0, 1)),

		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 1, 1)),
		VertexPositionColor(XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 1, 1))
	};
	g_objSprite.DrawPrimitiveUP(PRIMITIVE_LINELIST, 6, vertexs, viewMatrix* projectionMatrix);
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
