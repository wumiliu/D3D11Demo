#pragma once
#include "HpD3D9Type.h"
#include "UnrealMathUtility.h"

#include "Rotator.h"
#define  WITH_DIRECTXMATH 1

#ifndef FORCEINLINE
#if (_MSC_VER >= 1200)
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE __inline
#endif
#endif



#define CONSTEXPR


// q=w+xi+yj+zk
//其中 w,x,y,z是实数。同时，有:
//i*i = -1
//j*j = -1
//k*k = -1
//四元数也可以表示为：
//q = [w, v]
//http://blog.csdn.net/wangjiannuaa/article/details/8952196
//////////////////////////////////////////////////////////////////////////
// 通俗的讲，一个四元数（Quaternion）描述了一个旋转轴和一个旋转角度。这个旋转轴和这个角度可以通过 Quaternion::ToAngleAxis转换得到。
//当然也可以随意指定一个角度一个旋转轴来构造一个Quaternion。
//这个角度是相对于单位四元数而言的，也可以说是相对于物体的初始方向而言的。
//当用一个四元数乘以一个向量时，实际上就是让该向量围绕着这个四元数所描述的旋转轴，转动这个四元数所描述的角度而得到的向量。
//////////////////////////////////////////////////////////////////////////
class FQuat
{

public:
	//Identity quaternion
	static  const FQuat Identity;
public:

	/** Holds the quaternion's X-component. */
	float X;

	/** Holds the quaternion's Y-component. */
	float Y;

	/** Holds the quaternion's Z-component. */
	float Z;

	/** Holds the quaternion's W-component. */
	float W;
public:
	FORCEINLINE FQuat() { }
	FORCEINLINE FQuat(float InX, float InY, float InZ, float InW);
	FORCEINLINE FQuat(const FQuat& Q);

	//创建用于旋转的四元数q, v必须为单位向量 
	FQuat(FVector Axis, float AngleRad);

	explicit FQuat(const FRotator& R);
	explicit FQuat(const FMatrix& M);
	/**
	* get the axis and angle of rotation of this quaternion
	*
	* @param Axis{out] vector of axis of the quaternion
	* @param Angle{out] angle of the quaternion
	* @warning : assumes normalized quaternions.
	*/
	void ToAxisAndAngle(FVector& Axis, float& Angle) const;

	/**
	* Rotate a vector by this quaternion.
	*
	* @param V the vector to be rotated
	* @return vector after rotation
	*/
	FVector RotateVector(FVector V) const;

	/** @return Vector of the axis of the quaternion */
	FORCEINLINE FVector GetRotationAxis() const;


	/** Get X Rotation Axis. */
	FORCEINLINE FVector GetAxisX() const;

	/** Get Y Rotation Axis. */
	FORCEINLINE FVector GetAxisY() const;

	/** Get Z Rotation Axis. */
	FORCEINLINE FVector GetAxisZ() const;

	FRotator Rotator() const;

	FQuat MakeFromEuler(const FVector& Euler);

	/** Convert a Quaternion into floating-point Euler angles (in degrees). */
	FVector Euler() const;

	bool ContainsNaN() const;
	std::string ToString() const;

	FORCEINLINE void DiagnosticCheckNaN() const
	{
		if (!ContainsNaN())
		{
			printf("FQuat contains NaN : %s\n",ToString().c_str());
		}
	}
	void Normalize(float Tolerance = SMALL_NUMBER);

public:
	static FMatrix Make(const FQuat& Q, const FVector& Origin);

};



FORCEINLINE FQuat::FQuat(float InX, float InY, float InZ, float InW)
	: X(InX)
	, Y(InY)
	, Z(InZ)
	, W(InW)
{

}

FORCEINLINE FQuat::FQuat(const FQuat& Q)
	: X(Q.X)
	, Y(Q.Y)
	, Z(Q.Z)
	, W(Q.W)
{

}

FORCEINLINE FVector FQuat::GetRotationAxis() const
{
	// Ensure we never try to sqrt a neg number
	//W = cos(a)
	//sin(a) = 1- W*W
	//X = s * Axis.x;  -- > Axis.x = X/s
	//Y = s * Axis.y;
	//Z = s * Axis.z;
	const float S = FMath::Sqrt(FMath::Max(1.f - (W * W), 0.f));

	if (S >= 0.0001f)
	{
		return FVector(X / S, Y / S, Z / S);
	}

	return FVector(1.f, 0.f, 0.f);
}


FORCEINLINE FVector FQuat::GetAxisX() const
{
	return RotateVector(FVector(1.f, 0.f, 0.f));
}

FORCEINLINE FVector FQuat::GetAxisY() const
{
	return RotateVector(FVector(0.f, 1.f, 0.f));
}

FORCEINLINE FVector FQuat::GetAxisZ() const
{
	return RotateVector(FVector(0.f, 0.f, 1.f));
}
