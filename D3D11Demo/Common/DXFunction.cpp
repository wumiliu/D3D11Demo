#include "DXFunction.h"
#include "BaseFunction.h"
#include "Texture/DDSTextureLoader.h"

ID3D11ShaderResourceView* D3DX11CreateShaderResourceViewFromFile(
	ID3D11Device* pDevice, const char* strFileName)
{
	ID3D11Texture2D* pTexture2D;
	ID3D11ShaderResourceView* srv;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		pDevice,
		BaseFunction::gAnsiToUnicode(strFileName),
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	return srv;
}


HRESULT D3DX11CompileFromFile(LPCWSTR pFileName, D3D_SHADER_MACRO* pDefines, ID3DInclude* pInclude, LPCSTR pEntrypoint, 
	LPCSTR pTarget, UINT Flags1, UINT Flags2, HRESULT* pHResult, ID3DBlob** ppCode, ID3DBlob** ppErrorMsgs, HRESULT* pHResult2)
{
	HRESULT result = S_OK;
	DWORD shaderFlags = Flags1;
//	shaderFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
#endif

	// Compile the pixel shader code.
	result = D3DCompileFromFile(pFileName, pDefines, pInclude, pEntrypoint, pTarget, shaderFlags, Flags2,
		ppCode, ppErrorMsgs);

	return result;
}

void D3DXMatrixTranspose(D3DXMATRIX*pOut, CONST D3DXMATRIX*pM)
{
	pM->Transpose(*pOut);
}
 D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX *pOut)
{
	*pOut = Matrix::Identity;
	return pOut;
}
D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovLH(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf)
{
	*pOut = XMMatrixPerspectiveFovLH(fovy, Aspect, zn, zf);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixOrthoLH(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf)
{
	*pOut = XMMatrixOrthographicLH(w, h, zn, zf);
	return pOut;
}

D3DXVECTOR3* WINAPI D3DXVec3TransformCoord(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM)
{
	pV->Transform(*pOut, *pM);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixRotationYawPitchRoll(D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll)
{
	*pOut = XMMatrixRotationRollPitchYaw(Pitch, Yaw, Roll);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixLookAtLH(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt, CONST D3DXVECTOR3 *pUp)
{
	*pOut = XMMatrixLookAtLH(*pEye, *pAt, *pUp);
	return pOut;
}

D3DXMATRIX* WINAPI D3DXMatrixTranslation(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z)
{
	*pOut = Matrix::CreateTranslation(x, y, z);
	return pOut;
}
