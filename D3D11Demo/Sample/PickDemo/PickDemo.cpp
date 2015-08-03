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
	m_CubeModel = std::make_shared<D3D11RendererMesh>();
	m_Cylinder = std::make_shared<D3D11RendererMesh>();


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
	Vector3 vecEye(0.95f, 5.83f, -14.48f);
	Vector3 vecAt(0.90f, 5.44f, -13.56f);

	cameraComponent->SetViewParams(vecEye, vecAt);
	float fAspectRatio = (float)mClientWidth / (float)mClientHeight;
	cameraComponent->SetProjParams(XM_PIDIV4, fAspectRatio, 10.0f, 100.0f);


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
	RenderSystemAxis();
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();

	g_objSprite.ResetSize(mClientWidth, mClientHeight);
	g_objSprite.ShowRect(100, 100, 300, 200, { 1.0f, 0.65f, 0.3f, 1 },true);

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



	float fAspectRatio = (float)mClientWidth / (float)mClientHeight;
	float FrustumStartDist = 10.0f;
	float FrustumEndDist = 1010.0f;

	float HozLength = 0.0f;
	float VertLength = 0.0f;

	HozLength = FrustumStartDist* tanf(XM_PIDIV4 / 2);
	VertLength = HozLength / fAspectRatio;

	Vector3 Direction(0, 0, 1);
	Vector3 LeftVector(1, 0, 0);
	Vector3 UpVector(0, 1, 0);
	Vector3 Verts[8];

	// near plane verts
	Verts[0] = (Direction * FrustumStartDist) + (UpVector * VertLength) + (LeftVector * HozLength);
	Verts[1] = (Direction * FrustumStartDist) + (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[2] = (Direction * FrustumStartDist) - (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[3] = (Direction * FrustumStartDist) - (UpVector * VertLength) + (LeftVector * HozLength);

	if (XM_PIDIV4 > 0.0f)
	{
		HozLength = FrustumEndDist * tanf(XM_PIDIV4 / 2);
		VertLength = HozLength / fAspectRatio;
	}

	// far plane verts
	Verts[4] = (Direction * FrustumEndDist) + (UpVector * VertLength) + (LeftVector * HozLength);
	Verts[5] = (Direction * FrustumEndDist) + (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[6] = (Direction * FrustumEndDist) - (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[7] = (Direction * FrustumEndDist) - (UpVector * VertLength) + (LeftVector * HozLength);

	VertexPositionColor vertexsCamera[] =
	{
		VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1))
	};
	Matrix world = Matrix::CreateTranslation(0.95f, 5.83f, -14.48f);
	g_objSprite.DrawPrimitiveUP(PRIMITIVE_LINELIST, 24, vertexsCamera, world*viewMatrix* projectionMatrix);

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
