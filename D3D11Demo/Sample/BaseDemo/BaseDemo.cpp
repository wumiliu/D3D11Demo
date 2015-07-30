#include "BaseDemo.h"
#include "SwapChain.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "CommonStates.h"
#include "Camera/CameraHelp.h"
#include "CommonStates.h"
#include "Texture/DDSTextureLoader.h"
#include "TrackballCameraController.h"

BaseDemo::BaseDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;


	// Setup the camera   
	Vector3 vecEye(0.95f, 5.83f, -14.48f);
	Vector3 vecAt(0.90f, 5.44f, -13.56f);

	cameraComponent.SetViewParams(vecEye, vecAt);

	
	
}

BaseDemo::~BaseDemo()
{
	SAFE_RELEASE(srv);
}
void BaseDemo::InitResource()
{
	float fAspectRatio = (float)mClientWidth / (float)mClientHeight;
	cameraComponent.SetProjParams(XM_PIDIV4, fAspectRatio, 10.0f, 100.0f);

	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "baseMeshPS.hlsl";
	desc.vecPass.push_back("main");
	desc.vecPass.push_back("mainPick");
	desc.vecPass.push_back("mainRed");
	desc.vecPass.push_back("mainGreen");
	desc.vecPass.push_back("mainBlue");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();
	MeshData meshData;
	GeoGen::CreateSphere(1, 50, 50, meshData);

	bool bBuild = m_MeshModel->BuildBuffers(meshData);
	InitPos(10);
	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"seafloor.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);
}

void BaseDemo::InitPos(int nCount /*= 26*/)
{
	vecModels.clear();
	vecModels.resize(nCount);

	int i;
	float red, green, blue;

	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	for (i = 0; i < nCount; i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		vecModels[i].color = Vector4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		vecModels[i].positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		vecModels[i].positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		vecModels[i].positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
	}
}

void BaseDemo::UpdateScene(float dt)
{

}

void BaseDemo::DrawScene()
{

	SwapChainPtr->Begin();
	ID3D11SamplerState* LinearClamp = g_objStates.LinearClamp();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);
	//m_Material->PSSetShaderResources(0, 1, srv);
	Vector4 color{ 1, 0, 0, 0 };

//	m_Material->VSSetConstantBuffers("MyColor1", &color);
//	m_Material->VSSetConstantBuffers("MyColor", &color);

	m_Material->PSSetShaderResources("shaderTexture", &srv);

	m_Material->PSSetConstantBuffers("MyColor3", &color);
	m_Material->PSSetConstantBuffers("MyColor", &color);
	m_Material->PSSetConstantBuffers("MyColor1", &color);
	m_Material->PSSetConstantBuffers("MyColor2", &color);


	m_deviceContext->OMSetDepthStencilState(g_objStates.DepthDefault(), 1);

	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;

	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;

	CameraHelp help;
	help.ConstructFrustum(mView, mProj);
	int  renderCount = 0;
	for (uint32 i = 0; i < vecModels.size(); ++i)
	{
		ModelInfoType modleInfo = vecModels[i];
		Matrix worldMatrix = Matrix::CreateTranslation(modleInfo.positionX, modleInfo.positionY, modleInfo.positionZ);
		Matrix Rx = XMMatrixRotationX(XM_PI / 3);    // ÈÆXÖáÐý×ª   
		Matrix Ry = XMMatrixRotationY(XM_PI *(::timeGetTime() / 1000.0f / 2));    // ÈÆYÖáÐý×ª   

		worldMatrix = Rx *Ry * worldMatrix;// *Rz;

		// Set the radius of the sphere to 1.0 since this is already known.
		float radius = 1.0f;

		// Check if the sphere model is in the view frustum.
		bool renderModel = help.CheckSphere(modleInfo.positionX, modleInfo.positionY, modleInfo.positionZ, radius);
		if (renderModel)
		{
			renderCount++;
			m_Material->SetMatrix(worldMatrix, mView, mProj);

		//	m_Material->VSSetConstantBuffers("worldMatrix", &worldMatrix);
		//	m_Material->VSSetConstantBuffers("viewMatrix", &mView);
		//	m_Material->VSSetConstantBuffers("worldMatrix", &worldMatrix);
		


			m_MeshModel->render(m_Material.get());
		}
	}
	SwapChainPtr->Flip();
}

LRESULT BaseDemo::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = D3D11App::MsgProc(hwnd, msg, wParam, lParam);
	return result;
}
void BaseDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	D3D11App::OnMouseMove(btnState, x, y);
}


