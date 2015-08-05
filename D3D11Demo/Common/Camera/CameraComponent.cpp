#include "CameraComponent.h"


CameraComponent::CameraComponent()
{
	Vector3 vEyePt(3.0f, 2.0f,1.0f);
	Vector3 vLookatPt(0.0f, 0.0f, 1.0f);
	// Setup the view matrix
	SetViewParams(vEyePt, vLookatPt);
	// Setup the projection matrix
	SetProjParams(XM_PIDIV4, 1.0f, 1.0f, 1000.0f);
}


CameraComponent::~CameraComponent()
{

}

void CameraComponent::SetViewParams(Vector3 vEyePt, Vector3 vLookatPt, Vector3 vEyePtUp)
{
	m_vDefaultEye = m_vEye = vEyePt;
	m_vDefaultLookAt = m_vLookAt = vLookatPt;
	// Calc the view matrix


	Vector3 look = (vLookatPt - vEyePt);
	look.Normalize();
	Vector3 right = (vEyePtUp.Cross(look));

	right.Normalize();
	Vector3 up = look.Cross(right);


	//	把摄像机 平移回原点，在旋转根坐标轴重合
	// Rx  Ry  Rz									1 0 0
	// Ux  Uy Uz    * M == >				0 1 0
	// Lx  Ly   Lz									1 0 1
	//
	// M 为旋转的逆矩阵， 由于旋转矩阵是正交矩阵， 正交矩阵的逆矩阵 就是 它的转置矩阵
	//

	Vector3 NegEyePosition = -vEyePt;

	float x = NegEyePosition.Dot(right);
	float y = NegEyePosition.Dot(up);
	float z = NegEyePosition.Dot(look);
	Matrix ViewMatrix(right, up, look);
	m_mView = ViewMatrix.Transpose();
	m_mView(3, 0) = x;
	m_mView(3, 1) = y;
	m_mView(3, 2) = z;
	m_mView(3, 3) = 1;

	//等价CreateLookAtLh
	Matrix mView = Matrix::CreateLookAtLh(vEyePt, vLookatPt, vEyePtUp);

	Matrix mInvView;
	m_mView.Invert(mInvView);

}


void CameraComponent::SetProjParams(FLOAT fFOV, FLOAT fAspect, FLOAT fNearPlane, FLOAT fFarPlane)
{
	// Set attributes for the projection matrix
	m_fFOV = fFOV;
	m_fAspect = fAspect;
	m_fNearPlane = fNearPlane;
	m_fFarPlane = fFarPlane;
	//r = fAspect
	//	透视投影矩阵的方程
	// r/ (tan(&/2))     0    0     0
	// 0		1/(tan(&/2))    0    0	
	// 0   0      f/f-n                1
	// 0   0     -nf/f-n              0
	//
	//
	m_mProj = XMMatrixPerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane);
	//m_mProj = XMMatrixPerspectiveFovRH(fFOV, fAspect, fNearPlane, fFarPlane);

}

void CameraComponent::Reset()
{

}

void CameraComponent::Rotate(float offset_x, float offset_y)
{

}

void CameraComponent::Move(float offset_x, float offset_y)
{

}

void CameraComponent::Zoom(float offset_x, float offset_y)
{

}

void CameraComponent::GetFrustumMeshElements(std::vector<VertexPositionColor>& vecPos)
{
	//左手坐标系，z范围是0-1. 所以cvv空间是一个长方体
	vecPos.clear();

	float fAspectRatio = m_fAspect;
	float FrustumStartDist = m_fNearPlane;
	float FrustumEndDist = m_fFarPlane;

	float HozLength = 0.0f;
	float VertLength = 0.0f;

	VertLength = FrustumStartDist* tanf(m_fFOV * 0.5f);
	HozLength = VertLength * fAspectRatio;
	Vector3 Direction(0, 0, 1);
	Vector3 LeftVector(1, 0, 0);
	Vector3 UpVector(0, 1, 0);



	Vector3 Verts[8];

	// near plane verts
	Verts[0] = (Direction * FrustumStartDist) + (UpVector * VertLength) + (LeftVector * HozLength);
	Verts[1] = (Direction * FrustumStartDist) + (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[2] = (Direction * FrustumStartDist) - (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[3] = (Direction * FrustumStartDist) - (UpVector * VertLength) + (LeftVector * HozLength);

	VertLength = FrustumEndDist* tanf(m_fFOV * 0.5f);
	HozLength = VertLength * fAspectRatio;

	// far plane verts
	Verts[4] = (Direction * FrustumEndDist) + (UpVector * VertLength) + (LeftVector * HozLength);
	Verts[5] = (Direction * FrustumEndDist) + (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[6] = (Direction * FrustumEndDist) - (UpVector * VertLength) - (LeftVector * HozLength);
	Verts[7] = (Direction * FrustumEndDist) - (UpVector * VertLength) + (LeftVector * HozLength);

	/*for (int i = 0; i < 8;++i)
	{
		Vector3&  vert = Verts[i];
		Vector3 Tmp = vert;
		vert /= Tmp.z;
	}*/
	vecPos.push_back(VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)));

	vecPos.push_back(VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)));

	vecPos.push_back(VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)));

	vecPos.push_back(VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)));

	vecPos.push_back(VertexPositionColor(Verts[0], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[4], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[1], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[5], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[2], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[6], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[3], XMFLOAT4(1, 0, 0, 1)));
	vecPos.push_back(VertexPositionColor(Verts[7], XMFLOAT4(1, 0, 0, 1)));

}
