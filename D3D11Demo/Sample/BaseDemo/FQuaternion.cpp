#include "FQuaternion.h"
#include "FEulerAngle.h"

FQuaternion::FQuaternion()
{
}

FQuaternion::FQuaternion(FVector Axis, float AngleRad)
{
	const float half_a = 0.5f * AngleRad;
	float s, c;
	FMath::SinCos(&s, &c, half_a);

	X = s * Axis.x;
	Y = s * Axis.y;
	Z = s * Axis.z;
	W = c;
	Normalize();
}

FQuaternion::FQuaternion(const FEulerAngle& Euler)
{
	*this = Euler.Quaternion();
}

void FQuaternion::Normalize()
{
	VectorRegister VRotator = VectorLoadFloat3_W0(this);
	VRotator = VectorNormalizeRotator(VRotator);
	VectorStoreFloat3(VRotator, this);
}

FQuaternion FQuaternion::MakeFromEuler(const FVector& Euler)
{
	return FEulerAngle::MakeFromEuler(Euler).Quaternion();
}

FMatrix FQuaternion::Make(const FQuaternion& Q, const FVector& Origin)
{
	FMatrix M;

	const float x2 = Q.X + Q.X;  const float y2 = Q.Y + Q.Y;  const float z2 = Q.Z + Q.Z;
	const float xx = Q.X * x2;   const float xy = Q.X * y2;   const float xz = Q.X * z2;
	const float yy = Q.Y * y2;   const float yz = Q.Y * z2;   const float zz = Q.Z * z2;
	const float wx = Q.W * x2;   const float wy = Q.W * y2;   const float wz = Q.W * z2;


	M.m[0][0] = 1.0f - (yy + zz);	M.m[1][0] = xy - wz;				M.m[2][0] = xz + wy;			M.m[3][0] = Origin.x;
	M.m[0][1] = xy + wz;			M.m[1][1] = 1.0f - (xx + zz);		M.m[2][1] = yz - wx;			M.m[3][1] = Origin.y;
	M.m[0][2] = xz - wy;			M.m[1][2] = yz + wx;				M.m[2][2] = 1.0f - (xx + yy);	M.m[3][2] = Origin.z;
	M.m[0][3] = 0.0f;				M.m[1][3] = 0.0f;					M.m[2][3] = 0.0f;				M.m[3][3] = 1.0f;
	return M;
}

FEulerAngle FQuaternion::EulerAngle() const
{
	const float SingularityTest = Z*X - W*Y;
	const float YawY = 2.f*(W*Z + X*Y);
	const float YawX = (1.f - 2.f*(FMath::Square(Y) + FMath::Square(Z)));
	const float SINGULARITY_THRESHOLD = 0.4999995f;
#if PLATFORM_ENABLE_VECTORINTRINSICS
	FRotator RotatorFromQuat;
	union { VectorRegister v; float f[4]; } VRotatorFromQuat;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		// Pitch
		VRotatorFromQuat.f[0] = 3.0f*HALF_PI;	// 270 deg
		// Yaw
		VRotatorFromQuat.f[1] = FMath::Atan2(YawY, YawX);
		// Roll
		VRotatorFromQuat.f[2] = -VRotatorFromQuat.f[1] - (2.f * FMath::Atan2(X, W));
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		// Pitch
		VRotatorFromQuat.f[0] = HALF_PI;	// 90 deg
		// Yaw
		VRotatorFromQuat.f[1] = FMath::Atan2(YawY, YawX);
		//Roll
		VRotatorFromQuat.f[2] = VRotatorFromQuat.f[1] - (2.f * FMath::Atan2(X, W));
	}
	else
	{
		//Pitch
		VRotatorFromQuat.f[0] = FMath::FastAsin(2.f*(SingularityTest));
		// Yaw
		VRotatorFromQuat.f[1] = FMath::Atan2(YawY, YawX);
		// Roll
		VRotatorFromQuat.f[2] = FMath::Atan2(-2.f*(W*X + Y*Z), (1.f - 2.f*(FMath::Square(X) + FMath::Square(Y))));
	}

	VRotatorFromQuat.f[3] = 0.f; // We should initialize this, otherwise the value can be denormalized which is bad for floating point perf.
	VRotatorFromQuat.v = VectorMultiply(VRotatorFromQuat.v, GlobalVectorConstants::RAD_TO_DEG);
	VRotatorFromQuat.v = VectorNormalizeRotator(VRotatorFromQuat.v);
	VectorStoreFloat3(VRotatorFromQuat.v, &RotatorFromQuat);

#else // PLATFORM_ENABLE_VECTORINTRINSICS
	static const float RAD_TO_DEG = (180.f) / PI;
	FEulerAngle RotatorFromQuat;

	if (SingularityTest < -SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = 270.f;
		RotatorFromQuat.Yaw = FMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = -RotatorFromQuat.Yaw - (2.f * FMath::Atan2(X, W) * RAD_TO_DEG);
	}
	else if (SingularityTest > SINGULARITY_THRESHOLD)
	{
		RotatorFromQuat.Pitch = 90.f;
		RotatorFromQuat.Yaw = FMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = RotatorFromQuat.Yaw - (2.f * FMath::Atan2(X, W) * RAD_TO_DEG);
	}
	else
	{
		RotatorFromQuat.Pitch = FMath::FastAsin(2.f*(SingularityTest)) * RAD_TO_DEG;
		RotatorFromQuat.Yaw = FMath::Atan2(YawY, YawX) * RAD_TO_DEG;
		RotatorFromQuat.Roll = FMath::Atan2(-2.f*(W*X + Y*Z), (1.f - 2.f*(FMath::Square(X) + FMath::Square(Y)))) * RAD_TO_DEG;
	}
	//UE - DX
	RotatorFromQuat.Normalize();
#endif // PLATFORM_ENABLE_VECTORINTRINSICS


	return RotatorFromQuat.UEToDX();
}


