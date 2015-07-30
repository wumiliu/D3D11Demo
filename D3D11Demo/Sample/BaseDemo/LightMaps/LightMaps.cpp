#include "LightMaps.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "CommonStates.h"

LightMaps::LightMaps(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


LightMaps::~LightMaps()
{
	SAFE_RELEASE(srv[0]);
	SAFE_RELEASE(srv[1]);

}

void LightMaps::InitResource()
{
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "LightMaps\\LightMapsVS.hlsl";
	desc.pixelShaderPath = "LightMaps\\LightMapsPS.hlsl";
	desc.vecPass.push_back("main");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	desc.pixelShaderPath = "LightMaps\\AlphaMapping.hlsl";
	m_MaterialAlpha = std::make_shared<D3D11RendererMaterial>(desc);


	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	MeshData meshData;
	GeoGen::CreateSphere(1, 50, 50, meshData);
	GeoGen::CreateGrid(3, 3, 3, 5,meshData);

	bool bBuild = m_MeshModel->BuildBuffers(meshData);
	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"stone01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv[0]
		);
	SAFE_RELEASE(pTexture2D);

	ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"light01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv[1]
		);
	SAFE_RELEASE(pTexture2D);


	ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"stone01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srvlAlpha[0]
		);
	SAFE_RELEASE(pTexture2D);

	ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"dirt01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srvlAlpha[1]
		);
	SAFE_RELEASE(pTexture2D);
	ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"alpha01.dds",
		(ID3D11Resource**)&pTexture2D,
		&srvlAlpha[2]
		);
	SAFE_RELEASE(pTexture2D);

}

void LightMaps::UpdateScene(float dt)
{

}

void LightMaps::DrawScene()
{
	SwapChainPtr->Begin();
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	ID3D11SamplerState* LinearClamp = g_objStates.AnisotropicWrap();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);

	m_Material->PSSetShaderResources("shaderTextures", &srv[0]);

	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;

	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	XMMATRIX rz = XMMatrixRotationY(XM_PIDIV4 * mTimer.TotalTime());

	m_Material->SetMatrix(worldMatrix, mView, mProj);
	Vector3 cameraPosition = g_objTrackballCameraController.EyePos();
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	m_MeshModel->render(m_Material.get());

	worldMatrix = Matrix::CreateTranslation(3, 3, 0);
	m_Material->SetMatrix(worldMatrix, mView, mProj);
	m_MaterialAlpha->m_vertexShader.vecConstantBuffer[0] = m_Material->m_vertexShader.vecConstantBuffer[0];
	m_MaterialAlpha->PSSetShaderResources("shaderTextures", &srvlAlpha[0]);
	m_MeshModel->render(m_MaterialAlpha.get());
	SwapChainPtr->Flip();
}
