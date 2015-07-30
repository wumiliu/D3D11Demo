#include "SkyBoxApp.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "CommonStates.h"
#include "Camera/CameraComponent.h"

SkyBoxApp::SkyBoxApp(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}

SkyBoxApp::~SkyBoxApp()
{
	SAFE_RELEASE(srv);
}

void SkyBoxApp::InitResource()
{
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "SkyBox\\baseMeshVS.hlsl";
	desc.pixelShaderPath = "SkyBox\\baseMeshPS.hlsl";
	desc.vecPass.push_back("main");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	MeshData meshData;
	GeoGen::CreateSphere(20, 50, 50, meshData);
	//GeoGen::CreateSphere(100.f, 30, 30, meshData);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	bool bBuild = m_MeshModel->BuildBuffers(meshData);

	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"snowcube1024.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);

}

void SkyBoxApp::UpdateScene(float dt)
{

}

void SkyBoxApp::DrawScene()
{
	SwapChainPtr->Begin();

	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;
	
	mView = g_objTrackballCameraController.GetViewMatrix();
	mProj = g_objTrackballCameraController.GetProjMatrix();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	m_Material->PSSetShaderResources("g_cubeMap", &srv);
	Vector3 cameraPosition = g_objTrackballCameraController.EyePos();
	//worldMatrix = Matrix::CreateTranslation(cameraPosition);
	m_Material->SetMatrix(worldMatrix, mView, mProj);
	
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	m_MeshModel->render(m_Material.get());
	SwapChainPtr->Flip();

}
