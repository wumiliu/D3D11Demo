#pragma once
#include "HpD3D9Type.h"



HRESULT   D3DCompileShader(LPCSTR pFileName, LPCSTR pEntrypoint,
	LPCSTR pTarget, ID3DBlob** ppCode, ID3D11ShaderReflection** ppReflector = NULL);


//获取绘制2D的矩阵
XMMATRIX GetShow2DMatrix(int nWidth, int nHegith);

