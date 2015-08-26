#include "QuatDemo.h"
#include "SwapChain.h"
#include "CommonStates.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "Texture/TextureMgr.h"
#include "../DynamicPrimitive.h"


QuatDemo::QuatDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/) : D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
	m_Pos = { 0, 1, 0 };

	
}

QuatDemo::~QuatDemo()
{

}
void QuatDemo::InitResource()
{
	RendererMaterialDesc desc;
	desc.vertexShaderPath = "baseMeshVS.hlsl";
	desc.pixelShaderPath = "baseMeshPS.hlsl";

	desc.vecPass.push_back("mainTex");
	desc.vecPass.push_back("mainRed");
	m_Material = std::make_shared<D3D11RendererMaterial>(desc);

	m_MeshModel = std::make_shared<D3D11RendererMesh>();

	GeometryGenerator::MeshData box;
	GeometryGenerator::MeshData grid;
	GeometryGenerator::MeshData sphere;
	GeometryGenerator::MeshData cylinder;

	GeometryGenerator geoGen;
	geoGen.CreateBox(5.0f, 3.0f, 1.0f, box);
	geoGen.CreateGrid(30.0f, 30.0f, 60, 60, grid);
	geoGen.CreateSphere(1.0f, 30, 30, sphere);
	geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);


	bool bBuild = m_MeshModel->BuildBuffers(box);
	m_FloorModel = std::make_shared<D3D11RendererMesh>();
	m_FloorModel->BuildBuffers(grid);

	//
	// Define the animation keyframes
	//

	XMVECTOR q0 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(0.0));
	XMVECTOR q1 = XMQuaternionRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
	XMVECTOR q2 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
	XMVECTOR q3 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(70.0f));
	int nCount = 3;
	mSkullAnimation.Keyframes.resize(nCount);
	mSkullAnimation.Keyframes[0].TimePos = 0.0f;
	mSkullAnimation.Keyframes[0].Translation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSkullAnimation.Keyframes[0].Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4(&mSkullAnimation.Keyframes[0].RotationQuat, q0);

	mSkullAnimation.Keyframes[1].TimePos = 2.0f;
	mSkullAnimation.Keyframes[1].Translation = XMFLOAT3(0.0f, 0.0f, 5.0f);
	mSkullAnimation.Keyframes[1].Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4(&mSkullAnimation.Keyframes[1].RotationQuat, q1);


	mSkullAnimation.Keyframes[2].TimePos = 4.0f;
	mSkullAnimation.Keyframes[2].Translation = XMFLOAT3(0.0f, 0.0f, 5.0f);
	mSkullAnimation.Keyframes[2].Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMStoreFloat4(&mSkullAnimation.Keyframes[2].RotationQuat, q2);
	return;

	mSkullAnimation.Keyframes[3].TimePos = 6.0f;
	mSkullAnimation.Keyframes[3].Translation = XMFLOAT3(0.0f, 1.0f, -10.0f);
	mSkullAnimation.Keyframes[3].Scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
	XMStoreFloat4(&mSkullAnimation.Keyframes[3].RotationQuat, q3);

	mSkullAnimation.Keyframes[4].TimePos = 8.0f;
	mSkullAnimation.Keyframes[4].Translation = XMFLOAT3(-7.0f, 0.0f, 0.0f);
	mSkullAnimation.Keyframes[4].Scale = XMFLOAT3(0.25f, 0.25f, 0.25f);
	XMStoreFloat4(&mSkullAnimation.Keyframes[4].RotationQuat, q0);
	mAnimTimePos = 0.0f;

}


void QuatDemo::UpdateScene(float dt)
{
	dt /= 1000.0f;
	static float fRotate = 0.0f;
	fRotate += 1;
	m_Rotate = Matrix::CreateFromYawPitchRoll(fRotate / 180.0f*XM_PI, 0, 0);
	mAnimTimePos += dt;
	bool bLoop = true;
	if (mAnimTimePos >= mSkullAnimation.GetEndTime() && bLoop)
	{
		// Loop animation back to beginning.
		mAnimTimePos = 0.0f;// mSkullAnimation.GetEndTime();
	}

	mSkullAnimation.Interpolate(mAnimTimePos, mSkullWorld);
}

void QuatDemo::DrawScene()
{
	SwapChainPtr->Begin();
	ID3D11SamplerState* LinearClamp = g_objStates.LinearWrap();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);
	Matrix mWorld;
	Matrix mView;
	Matrix mProj;
	Matrix mWorldViewProjection;

	mView = g_objTrackballCameraController.View();
	mProj = g_objTrackballCameraController.Proj();
	mWorldViewProjection = mView * mProj;

	mWorld = m_Rotate* Matrix::CreateTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	const float *pArray = (const float *)&mSkullWorld;
	mWorld = Matrix::CreateRotationY(0.5f*XM_PI) * Matrix::CreateRotationX(0.5f*XM_PI);// *Matrix::CreateRotationZ(0.5f*XM_PI);
	mWorld = (Matrix)pArray;
	m_Material->SetMatrix(mWorld, mView, mProj);
	m_deviceContext->RSSetState(g_objStates.CullNone());
	ID3D11ShaderResourceView* pSrv = g_objTextureMgr.CreateTextureDDs("ice.dds");
	if (pSrv)
	{
		m_Material->PSSetShaderResources("shaderTexture", &pSrv);
		m_MeshModel->render(m_Material.get());

		m_Pos = { 0, 0, 0 };
		mWorld = Matrix::CreateTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
		m_Material->SetMatrix(mWorld, mView, mProj);
		m_MeshModel->render(m_Material.get());

	}
	else
	{
		m_MeshModel->render(m_Material.get(), 1);
	}
	m_deviceContext->RSSetState(g_objStates.Wireframe());
	m_Material->SetMatrix(Matrix::Identity, mView, mProj);
	m_FloorModel->render(m_Material.get(), 1);
	RenderSystemAxis();
	SwapChainPtr->Flip();
}

void QuatDemo::RenderSystemAxis()
{
	Matrix mWorld = Matrix::CreateScale(6,6,6)* Matrix::CreateTranslation(m_Pos.x, m_Pos.y, m_Pos.z);
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();
	VertexPositionColor vertexs[] =
	{
		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 0, 0, 1)),
		VertexPositionColor(XMFLOAT3(1, 0, 0), XMFLOAT4(1, 0, 0, 1)),

		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 1, 0, 1)),
		VertexPositionColor(XMFLOAT3(0, 1, 0), XMFLOAT4(0, 1, 0, 1)),

		VertexPositionColor(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 1, 1)),
		VertexPositionColor(XMFLOAT3(0, 0, 1), XMFLOAT4(0, 0, 1, 1))
	};
	g_objSprite.DrawPrimitiveUP(PRIMITIVE_LINELIST, 6, vertexs, mWorld*viewMatrix* projectionMatrix);
}

