#pragma once
#include "HpD3D9Type.h"

template<typename T>
T Clamp(const T& x, const T& low, const T& high)
{
	return x < low ? low : (x > high ? high : x);
}
class  TrackballCameraController
{

public:
	static TrackballCameraController& GetInstance();
	TrackballCameraController();
	~TrackballCameraController();
	void ProjOrthoParams(float w, float h, float near_plane, float far_plane);
	void ProjParams(float fov, float aspect, float near_plane, float far_plane);


	Matrix View()const;
	Matrix Proj()const;
	Matrix ViewProj()const;
	Matrix InvViewProj()const;

	void Rotate(float offset_x, float offset_y);
	void Move(float offset_x, float offset_y);
	void Zoom(float offset_x, float offset_y);

	void SetTarget(XMVECTOR _target);
	void SetRadius(float _mRadius){
		mRadius = _mRadius;
		UpDate();
	}
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR up);
	XMVECTOR RightVec()
	{
		XMFLOAT3 _RightVec = XMFLOAT3(inv_view_mat_(0, 0), inv_view_mat_(0, 1), inv_view_mat_(0, 2));
		return XMLoadFloat3(&_RightVec);
	}
	XMVECTOR UpVec()
	{
		XMFLOAT3 _UpVec = XMFLOAT3(inv_view_mat_(1, 0), inv_view_mat_(1, 1), inv_view_mat_(1, 2));
		return XMLoadFloat3(&_UpVec);
	}
	XMVECTOR EyePos()
	{
		XMFLOAT3 _EyePos = XMFLOAT3(inv_view_mat_(3, 0), inv_view_mat_(3, 1), inv_view_mat_(3, 2));
		return XMLoadFloat3(&_EyePos);
	}
	Matrix GetSystemAxisMatrix(int nWidth, int nHeight);
public:
	float		rotationScaler_;	// Scaler for rotation
	float		moveScaler_;		// Scaler for movement
	void UpDate();
	float		fov_;
	float		aspect_;
	float		near_plane_;
	float		far_plane_;
	Matrix  proj_mat_;
	Matrix inv_proj_mat_;

	Matrix  view_mat_;
	Matrix inv_view_mat_;

	XMFLOAT3 mEyePos;
	float mRadius;
	float mTheta;
	float mPhi;
	XMVECTOR target;
	XMVECTOR up;

};
#define g_objTrackballCameraController TrackballCameraController::GetInstance()
