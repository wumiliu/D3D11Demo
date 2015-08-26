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
#include "Camera/CameraComponent.h"
#include "Texture/DX11RTTexture.h"
#include "Camera/CameraHelp.h"
using namespace GeoGen;

PickDemo::PickDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	:D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


PickDemo::~PickDemo()
{
	SAFE_RELEASE(srv);
}

void PickDemo::InitResource()
{
	DX11RTTexturePtr = std::make_shared<DX11RTTexture>();
	m_CubeModel = std::make_shared<D3D11RendererMesh>();
	m_Cylinder = std::make_shared<D3D11RendererMesh>();

	m_CubeModelHelp = std::make_shared<D3D11RendererMesh>();

	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "baseMeshPS.hlsl";
	desc.vecPass.push_back("main");
	desc.vecPass.push_back("mainPick");
	desc.vecPass.push_back("mainRed");
	desc.vecPass.push_back("mainGreen");
	desc.vecPass.push_back("mainBlue");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	MeshData meshData;
	GeoGen::CreateBox(2, 2, 2, meshData);
	m_CubeModelHelp->BuildBuffers(meshData);
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

	// Setup the camera   
	Vector3 vecEye(0.0f, 0.0f, 0.0f);
	Vector3 vecAt(0.0f, 0.0f, -1.0f);

	cameraComponent->SetViewParams(vecEye, vecAt);
	float fAspectRatio = (float)mClientWidth / (float)mClientHeight;
	cameraComponent->SetProjParams(XM_PIDIV4, fAspectRatio, 10.0f, 1010.0f);
	DX11RTTexturePtr->Create(mClientWidth, mClientHeight);

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
	RenderRT();

	SwapChainPtr->Begin();
	ResetState();
	RenderSystemAxis();
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();



	g_objSprite.ShowBlock(500, 50, 700, 100, { 1, 0, 1, 1 });
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

	CameraHelp cameraHelp;
	std::vector<VertexPositionColor> vertexsCamera;
	cameraComponent->GetFrustumMeshElements(vertexsCamera);
	//cameraHelp.GetFrustumMeshElements(vertexsCamera,mClientWidth,mClientHeight);

	Matrix world = Matrix::CreateTranslation(0, 6.0f, -12.0f);// *cameraComponent->GetProjMatrix();
//	world *= (cameraComponent->GetViewMatrix()* cameraComponent->GetProjMatrix());
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModelHelp->render(m_Material.get(), 4);

	world = world*(cameraComponent->GetViewMatrix()* cameraComponent->GetProjMatrix()) ;
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModelHelp->render(m_Material.get(), 4);

	world = cameraComponent->GetViewMatrix();
	world = cameraComponent->GetViewMatrix().Invert();
//	world = cameraComponent->GetProjMatrix();

	g_objSprite.DrawPrimitiveUP(PRIMITIVE_LINELIST, 24, &vertexsCamera[0], world*viewMatrix* projectionMatrix);
	world = cameraComponent->GetProjMatrix();
	g_objSprite.DrawPrimitiveUP(PRIMITIVE_LINELIST, 24, &vertexsCamera[0], world*viewMatrix* projectionMatrix);

	int x = mClientWidth * 0.66666f - 10;
	int y = mClientHeight * 0.66666f - 10;
	int width = mClientWidth + 10;
	int height = mClientHeight + 10;

	RECT rect1;
	rect1.left = 0;
	rect1.right = DX11RTTexturePtr->m_nWidth;
	rect1.top = 0;
	rect1.bottom = DX11RTTexturePtr->m_nHeight;
	RECT rect;
	rect.left = mClientWidth * 0.66666f - 5;
	rect.right = mClientWidth - 5;
	rect.top = mClientHeight * 0.66666f - 5;
	rect.bottom = mClientHeight - 5;

	g_objSprite.ShowBlock(x, y, x + width, y + height, { 0.000000000f, 0.000000000f, 0.000000000f, 0.500000000f });
	g_objSprite.ShowTexEx(&rect, &rect1, rect1.right, rect1.bottom, DX11RTTexturePtr->GetRTView());




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

void PickDemo::RenderSystemAxis()
{
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();
	ID3D11SamplerState* LinearClamp = g_objStates.LinearClamp();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);
	//m_deviceContext->PSSetSamplers(g_objStates.PointWrap());

	m_Material->PSSetShaderResources(0, 1, srv);
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	//return;
	Matrix world = Matrix::CreateScale(0.5, 0.5, 0.5) * Matrix::CreateTranslation(0, 5.5f*0.5f, 0);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get(), 3);
	world *= Matrix::CreateRotationX(XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get(), 4);

	world = Matrix::CreateScale(0.5, 0.5, 0.5) * Matrix::CreateTranslation(0, 5.5f*0.5f, 0);
	world *= Matrix::CreateRotationZ(-XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_CubeModel->render(m_Material.get(), 2);
	m_CubeModel->renderHelp(m_Material.get());
	world = Matrix::CreateScale(0.5f, 0.5f, 0.5f) * Matrix::CreateTranslation(0.0f, 2.5f*0.5f, 0.0f);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get(), 3);

	world *= Matrix::CreateRotationX(XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get(), 4);
	world = Matrix::CreateScale(0.5f, 0.5f, 0.5f) * Matrix::CreateTranslation(0.0f, 2.5f*0.5f, 0.0f);
	world *= Matrix::CreateRotationZ(-XM_PIDIV2);
	m_Material->SetShaderParameters(world, viewMatrix, projectionMatrix);
	m_Cylinder->render(m_Material.get(), 2);
}

void PickDemo::RenderRT()
{
	DX11RTTexturePtr->Begin();
	g_objSprite.ResetSize(mClientWidth, mClientHeight);
	//g_objSprite.ShowRect(100, 100, 300, 200, { 1.0f, 0.65f, 0.3f, 1 }, true);

	Matrix world = Matrix::CreateTranslation(0,6.0f, -12.0f);
	m_Material->SetShaderParameters(world, cameraComponent->GetViewMatrix(), cameraComponent->GetProjMatrix());
	m_CubeModelHelp->render(m_Material.get(), 4);

	DX11RTTexturePtr->End();
}
