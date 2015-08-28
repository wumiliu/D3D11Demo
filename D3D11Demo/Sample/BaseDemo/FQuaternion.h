#pragma once
#include "HpD3D9Type.h"
#include "UnrealMathUtility.h"
class FEulerAngle;
class FQuaternion
{
public:
	float X;
	float Y;
	float Z;
	float W;
public:
	FQuaternion();
	FQuaternion (const FEulerAngle& Euler);

	//创建用于旋转的四元数q, v必须为单位向量 
	FQuaternion(FVector Axis, float AngleRad);
	void Normalize();

	FQuaternion MakeFromEuler(const FVector& Euler);

	FEulerAngle EulerAngle() const;
public:
	static FMatrix Make(const FQuaternion& Q, const FVector& Origin);
};

