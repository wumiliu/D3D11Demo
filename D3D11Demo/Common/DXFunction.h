#pragma once

#include "HpD3D9Type.h"

#ifndef D3DXVECTOR3
#define  D3DXVECTOR3 Vector3
#endif

#ifndef D3DXVECTOR2
#define  D3DXVECTOR2 Vector2
#endif

#ifndef D3DXVECTOR4
#define  D3DXVECTOR4 Vector4
#endif


#ifndef D3DXMATRIX
#define  D3DXMATRIX Matrix
#endif

#ifndef D3DX_PI
#define  D3DX_PI XM_PI
#endif

#ifndef D3DXQUATERNION
#define  D3DXQUATERNION Quaternion
#endif


ID3D11ShaderResourceView* D3DX11CreateShaderResourceViewFromFile(
		ID3D11Device*  pDevice, 
		const char*   strFileName);

HRESULT  D3DX11CompileFromFile( LPCWSTR pFileName,
 D3D_SHADER_MACRO* pDefines,
 ID3DInclude* pInclude,
 LPCSTR pEntrypoint,
 LPCSTR pTarget,
 UINT Flags1,
 UINT Flags2,
 HRESULT* pHResult,
 ID3DBlob** ppCode,
 ID3DBlob** ppErrorMsgs,
 HRESULT* pHResult2);

void D3DXMatrixTranspose(D3DXMATRIX*pOut,
	CONST D3DXMATRIX*pM);

D3DXMATRIX* D3DXMatrixIdentity(D3DXMATRIX *pOut);

D3DXMATRIX* WINAPI D3DXMatrixPerspectiveFovLH
(D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf);

D3DXMATRIX* WINAPI D3DXMatrixOrthoLH(D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf);

D3DXVECTOR3* WINAPI D3DXVec3TransformCoord
(D3DXVECTOR3 *pOut, CONST D3DXVECTOR3 *pV, CONST D3DXMATRIX *pM);

D3DXMATRIX* WINAPI D3DXMatrixRotationYawPitchRoll
(D3DXMATRIX *pOut, FLOAT Yaw, FLOAT Pitch, FLOAT Roll);

D3DXMATRIX* WINAPI D3DXMatrixLookAtLH
(D3DXMATRIX *pOut, CONST D3DXVECTOR3 *pEye, CONST D3DXVECTOR3 *pAt,
CONST D3DXVECTOR3 *pUp);


D3DXMATRIX* WINAPI D3DXMatrixTranslation
(D3DXMATRIX *pOut, FLOAT x, FLOAT y, FLOAT z);


