#include "Quat.h"
#include "UnrealMathDirectX.h"
#include "UnrealMathVectorCommon.h"
const FQuat FQuat::Identity(0, 0, 0, 1);



void FQuat::ToAxisAndAngle(FVector& Axis, float& Angle) const
{
	Angle = 2.f * FMath::Acos(W);
	Axis = GetRotationAxis();
}

FVector FQuat::RotateVector(FVector V) const
{
#if WITH_DIRECTXMATH
	FVector Result;
	VectorQuaternionVector3Rotate(&Result, &V, this);
	return Result;

	/*
	// In unit testing this appears to be slower than the non-vectorized version.
	#elif PLATFORM_ENABLE_VECTORINTRINSICS
	FQuat VQ(V.X, V.Y, V.Z, 0.f);
	FQuat VT, VR;
	FQuat I = Inverse();
	VectorQuaternionMultiply(&VT, this, &VQ);
	VectorQuaternionMultiply(&VR, &VT, &I);

	return FVector(VR.X, VR.Y, VR.Z);
	*/

#else
	// (q.W*q.W-qv.qv)v + 2(qv.v)qv + 2 q.W (qv x v)

	const FVector qv(X, Y, Z);
	FVector vOut = (2.f * W) * (qv ^ V);
	vOut += ((W * W) - (qv | qv)) * V;
	vOut += (2.f * (qv | V)) * qv;

	return vOut;
#endif
}
//Yaw¡¢Pitch¡¢Roll 
FRotator FQuat::Rotator() const
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
	FRotator RotatorFromQuat;

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

	RotatorFromQuat.Normalize();
#endif // PLATFORM_ENABLE_VECTORINTRINSICS


	return RotatorFromQuat;

}

FQuat::FQuat(const FRotator& R)
{
	*this = R.Quaternion();
}

FQuat::FQuat(FVector Axis, float AngleRad)
{
	const float half_a = 0.5f * AngleRad;
	float s, c;
	FMath::SinCos(&s, &c, half_a);

	X = s * Axis.x;
	Y = s * Axis.y;
	Z = s * Axis.z;
	W = c;

	DiagnosticCheckNaN();
}


FMatrix FQuat::Make(const FQuat& Q, const FVector& Origin)
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

FQuat FQuat::MakeFromEuler(const FVector& Euler)
{
	return FRotator::MakeFromEuler(Euler).Quaternion();
}

FVector FQuat::Euler() const
{
	return Rotator().Euler();

}

bool FQuat::ContainsNaN() const
{
	return (FMath::IsNaN(X) || !FMath::IsFinite(X) ||
		FMath::IsNaN(Y) || !FMath::IsFinite(Y) ||
		FMath::IsNaN(Z) || !FMath::IsFinite(Z) ||
		FMath::IsNaN(W) || !FMath::IsFinite(W)
		);
}

std::string FQuat::ToString() const
{
	char strBuffer[128];
	sprintf_s(strBuffer, sizeof(strBuffer), "X=%3.3f Y=%3.3f Z=%3.3f W=%3.3f", X, Y, Z, W);
	std::string strText = strBuffer;
	return strText;
}

 void FQuat::Normalize(float Tolerance /*= SMALL_NUMBER*/)
{
	const float SquareSum = X * X + Y * Y + Z * Z + W * W;

	if (SquareSum > Tolerance)
	{
		const float Scale = FMath::InvSqrt(SquareSum);

		X *= Scale;
		Y *= Scale;
		Z *= Scale;
		W *= Scale;
	}
	else
	{
		*this = FQuat::Identity;
	}
}



