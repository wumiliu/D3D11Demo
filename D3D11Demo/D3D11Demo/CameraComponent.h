#pragma once
#include "CameraTypes.h"
#include "HpD3D9Type.h"

class CameraComponent
{
public:
	CameraComponent();
	~CameraComponent();
	void LookAt(Vector3 pos, Vector3 target, Vector3 _up);


	/** The horizontal field of view (in degrees) in perspective mode (ignored in Orthographic mode) */
	float FieldOfView;

	/** The desired width (in world units) of the orthographic view (ignored in Perspective mode) */
	float OrthoWidth;

	// Aspect Ratio (Width/Height)
	float AspectRatio;

	// The type of camera
	ECameraProjectionMode::Type ProjectionMode;

	Matrix ViewMatrix;

	/** UE4 projection matrix projects such that clip space Z=1 is the near plane, and Z=0 is the infinite far plane. */
	Matrix ProjectionMatrix;
};

