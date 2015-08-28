#pragma once
#include "HpD3D9Type.h"

class FQuaternion;
//DX 左手坐标系 left x  up y look  z
//pitch()：俯仰，将物体绕X轴旋转（localRotationX）
//yaw()：航向，将物体绕Y轴旋转（localRotationY）
//roll()：横滚，将物体绕Z轴旋转（localRotationZ）

//DX坐标系(x,y,z) 内部计算用的是UE 坐标系

class FEulerAngle
{
public:
	float Pitch;	 //俯仰,将物体绕X轴旋转（localRotationX）
	float Yaw;		//航向,将物体绕Y轴旋转（localRotationY）
	float Roll;		//横滚,将物体绕Z轴旋转（localRotationZ）

	static  const FEulerAngle ZeroRotator;
public:
	FEulerAngle(){};
	FEulerAngle(const FEulerAngle& Euler);

	FEulerAngle(float InPitch, float InYaw, float InRoll);
	FQuaternion Quaternion() const;
	void Normalize();

	FEulerAngle UEToDX();
	FEulerAngle DXToUE();

public:
	static FEulerAngle MakeFromEuler(const FVector& Euler);
};

