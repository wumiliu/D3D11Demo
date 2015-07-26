#pragma once

#include "HpD3D9Type.h"
struct D3D11ConstantBuffer
{
	D3D11ConstantBuffer()
	{
		bMap = false;
		dataPtr = NULL;
	}
	~D3D11ConstantBuffer()
	{
		SAFE_RELEASE(pConstantBuffer);
	}
	ID3D11Buffer* pConstantBuffer;
	void* Map(ID3D11DeviceContext* pDeviceContext);
	void Update(ID3D11DeviceContext* pDeviceContext);
	bool bMap;
	void* dataPtr;
};



template< class D3D11ShaderType >
struct D3D11Shader
{
	D3D11Shader()
	{
		m_pShader = NULL;
		pReflector = NULL;
	}
	~D3D11Shader()
	{
		Clear();
	}
	void Clear()
	{
		for (uint i = 0; i < vecConstantBuffer.size(); ++i)
		{
		//	SAFE_RELEASE(vecConstantBuffer[i]);
		}
		vecConstantBuffer.clear();
		SAFE_RELEASE(m_pShader);
		SAFE_RELEASE(pReflector);
	}
	D3D11ShaderType*  m_pShader;
	ID3D11ShaderReflection*  pReflector;
	int GetConstantIndex(const char* pszName);
	std::vector<D3D11ConstantBuffer> vecConstantBuffer;

};

template< class D3D11ShaderType >
int D3D11Shader<D3D11ShaderType>::GetConstantIndex(const char* pszName)
{
	D3D11_SHADER_DESC shaderDesc;
	if (pReflector)
	{
		pReflector->GetDesc(&shaderDesc);
		for (uint i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
			pConstantBuffer->GetDesc(&shaderBufferDesc);
			for (uint j = 0; j < shaderBufferDesc.Variables; ++j)
			{
				ID3D11ShaderReflectionVariable* pVariable = pConstantBuffer->GetVariableByIndex(j);
				if (pVariable)
				{
					D3D11_SHADER_VARIABLE_DESC variableDesc;
					memset(&variableDesc, 0, sizeof(D3D11_SHADER_VARIABLE_DESC));
					pVariable->GetDesc(&variableDesc);
					if (strcmp(variableDesc.Name,pszName) == 0)
					{
						return i;
					}
					
				}
			}
		}
	}
	return 0;
}