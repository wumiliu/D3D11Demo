#include "d3dUtil.h"
#include "BaseFunction.h"
#pragma comment(lib, "d3dcompiler.lib")
DirectX::XMMATRIX GetShow2DMatrix(int nWidth, int nHegith)
{
	// Transform NDC space [-1,+1]^2 to screen space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f * nWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f * nHegith, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f * nWidth, 0.5f * nHegith, 0.0f, 1.0f);
	return XMMatrixInverse(NULL, toTexSpace);
}

HRESULT D3DCompileShader(LPCSTR pFileName, LPCSTR pEntrypoint,
	LPCSTR pTarget,ID3DBlob** ppCode)
{
	ID3DBlob* errorMessage;
	HRESULT result = S_OK;
	//列主序矩阵（默认设置）
	//行主序矩阵
	DWORD shaderFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;	
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif
	// Compile the pixel shader code.
	result = D3DCompileFromFile(BaseFunction::gAnsiToUnicode(pFileName), NULL, NULL, pEntrypoint, pTarget, shaderFlags, 0,
		ppCode, &errorMessage);

	return result;
}
