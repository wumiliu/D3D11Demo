#pragma once

#include "HpD3D9Type.h"


template< class D3D11ShaderType >
struct D3D11Shader
{
	D3D11ShaderType*  m_pShader;
	ID3D11ShaderReflection*  pReflector;
	std::vector<ID3D11Buffer*> vecConstantBuffer;
};
