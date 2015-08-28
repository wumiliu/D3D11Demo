#include "Rotator.h"
#include "UnrealMathUtility.h"

#include "UnrealMathDirectX.h"
#include "UnrealMathVectorCommon.h"
const FRotator FRotator::ZeroRotator(0.f, 0.f, 0.f);

FORCEINLINE FRotator::FRotator(float InPitch, float InYaw, float InRoll)
	: Pitch(InPitch), Yaw(InYaw), Roll(InRoll)
{

}

FRotator::FRotator(const FQuat& Quat)
{
	*this = Quat.Rotator();
}

FRotator::FRotator(const FRotator& R)
	: Pitch(R.Pitch), Yaw(R.Yaw), Roll(R.Roll)
{

}

FQuat FRotator::Quaternion() const
{
	VectorRegister Angles = MakeVectorRegister(Roll, Pitch, Yaw, 0.0f);
	VectorRegister HalfAngles = VectorMultiply(Angles, GlobalVectorConstants::DEG_TO_RAD_HALF);

	union { VectorRegister v; float f[4]; } SinAngles, CosAngles;
	VectorSinCos(&SinAngles.v, &CosAngles.v, &HalfAngles);

	const float	SR = SinAngles.f[0];
	const float	SP = SinAngles.f[1];
	const float	SY = SinAngles.f[2];
	const float	CR = CosAngles.f[0];
	const float	CP = CosAngles.f[1];
	const float	CY = CosAngles.f[2];

	FQuat RotationQuat;
	RotationQuat.W = CR*CP*CY + SR*SP*SY;
	RotationQuat.X = CR*SP*SY - SR*CP*CY;
	RotationQuat.Y = -CR*SP*CY - SR*CP*SY;
	RotationQuat.Z = CR*CP*SY - SR*SP*CY;

	return RotationQuat;;
}

FVector FRotator::Euler() const
{
	return FVector(Roll, Pitch, Yaw);
}

void FRotator::Normalize()
{
#if PLATFORM_ENABLE_VECTORINTRINSICS
	VectorRegister VRotator = VectorLoadFloat3_W0(this);
	VRotator = VectorNormalizeRotator(VRotator);
	VectorStoreFloat3(VRotator, this);
#else
	Pitch = NormalizeAxis(Pitch);
	Yaw = NormalizeAxis(Yaw);
	Roll = NormalizeAxis(Roll);
#endif
}

float FRotator::NormalizeAxis(float Angle)
{
	// returns Angle in the range [0,360)
	Angle = ClampAxis(Angle);

	if (Angle > 180.f)
	{
		// shift to (-180,180]
		Angle -= 360.f;
	}

	return Angle;
}

float FRotator::ClampAxis(float Angle)
{
	// returns Angle in the range (-360,360)
	Angle = FMath::Fmod(Angle, 360.f);

	if (Angle < 0.f)
	{
		// shift to [0,360) range
		Angle += 360.f;
	}

	return Angle;
}

FRotator FRotator::MakeFromEuler(const FVector& Euler)
{
	return FRotator(Euler.y, Euler.z, Euler.x);
}



FRotator FRotator::UEToDX()
{
	FRotator a(*this);
	//Roll = -Pitch
	//Pitch = -Yaw
	//Yaw = Roll
	//从DX--UE
	a.Pitch = -Roll;
	a.Yaw = -Pitch;
	a.Roll = Yaw;
	return a;
}

FRotator FRotator::UEToDX(const FRotator& R)
{
	FRotator a;
	//Roll = -Pitch
	//Pitch = -Yaw
	//Yaw = Roll
	//从DX--UE
	a.Pitch = -R.Roll;
	a.Yaw = -R.Pitch;
	a.Roll = R.Yaw;
	return a;
}

FRotator FRotator::DXToUE()
{
	FRotator a;
	//Roll = -Pitch
	//Pitch = -Yaw
	//Yaw = Roll
	//从DX--UE
	a.Roll = -Pitch;
	a.Pitch = -Yaw;
	a.Yaw = Roll;
	return a;
}

FRotator FRotator::DXToUE(const FRotator& R)
{
	FRotator a;
	//Roll = -Pitch
	//Pitch = -Yaw
	//Yaw = Roll
	//从DX--UE
	a.Roll = -R.Pitch;
	a.Pitch = -R.Yaw;
	a.Yaw = R.Roll;
	return a;
}
