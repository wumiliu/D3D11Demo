#include "ReflectionApp.h"
#include "SwapChain.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "Texture/DDSTextureLoader.h"
#include "TrackballCameraController.h"
#include "Texture/DX11RTTexture.h"
#include "CommonStates.h"
#include "Camera/CameraComponent.h"

ReflectionApp::ReflectionApp(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	:D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


ReflectionApp::~ReflectionApp()
{
	SAFE_RELEASE(srv);
}

void ReflectionApp::InitResource()
{
	DX11RTTexturePtr = std::make_shared<DX11RTTexture>();
	DX11RTTexturePtr->Create(mClientWidth, mClientHeight);

	RendererMaterialDesc desc;
	desc.vertexShaderPath = "Reflection\\ReflectionVS.hlsl";
	desc.pixelShaderPath = "Reflection\\ReflectionPS.hlsl";
	desc.vecPass.push_back("main");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	m_FloorModel = std::make_shared<D3D11RendererMesh>();
	MeshData meshData;
	GeoGen::CreateSphere(1, 50, 50, meshData);
	GeoGen::CreateGrid(100, 100, 1, 1, meshData);
	m_FloorModel->BuildBuffers(meshData);
	GeoGen::CreateBox(1, 1, 1, meshData);

	bool bBuild = m_MeshModel->BuildBuffers(meshData);
	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"seafloor.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);

	 ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"blue01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv1
		);

	SAFE_RELEASE(pTexture2D);

	// Setup the camera   
	Vector3 vecEye(0.0f, 0.0f, -10.0f);
	Vector3 vecAt(0.0f, 0.0f, 0.0f);

	cameraComponent->SetViewParams(vecEye, vecAt);
	float fAspectRatio = (float)mClientWidth / (float)mClientHeight;
	cameraComponent->SetProjParams(XM_PIDIV4, fAspectRatio, 1.0f, 1010.0f);

}

void ReflectionApp::UpdateScene(float dt)
{

}

void ReflectionApp::DrawScene()
{
	SwapChainPtr->Begin();
	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;

	mView = cameraComponent->GetViewMatrix();
	mProj = cameraComponent->GetProjMatrix();
	mView = g_objTrackballCameraController.GetViewMatrix();
	mProj = g_objTrackballCameraController.GetProjMatrix();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix;
	XMMATRIX rz = XMMatrixRotationY(XM_PIDIV4 * mTimer.TotalTime());
	m_Material->PSSetShaderResources("shaderTexture", &srv);
	m_Material->SetMatrix(worldMatrix, mView, mProj);
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	m_MeshModel->render(m_Material.get());

	m_Material->PSSetShaderResources("shaderTexture", &srv1);
	
	worldMatrix = Matrix::CreateTranslation(0, -1.5f, 0);
	m_Material->SetMatrix(worldMatrix, mView, mProj);

	m_FloorModel->render(m_Material.get());
	SwapChainPtr->Flip();
}

void ReflectionApp::RenderSceneToTexture()
{
	if (DX11RTTexturePtr == NULL)
	{
		return;
	}
	// Use the camera to calculate the reflection matrix.
	//m_Camera->RenderReflection(-1.5f);

	DX11RTTexturePtr->Begin();
}
