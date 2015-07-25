#include "DiffuseLight.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"

DiffuseLight::DiffuseLight(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
}


DiffuseLight::~DiffuseLight()
{

}

void DiffuseLight::InitResource()
{
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "DiffuseLightPS.hlsl";
	desc.vecPass.push_back("main");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	MeshData meshData;
	GeoGen::CreateSphere(1, 50, 50, meshData);

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

void DiffuseLight::UpdateScene(float dt)
{

}

void DiffuseLight::DrawScene()
{
	SwapChainPtr->Begin();

	m_Material->PSSetShaderResources("shaderTexture", srv);
	Vector4 diffuseColor = { 1.0f, 0.0f, 1.0f, 1.0f };
	m_Material->PSSetConstantBuffers("diffuseColor", &diffuseColor);



	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;
	
	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	XMMATRIX rz = XMMatrixRotationY(XM_PIDIV4 * mTimer.TotalTime());
	Vector3 lightDirection = { -3.0f, 0.0f, 0.0f };

	lightDirection = lightDirection.Transform(lightDirection, rz);
	m_Material->PSSetConstantBuffers("lightDirection", &lightDirection);
	m_Material->SetMatrix(worldMatrix, mView, mProj);
	m_MeshModel->render(m_Material.get());
	SwapChainPtr->Flip();
}
