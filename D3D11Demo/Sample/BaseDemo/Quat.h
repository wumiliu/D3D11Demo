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

struct FMath
{
	static FORCEINLINE float Fmod(float X, float Y) { return fmodf(X, Y); }
	static FORCEINLINE float Sin(float Value) { return sinf(Value); }
	static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Cos(float Value) { return cosf(Value); }
	static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
	static FORCEINLINE float Tan(float Value) { return tanf(Value); }
	static FORCEINLINE float Atan(float Value) { return atanf(Value); }
	static FORCEINLINE float Atan2(float Y, float X) { return atan2f(Y, X); }
	static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
	static FORCEINLINE float Pow(float A, float B) { return powf(A, B); }

	template< class T >
	static FORCEINLINE T Square(const T A)
	{
		return A*A;
	}

	/** Computes absolute value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Abs(const T A)
	{
		return (A >= (T)0) ? A : -A;
	}

	/** Returns 1, 0, or -1 depending on relation of T to 0 */
	template< class T >
	static CONSTEXPR FORCEINLINE T Sign(const T A)
	{
		return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
	}

	/** Returns higher value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Max(const T A, const T B)
	{
		return (A >= B) ? A : B;
	}

	/** Returns lower value in a generic way */
	template< class T >
	static CONSTEXPR FORCEINLINE T Min(const T A, const T B)
	{
		return (A <= B) ? A : B;
	}



	static FORCEINLINE float FastAsin(float Value)
	{
		// Clamp input to [-1,1].
		bool nonnegative = (Value >= 0.0f);
		float x = FMath::Abs(Value);
		float omx = 1.0f - x;
		if (omx < 0.0f)
		{
			omx = 0.0f;
		}
		float root = FMath::Sqrt(omx);
		// 7-degree minimax approximation
		float result = ((((((-0.0012624911f * x + 0.0066700901f) * x - 0.0170881256f) * x + 0.0308918810f) * x - 0.0501743046f) * x + 0.0889789874f) * x - 0.2145988016f) * x + FASTASIN_HALF_PI;
		result *= root;  // acos(|x|)
		// acos(x) = pi - acos(-x) when x < 0, asin(x) = pi/2 - acos(x)
		return (nonnegative ? FASTASIN_HALF_PI - result : result - FASTASIN_HALF_PI);
	}


	static FORCEINLINE void SinCos(float* ScalarSin, float* ScalarCos, float  Value)
	{
		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = (INV_PI*0.5f)*Value;
		if (Value >= 0.0f)
		{
			quotient = (float)((int)(quotient + 0.5f));
		}
		else
		{
			quotient = (float)((int)(quotient - 0.5f));
		}
		float y = Value - (2.0f*PI)*quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > HALF_PI)
		{
			y = PI - y;
			sign = -1.0f;
		}
		else if (y < -HALF_PI)
		{
			y = -PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*ScalarSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*ScalarCos = sign*p;
	}


	static FORCEINLINE float InvSqrt(float F)
	{
		static const __m128 fThree = _mm_set_ss(3.0f);
		static const __m128 fOneHalf = _mm_set_ss(0.5f);
		__m128 Y0, X0, Temp;
		float temp;

		Y0 = _mm_set_ss(F);
		X0 = _mm_rsqrt_ss(Y0);	// 1/sqrt estimate (12 bits)

		// Newton-Raphson iteration (X1 = 0.5*X0*(3-(Y*X0)*X0))
		Temp = _mm_mul_ss(_mm_mul_ss(Y0, X0), X0);	// (Y*X0)*X0
		Temp = _mm_sub_ss(fThree, Temp);				// (3-(Y*X0)*X0)
		Temp = _mm_mul_ss(X0, Temp);					// X0*(3-(Y*X0)*X0)
		Temp = _mm_mul_ss(fOneHalf, Temp);			// 0.5*X0*(3-(Y*X0)*X0)
		_mm_store_ss(&temp, Temp);

		return temp;
	}

	static FORCEINLINE bool IsNaN(float A) { return _isnan(A) != 0; }
	static FORCEINLINE bool IsFinite(float A) { return _finite(A) != 0; }

};


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
