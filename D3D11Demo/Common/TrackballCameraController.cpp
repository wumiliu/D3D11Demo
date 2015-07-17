#include "TrackballCameraController.h"

TrackballCameraController::TrackballCameraController() :
mEyePos(0.0f, 0.0f, 0.0f), mTheta(0.3f*XM_PI), mPhi(0.4f*XM_PI), mRadius(30.0f)
{
	view_mat_ = XMMatrixIdentity();
	proj_mat_ = XMMatrixIdentity();
	moveScaler_ = 0.00144151133f;
	rotationScaler_ = 0.0099999999978f;
	target = XMLoadFloat3(&XMFLOAT3(0, 0, 0));
	up = XMLoadFloat3(&XMFLOAT3(0, 1, 0));
	UpDate();
}


TrackballCameraController::~TrackballCameraController()
{

}
Matrix TrackballCameraController::View() const
{
	return view_mat_;
}

Matrix TrackballCameraController::Proj() const
{
	return proj_mat_;
}

Matrix TrackballCameraController::ViewProj() const
{
	return view_mat_ * proj_mat_;
}

Matrix TrackballCameraController::InvViewProj() const
{
	return inv_proj_mat_ * inv_view_mat_;
}

Matrix TrackballCameraController::GetSystemAxisMatrix(int nWidth, int nHeight)
{
	float x = (nWidth-70.5f) / nWidth * 2 - 1;

	float y = -(nHeight - 70.5f) / nHeight * 2 + 1;
	XMVECTOR tmpPos = { x, y, 0.1f, 1.0f };
	XMVECTOR origin = XMVector3TransformCoord(tmpPos, g_objTrackballCameraController.InvViewProj());// g_objTrackballCameraController.InvViewProj());

	float x1 = (nWidth - 20.5f) / nWidth * 2 - 1;
	//float x1 = (121.0f) / nWidth * 2 - 1;
	XMVECTOR tmpPos1 = { x1, y, 0.1f, 1.0f };
	XMVECTOR x_dir = XMVector3TransformCoord(tmpPos1, g_objTrackballCameraController.InvViewProj());

	XMVECTOR len = XMVector3Length(XMVectorSubtract(x_dir, origin));
	XMFLOAT3 len_;
	XMStoreFloat3(&len_, len);
	XMMATRIX worldScal = XMMatrixScaling(len_.x, len_.y, len_.z);

	XMStoreFloat3(&len_, origin);
	XMMATRIX trans = XMMatrixTranslation(len_.x, len_.y, len_.z);
	Matrix WVP = worldScal * trans;
	WVP *= ViewProj();
	return WVP;
}

void TrackballCameraController::UpDate()
{
	// Convert Spherical to Cartesian coordinates.
	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);


	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	//XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	pos += target;
	LookAt(pos, target, up);
}

void TrackballCameraController::ProjOrthoParams(float w, float h, float near_plane, float far_plane)
{
	fov_ = 0;
	aspect_ = w / h;
	ProjParams(fov_, aspect_, near_plane, far_plane);
}

void TrackballCameraController::ProjParams(float fov, float aspect, float near_plane, float far_plane)
{
	fov_ = fov;
	aspect_ = aspect;
	near_plane_ = near_plane;
	far_plane_ = far_plane;

	proj_mat_ = XMMatrixPerspectiveFovLH(fov_, aspect_, near_plane_, far_plane_);
	XMVECTOR inv;
	inv_proj_mat_ = XMMatrixInverse(&inv, proj_mat_);

}

void TrackballCameraController::Rotate(float offset_x, float offset_y)
{
	// Make each pixel correspond to a quarter of a degree.
	float dx = XMConvertToRadians(0.25f*static_cast<float>(-offset_x));
	float dy = XMConvertToRadians(0.25f*static_cast<float>(-offset_y));

	// Update angles based on input to orbit camera around box.
	mTheta += dx;
	mPhi += dy;
	mPhi = Clamp(mPhi, 0.1f, XM_PI - 0.1f);
	UpDate();
}

void TrackballCameraController::Move(float offset_x, float offset_y)
{

	/*
	XMFLOAT3 _target;
	XMStoreFloat3(&_target, target);
	_target.x += 2*moveScaler_*offset_x;
	_target.y += 2*moveScaler_*offset_y;
	target = XMLoadFloat3(&_target);

	float x = mRadius*sinf(mPhi)*cosf(mTheta);
	float z = mRadius*sinf(mPhi)*sinf(mTheta);
	float y = mRadius*cosf(mPhi);


	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	pos += target;
	LookAt(pos, target, up);*/

	XMVECTOR offset = RightVec() * (-offset_x * moveScaler_ * 2);
	XMVECTOR pos = EyePos() + offset;
	target += offset;

	offset = up* (offset_y * moveScaler_ * 2);
	pos += offset;
	target += offset;

	LookAt(pos, target, up);


}

void TrackballCameraController::Zoom(float offset_x, float offset_y)
{

	mRadius -= 0.01f*(offset_x - offset_y);

	// Restrict the radius.
	mRadius = Clamp(mRadius, 2.0f, 500.0f);
	UpDate();
}

TrackballCameraController& TrackballCameraController::GetInstance()
{
	static	TrackballCameraController _instance;
	return*(&_instance);
}

void TrackballCameraController::SetTarget(XMVECTOR _target)
{
	target = _target;
	UpDate();
}

void TrackballCameraController::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{


}

void TrackballCameraController::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR _up)
{
	view_mat_ = XMMatrixLookAtLH(pos, target, up);
	XMVECTOR inv;
	inv_view_mat_ = XMMatrixInverse(&inv, view_mat_);
}

