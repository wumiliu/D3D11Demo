#include "ShaderResource.h"
#include "../SharedResourcePool.h"
#include "../DeviceManager.h"
#include "../Shader/ShaderBufferManager.h"
#include "../CommonStates.h"

template< class BufferShader >
HRESULT IShaderResource::CreateBuffer(ID3D11Buffer **ppBuffer)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(BufferShader);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = g_objShaderManager.CreateBuffer(&matrixBufferDesc, NULL, ppBuffer);
	return result;
}

IShaderResource::IShaderResource()
{
	m_vertexShader = NULL;
	m_pixelShader = NULL;
	m_layout = NULL;
	m_matrixBuffer = NULL;
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
}

IShaderResource::~IShaderResource()
{
	SafeRelease(m_vertexShader);
	SafeRelease(m_pixelShader);
	SafeRelease(m_layout);
	SafeRelease(m_matrixBuffer);
}

void IShaderResource::InitBuffer()
{
	CreateBuffer<MatrixBufferShader>(&m_matrixBuffer);
}

void IShaderResource::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView * ppShaderResourceViews)
{
	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->PSSetShaderResources(StartSlot, NumViews, &ppShaderResourceViews);
}

void IShaderResource::SetShaderParameters(Matrix world, Matrix view, Matrix proj, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferShader* dataPtr;
	result = m_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferShader*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;
	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_matrixBuffer, 0);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	PSSetShaderResources(0, 1, texture);
}

void IShaderResource::RenderShader()
{
	m_deviceContext->IASetInputLayout(m_layout);
	m_deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	m_deviceContext->PSSetShader(m_pixelShader, NULL, 0);
	CommonStates& states = g_objStates;
	ID3D11SamplerState* LinearClamp = states.LinearClamp();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);

}

LightShader::LightShader()
{

}

void LightShader::InitBuffer()
{
	IShaderResource::InitBuffer();
	CreateBuffer<LightBufferShader>(&m_lightBuffer);
}

void LightShader::SetLightBuffer(Vector3 lightDirection, Vector4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightBufferShader* dataPtr;
	result = m_deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (LightBufferShader*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->lightDirection = lightDirection;
	dataPtr->diffuseColor = diffuseColor;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_matrixBuffer, 0);
	m_deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);
}

LightShader::~LightShader()
{
	SafeRelease(m_lightBuffer);
}

ShadowShader::ShadowShader()
{

}

void ShadowShader::InitBuffer()
{
	CreateBuffer<ShadowShader::MatrixBufferType>(&m_matrixBuffer);
	CreateBuffer<ShadowShader::LightBufferType>(&m_lightBuffer);
	CreateBuffer<ShadowShader::LightBufferType2>(&m_lightBuffer2);
}

void ShadowShader::SetLightBuffer(Vector3 lightPosition, Vector4 ambientColor, Vector4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	LightBufferType* dataPtr2;
	LightBufferType2* dataPtr3;

	// Lock the light constant buffer so it can be written to.
	result = m_deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.

	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_deviceContext->PSSetConstantBuffers(0, 1, &m_lightBuffer);

	// Lock the second light constant buffer so it can be written to.
	result = m_deviceContext->Map(m_lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (LightBufferType2*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr3->lightPosition = lightPosition;
	dataPtr3->padding = 0.0f;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_lightBuffer2, 0);

	// Set the position of the light constant buffer in the vertex shader.
	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_deviceContext->VSSetConstantBuffers(1, 1, &m_lightBuffer2);
}

void ShadowShader::SetShaderParameters(Matrix world, Matrix view, Matrix proj, Matrix lightViewMatrix, Matrix lightProjectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMapTexture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ShadowShader::MatrixBufferType* dataPtr;
	result = m_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (ShadowShader::MatrixBufferType*)mappedResource.pData;
	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;
	dataPtr->lightView = lightViewMatrix;
	dataPtr->lightProjection = lightProjectionMatrix;
	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_matrixBuffer, 0);
	m_deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);

	PSSetShaderResources(0, 1, texture);
	PSSetShaderResources(1, 1, depthMapTexture);
}

void ShadowShader::Render(int indexCount)
{
	CommonStates& states = g_objStates;
	ID3D11SamplerState* LinearClamp = states.LinearClamp();
	ID3D11SamplerState* LinearWrap = states.LinearWrap();
	m_deviceContext->PSSetSamplers(0, 1, &LinearClamp);
	m_deviceContext->PSSetSamplers(1, 1, &LinearWrap);

	// Set the sampler states in the pixel shader.

	// Render the triangle.
	m_deviceContext->DrawIndexed(indexCount, 0, 0);
}
