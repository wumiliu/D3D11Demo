#include "QuatDemo.h"
#include "SwapChain.h"
#include "CommonStates.h"
#include "D3D11RendererMesh.h"
#include "D3D11RendererMaterial.h"
#include "TrackballCameraController.h"
#include "Texture/TextureMgr.h"
#include "../DynamicPrimitive.h"
#include "../UnrealMath.h"


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
	XMVECTOR q2 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f), XMConvertToRadians(90.0f));
	XMVECTOR q3 = XMQuaternionRotationAxis(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMConvertToRadians(70.0f));

	FRotator myR0(0.0f, 0.0f, 0.0f);
	FQuat myQ0(myR0);
	q0 = { myQ0.X, myQ0.Y, myQ0.Z, myQ0.W };


	//z
	FRotator myR1(0.0f, 90.0f, 0.0f);
	FQuat myQ1(myR1);
	q1 = { myQ1.X, myQ1.Y, myQ1.Z, myQ1.W };

	//x
	FRotator myR2(-30.0f, 90.0f, 0.0f);
	FQuat myQ2(myR2);
	q2= { myQ2.X, myQ2.Y, myQ2.Z, myQ2.W };

	FRotator myR3(45.0f, 30.0f, 20.0f);
	FQuat myQ3(myR3);
	q3 = { myQ3.X, myQ3.Y, myQ3.Z, myQ3.W };


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
	mAnimTimePos += dt*0.5f;
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
		XMVECTOR qTmp = XMQuaternionRotationAxis(XMVectorSet(0.0f,0.0f, 1.0f, 1.0f), XMConvertToRadians(30.0f));
		Quaternion q2(qTmp);

		XMVECTOR S = XMLoadFloat3(&XMFLOAT3(1.0f, 1.0f, 1.0f));
		XMVECTOR P = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 5.0f));
		XMVECTOR Q = XMLoadFloat4(&q2);

		XMVECTOR zero = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMFLOAT4X4 M;
		mWorld = XMMatrixAffineTransformation(S, zero, Q, P);
	//	Matrix mWorldTmp = Matrix::CreateFromQuaternion(q2);
	//	mWorldTmp =mWorldTmp * Matrix::CreateTranslation(0,0,5);

		FQuat q(q2.x, q2.y, q2.z, q2.w);
		

		FRotator rotator = q.Rotator();
		FQuat q4 = rotator.Quaternion();
		FQuat q3(rotator);
		Vector3 Euler = q.Rotator().Euler();
		Vector3 vAxis;
		float fAngle;
		q.ToAxisAndAngle(vAxis, fAngle);

		Quaternion q2Tmp = Quaternion::CreateFromAxisAngle(vAxis,fAngle);
		FQuat q11(Vector3(1, 1, 1), XMConvertToRadians(146));
		q11.Normalize(SMALL_NUMBER);
		std::string strText = q.ToString();
		Vector3 vAxisX = q.GetAxisX();
		Vector3 vAxisY = q.GetAxisY();
		Vector3 vAxisZ = q.GetAxisZ();

	//	fAngle = XMConvertToDegrees(fAngle);
		FQuat q1;
		m_Pos = q.RotateVector(m_Pos);
	
		m_Material->SetMatrix(mWorld, mView, mProj);
		m_deviceContext->RSSetState(g_objStates.Wireframe());
	//	m_MeshModel->render(m_Material.get(), 1);
		mWorld = Matrix::Identity;
		mWorld *= XMMatrixRotationY(vAxis.y *fAngle);
		mWorld *= XMMatrixRotationZ(vAxis.z*fAngle);
		mWorld *= XMMatrixRotationX(vAxis.x *fAngle);

	
		mWorld *=Matrix::CreateTranslation(0, 0, 5);
		m_Material->SetMatrix(mWorld, mView, mProj);
	//	m_MeshModel->render(m_Material.get(), 0);


		//四元数转换为矩阵（等价） 四元数转换为欧拉角 和欧拉角转换为四元数
		Matrix mWorldTmp = Matrix::CreateFromQuaternion(q2);
		Matrix mWorldTmp1 = FQuat::Make(q4, Vector3::Zero);

		const FVector XAxis = mWorldTmp1.Right();
		const FVector YAxis = mWorldTmp1.Up();
		const FVector ZAxis = mWorldTmp1.Backward();

		//y z x
		FRotator myR(0.0f, 20.0f, 20.0f);
		FQuat myQ(myR);
		FRotator myRTmp = myQ.Rotator();
		FVector EulerTmp = myRTmp.Euler();
		mWorldTmp1 = FQuat::Make(myQ, Vector3::Zero);
		mWorldTmp1 = Matrix::CreateScale(1.0f, 1.0f, 1.0f)*mWorldTmp1 * Matrix::CreateTranslation(0, 0, 0);
		m_Material->SetMatrix(mWorldTmp1, mView, mProj);
		m_MeshModel->render(m_Material.get(), 0);

		mWorld = Matrix::CreateScale(0.98f, 0.98f, 0.98f);
		mWorld = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
		//虚幻转换为DX 坐标系
		mWorld *= Matrix::CreateRotationX((XMConvertToRadians(-myR.Roll)));
		mWorld *= Matrix::CreateRotationY((XMConvertToRadians(-myR.Pitch)));
		mWorld *= Matrix::CreateRotationZ((XMConvertToRadians(myR.Yaw)));
		m_Material->SetMatrix(mWorld, mView, mProj);
		m_MeshModel->render(m_Material.get(), 1);
		//

		float mX = XMConvertToRadians(-myR.Roll);
		float mY = XMConvertToRadians(-myR.Pitch);
		float mZ = XMConvertToRadians(myR.Yaw);
		FXMVECTOR Angles = { mZ, mY, mX };

		Matrix mWorld3 = XMMatrixRotationRollPitchYawFromVector(Angles);

		Matrix mWorld1 = Matrix::CreateScale(0.98f, 0.98f, 0.98f);
		mWorld1 = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
		Quaternion q5 = Quaternion::CreateFromRotationMatrix(mWorld);
		Quaternion q8 = Quaternion::CreateFromYawPitchRoll(mY, mX, mZ);

		mWorld3 = Matrix::CreateFromQuaternion(q8);

		FQuat q70(q5.x, q5.y, q5.z, q5.w);
		Quaternion q7(q5.x, q5.y, q5.z, q5.w);
		FRotator r7 = q70.Rotator();


	//	q5.Normalize();
		Matrix mWorld2 = Matrix::CreateFromYawPitchRoll(XMConvertToRadians(myR.Pitch), XMConvertToRadians(-myR.Roll), XMConvertToRadians(-myR.Yaw));
	

		
		mWorld1 *= Matrix::CreateFromQuaternion(q5);
		FQuat q6(q5.x, q5.y, q5.z, q5.w);

		//mWorld1 = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
		//mWorld1 *= Matrix::CreateFromYawPitchRoll(XMConvertToRadians(myR.Pitch), XMConvertToRadians(-myR.Roll), XMConvertToRadians(-myR.Yaw));
		//mWorld *= Matrix::CreateRotationX((XMConvertToRadians(-myR.Roll)));
		//mWorld *= Matrix::CreateRotationY((XMConvertToRadians(-myR.Pitch)));
		//mWorld *= Matrix::CreateRotationZ((XMConvertToRadians(myR.Yaw)));
		m_Material->SetMatrix(mWorld3, mView, mProj);
		m_MeshModel->render(m_Material.get(), 1);

	}
	else
	{
		m_MeshModel->render(m_Material.get(), 1);
	}
	m_deviceContext->RSSetState(g_objStates.Wireframe());
	m_Material->SetMatrix(Matrix::Identity, mView, mProj);
//	m_FloorModel->render(m_Material.get(), 1);
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

