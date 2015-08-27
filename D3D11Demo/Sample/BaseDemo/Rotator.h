#pragma once
#include "HpD3D9Type.h"
#include "Quat.h"

class FQuat;
//pitch()：俯仰，将物体绕X轴旋转（localRotationX）


//yaw()：航向，将物体绕Y轴旋转（localRotationY）

//roll()：横滚，将物体绕Z轴旋转（localRotationZ）


//这边是UE 的坐标系 （pitch : y, yaw: z,roll: x）
struct FRotator
{
public:

	/** Looking up and down (0=Straight Ahead, +Up, -Down). */
	float Pitch;

	/** Rotating around (running in circles), 0=East, +North, -South. */
	float Yaw;

	/** Rotation about axis of screen, 0=Straight, +Clockwise, -CCW. */
	float Roll;

public:

	/** A rotator of zero degrees on each axis. */
	static  const FRotator ZeroRotator;

public:

	/**
	* Default constructor (no initialization).
	*/
	FORCEINLINE FRotator() { };


	/**
	* Constructor.
	*
	* @param InPitch Pitch in degrees.
	* @param InYaw Yaw in degrees.
	* @param InRoll Roll in degrees.
	*/
	FORCEINLINE FRotator(float InPitch, float InYaw, float InRoll);
	/**
	* Constructor.
	*
	* @param Quat Quaternion used to specify rotation.
	*/
	explicit  FRotator(const FQuat& Quat);

public:
	/**
	* Get Rotation as a quaternion.
	*
	* @return Rotation as a quaternion.
	*/
	 FQuat Quaternion() const;

	/**
	* Convert a Rotator into floating-point Euler angles (in degrees). Rotator now stored in degrees.
	*
	* @return Rotation as a Euler angle vector.
	*/
	 FVector Euler() const;

	 void Normalize();

public:
	static float ClampAxis(float Angle);

	static float NormalizeAxis(float Angle);
	static FRotator MakeFromEuler(const FVector& Euler);
};


