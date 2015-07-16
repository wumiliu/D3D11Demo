#include "d3dUtil.h"

DirectX::XMMATRIX GetShow2DMatrix(int nWidth, int nHegith)
{
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f * nWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f * nHegith, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f * nWidth, 0.5f * nHegith, 0.0f, 1.0f);
	return XMMatrixInverse(NULL, toTexSpace);
}