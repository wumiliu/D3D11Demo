#pragma once

// Normalize Rotator
FORCEINLINE VectorRegister VectorNormalizeRotator(const VectorRegister& UnnormalizedRotator)
{
	static const VectorRegister VEC_360 = MakeVectorRegister(360.f, 360.f, 360.f, 360.f);
	static const VectorRegister VEC_180 = MakeVectorRegister(180.f, 180.f, 180.f, 180.f);

	// shift in the range [-360,360]
	VectorRegister V0 = VectorMod(UnnormalizedRotator, VEC_360);
	VectorRegister V1 = VectorAdd(V0, VEC_360);
	VectorRegister V2 = VectorSelect(VectorCompareGE(V0, VectorZero()), V0, V1);

	// shift to [-180,180]
	VectorRegister V3 = VectorSubtract(V2, VEC_360);
	VectorRegister V4 = VectorSelect(VectorCompareGT(V2, VEC_180), V3, V2);

	return  V4;
}