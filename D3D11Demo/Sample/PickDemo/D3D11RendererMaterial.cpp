#include "D3D11RendererMaterial.h"
#include "DeviceManager.h"
#include "d3dUtil.h"


D3D11RendererMaterial::D3D11RendererMaterial(const RendererMaterialDesc& desc)
{
	m_vertexShader = NULL;
	m_geometryShader = NULL;
	m_hullShader = NULL;
	m_domainShader = NULL;
	m_pInputLayout = NULL;
	m_matrixBuffer = NULL;
	loadShaders(desc);
}


D3D11RendererMaterial::~D3D11RendererMaterial()
{

}

void D3D11RendererMaterial::loadShaders(const RendererMaterialDesc& desc)
{
	if (desc.vertexShaderPath)
	{
		D3DCompileShader(desc.vertexShaderPath, "main", "vs_5_0", &vertexshaderBuffer);
		g_objDeviecManager.CreateVertexShader(vertexshaderBuffer, &m_vertexShader);
	}
	if (desc.pixelShaderPath)
	{
		ID3DBlob* shaderBuffer;
		D3DCompileShader(desc.pixelShaderPath, "main", "ps_5_0", &shaderBuffer);
		g_objDeviecManager.CreatePixelShader(shaderBuffer, &m_pixelShader[0]);
		D3DCompileShader(desc.pixelShaderPath, "mainPick", "ps_5_0", &shaderBuffer);
		g_objDeviecManager.CreatePixelShader(shaderBuffer, &m_pixelShader[1]);
	}

	g_objDeviecManager.CreateConstantBuffer<MatrixBufferShader>(&m_matrixBuffer);
}

void D3D11RendererMaterial::setShaders(int i)
{
	g_objDeviecManager.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->PSSetShader(m_pixelShader[i], NULL, 0);
	g_objDeviecManager.GetImmediateContext()->GSSetShader(m_geometryShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->HSSetShader(m_hullShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->DSSetShader(m_domainShader, NULL, 0);
}


void D3D11RendererMaterial::SetShaderParameters(Matrix world, Matrix view, Matrix proj, ID3D11ShaderResourceView* texture /*= NULL*/)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
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
}

void D3D11RendererMaterial::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *ppShaderResourceViews)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->PSSetShaderResources(StartSlot, NumViews, &ppShaderResourceViews);
}
