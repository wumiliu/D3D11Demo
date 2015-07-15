#include <iostream>
#include <stdio.h>
#include "../DeviceManager.h"
#include "ShaderBufferManager.h"


ShaderBufferManager::ShaderBufferManager()
{
	m_d3dDevice = g_objDeviecManager.GetDevice();
}

ShaderBufferManager::~ShaderBufferManager()
{

}

ShaderBufferManager* ShaderBufferManager::GetInstancePtr()
{
	static ShaderBufferManager _instance;
	return &_instance;
}

ShaderBufferManager& ShaderBufferManager::GetInstance()
{
	return *(GetInstancePtr());
}

void ShaderBufferManager::GetShaderBuffer(const char* pszFileName, ShaderBuffer &pBuffer)
{
	FILE * m_IFileRead;
	fopen_s(&m_IFileRead, pszFileName, "rb");
	fseek(m_IFileRead, 0, SEEK_END);
	int fileSize = ftell(m_IFileRead);
	pBuffer.InitBuffer(fileSize);
	fseek(m_IFileRead, 0, SEEK_SET);
	fread(pBuffer.GetBuffer(), sizeof(unsigned char), fileSize, m_IFileRead);
	pBuffer.m_fileName = pszFileName;
	fclose(m_IFileRead);
}

HRESULT ShaderBufferManager::GetVertexShader(const char* pszFileName, LPVOID pClassLinkage, LPVOID *ppVertexShader)
{
	ShaderBuffer shaderBuffer;
	GetShaderBuffer(pszFileName, shaderBuffer);
	HRESULT result = m_d3dDevice->CreateVertexShader(
		shaderBuffer.GetBuffer(),
		shaderBuffer.GetBufferSize(),
		(ID3D11ClassLinkage *)pClassLinkage,
		(ID3D11VertexShader **)ppVertexShader);

	((ID3D11VertexShader *)ppVertexShader)->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(pszFileName) - 1, pszFileName);

	return result;
}

HRESULT ShaderBufferManager::GetPixelShader(const char* pszFileName, LPVOID pClassLinkage, LPVOID *ppPixelShader)
{
	ShaderBuffer shaderBuffer;
	GetShaderBuffer(pszFileName, shaderBuffer);
	HRESULT result = m_d3dDevice->CreatePixelShader(
		shaderBuffer.GetBuffer(),
		shaderBuffer.GetBufferSize(),
		(ID3D11ClassLinkage *)pClassLinkage,
		(ID3D11PixelShader **)ppPixelShader);
	return result;
}

HRESULT ShaderBufferManager::GetVertexByData(ShaderBuffer &shaderBuffer, LPVOID pClassLinkage, LPVOID *ppVertexShader)
{
	HRESULT result = m_d3dDevice->CreateVertexShader(
		shaderBuffer.GetBuffer(),
		shaderBuffer.GetBufferSize(),
		(ID3D11ClassLinkage *)pClassLinkage,
		(ID3D11VertexShader **)ppVertexShader);
	ID3D11VertexShader *ppVertexShader1= (ID3D11VertexShader *)*ppVertexShader;

	static const char c_szName[] = "My name";
	((ID3D11VertexShader *)ppVertexShader1)->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(shaderBuffer.m_fileName.c_str()) - 1, shaderBuffer.m_fileName.c_str());
	return result;
}

HRESULT ShaderBufferManager::GetLayoutData(ShaderBuffer &shaderBuffer, const LPVOID pInputElementDescs, UINT NumElements, LPVOID *ppInputLayout)
{
	HRESULT result = m_d3dDevice->CreateInputLayout(
		(D3D11_INPUT_ELEMENT_DESC *)pInputElementDescs,
		NumElements,
		shaderBuffer.GetBuffer(),
		shaderBuffer.GetBufferSize(),
		(ID3D11InputLayout**)ppInputLayout);
	return result;
}

HRESULT ShaderBufferManager::GetVertexShaderAndLayout(const char* pszFileName, LPVOID pClassLinkage, 
	LPVOID *ppVertexShader, const LPVOID pInputElementDescs, UINT NumElements, LPVOID *ppInputLayout)
{
	ShaderBuffer shaderBuffer;
	GetShaderBuffer(pszFileName, shaderBuffer);
	HRESULT result = GetVertexByData(shaderBuffer, pClassLinkage, ppVertexShader);
	if (FAILED(result))
	{
		return result;
	}
	result = GetLayoutData(shaderBuffer, pInputElementDescs, NumElements, ppInputLayout);
	return result;
}

HRESULT ShaderBufferManager::CreateBuffer(D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer)
{
	return m_d3dDevice->CreateBuffer(pDesc, pInitialData, ppBuffer);
}

