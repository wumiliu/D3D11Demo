#include "ReflectionApp.h"
#include "SwapChain.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "Texture/DDSTextureLoader.h"
#include "TrackballCameraController.h"
#include "Texture/DX11RTTexture.h"

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

	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;
	Matrix worldMatrix = Matrix::CreateTranslation(0, 0, 0);
	XMMATRIX rz = XMMatrixRotationY(XM_PIDIV4 * mTimer.TotalTime());
	m_Material->SetMatrix(rz, mView, mProj);
	m_MeshModel->render(m_Material.get());

	SwapChainPtr->Flip();
}

void ReflectionApp::RenderSceneToTexture()
{
	if (DX11RTTexturePtr == NULL)
	{
		return;
	}
	DX11RTTexturePtr->Begin();
}
