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
	bUseGeometry = false;
	loadShaders(desc);
}


D3D11RendererMaterial::~D3D11RendererMaterial()
{
	SAFE_RELEASE(m_vertexShader);
	for (uint32 i = 0; i < m_pixelShader.size(); ++i)
	{
		SAFE_RELEASE(m_pixelShader[i]);
	}
	SAFE_RELEASE(m_geometryShader);
	SAFE_RELEASE(m_hullShader);
	SAFE_RELEASE(m_domainShader);
	SAFE_RELEASE(vertexshaderBuffer);
	SAFE_RELEASE(m_pInputLayout);
	SAFE_RELEASE(m_matrixBuffer);

}

void D3D11RendererMaterial::loadShaders(const RendererMaterialDesc& desc)
{
	if (desc.vertexShaderPath)
	{
		D3DCompileShader(desc.vertexShaderPath, "main", "vs_4_0", &vertexshaderBuffer);
		g_objDeviecManager.CreateVertexShader(vertexshaderBuffer, &m_vertexShader);
	}
	if (desc.pixelShaderPath)
	{
		ID3DBlob* shaderBuffer;
		if (desc.vecPass.size() <= 0)
		{
			D3DCompileShader(desc.pixelShaderPath, "main", "ps_4_0", &shaderBuffer);
			ID3D11PixelShader *pPixelShader;
			g_objDeviecManager.CreatePixelShader(shaderBuffer, &pPixelShader);
			m_pixelShader.push_back(pPixelShader);
		}
		else
		{
			ID3DBlob* shaderBuffer;
			ID3D11PixelShader *pPixelShader;
			for (uint32 i = 0; i < desc.vecPass.size();++i)
			{
				D3DCompileShader(desc.pixelShaderPath, desc.vecPass[i].c_str(), "ps_4_0", &shaderBuffer);
				g_objDeviecManager.CreatePixelShader(shaderBuffer, &pPixelShader);
				m_pixelShader.push_back(pPixelShader);
			}
		}
	}
	if (desc.geometryShaderPath)
	{
		ID3DBlob* shaderBuffer;
		D3DCompileShader(desc.geometryShaderPath, "DrawGS", "gs_4_0", &shaderBuffer);
		g_objDeviecManager.CreateGeometryShader(shaderBuffer, &m_geometryShader);
	}
	g_objDeviecManager.CreateConstantBuffer<MatrixBufferShader>(&m_matrixBuffer);
}

void D3D11RendererMaterial::setShaders(uint32 i)
{
	g_objDeviecManager.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
	if (i < m_pixelShader.size())
	{
		g_objDeviecManager.GetImmediateContext()->PSSetShader(m_pixelShader[i], NULL, 0);
	}
	if (bUseGeometry)
	{
		g_objDeviecManager.GetImmediateContext()->GSSetShader(m_geometryShader, NULL, 0);
	}
	else
	{
		g_objDeviecManager.GetImmediateContext()->GSSetShader(NULL, NULL, 0);
	}
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
