#include "SpecularLight.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "CommonStates.h"

SpecularLight::SpecularLight(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


SpecularLight::~SpecularLight()
{
	SAFE_RELEASE(srv);
}

void SpecularLight::InitResource()
{
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "SpecularLight\\SpecularLightVS.hlsl";
	desc.pixelShaderPath = "SpecularLight\\SpecularLightPS.hlsl";
	desc.vecPass.push_back("main");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	MeshData meshData;
	GeoGen::CreateSphere(1, 50, 50, meshData);
	//GeoGen::CreateBox(1, 1, 1, meshData);

	bool bBuild = m_MeshModel->BuildBuffers(meshData);
	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"seafloor.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);

}

void SpecularLight::UpdateScene(float dt)
{

}

void SpecularLight::DrawScene()
{
	SwapChainPtr->Begin();
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	ID3D11SamplerState* LinearClamp = g_objStates.AnisotropicWrap();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);

	m_Material->PSSetShaderResources("shaderTexture", &srv);




	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;

	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	XMMATRIX rz = XMMatrixRotationY(XM_PIDIV4 * mTimer.TotalTime());


	//	lightDirection = lightDirection.Transform(lightDirection, rz);
	Vector4 ambientColor = { 0.15f, 0.15f, 0.15f, 1.0f };
	m_Material->PSSetConstantBuffers("ambientColor", &ambientColor);
	Vector4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

	m_Material->PSSetConstantBuffers("diffuseColor", &diffuseColor);

	Vector3 lightDirection = { 0.0f, 0.0f, 1.0f };
	m_Material->PSSetConstantBuffers("lightDirection", &lightDirection);
	float specularPower = 32.0f;
	m_Material->PSSetConstantBuffers("specularPower", &specularPower);
	Vector4 specularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_Material->PSSetConstantBuffers("specularColor", &specularColor);


	m_Material->SetMatrix(rz, mView, mProj);
	Vector3 cameraPosition = g_objTrackballCameraController.EyePos();
	m_Material->VSSetConstantBuffers("cameraPosition", &cameraPosition);
	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);
	m_MeshModel->render(m_Material.get());
	worldMatrix = Matrix::CreateTranslation(lightDirection) * Matrix::CreateScale(0.2f, 0.2f, 0.2f);
//	m_Material->SetMatrix(worldMatrix, mView, mProj);
//	m_MeshModel->render(m_Material.get());
	SwapChainPtr->Flip();
}
