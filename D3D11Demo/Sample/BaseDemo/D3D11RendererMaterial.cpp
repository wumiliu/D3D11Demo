#include "D3D11RendererMaterial.h"
#include "DeviceManager.h"
#include "d3dUtil.h"


template< class D3D11ShaderType >
void CreateD3D11Shader(D3D11Shader<D3D11ShaderType>* m_Shader, ID3D11ShaderReflection* pReflector)
{
	///即使 hlsl 中定义了多个常量,如果在hlsl 代码中没有使用的话，常量的数量还是为0
	D3D11_SHADER_DESC shaderDesc;
	if (pReflector)
	{
		m_Shader->pReflector = pReflector;
		pReflector->GetDesc(&shaderDesc);
		m_Shader->vecConstantBuffer.resize(shaderDesc.ConstantBuffers);
		for (uint i = 0; i < shaderDesc.ConstantBuffers; ++i)
		{
			ID3D11ShaderReflectionConstantBuffer* pConstantBuffer = pReflector->GetConstantBufferByIndex(i);
			D3D11_SHADER_BUFFER_DESC shaderBufferDesc;
			pConstantBuffer->GetDesc(&shaderBufferDesc);
			g_objDeviecManager.CreateConstantBuffer(&m_Shader->vecConstantBuffer[i], shaderBufferDesc.Size);
		}
	}
}


D3D11RendererMaterial::D3D11RendererMaterial(const RendererMaterialDesc& desc)
{
	m_vertexShader = NULL;
	m_geometryShader = NULL;
	m_hullShader = NULL;
	m_domainShader = NULL;
	m_pInputLayout = NULL;
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


}

void D3D11RendererMaterial::loadShaders(const RendererMaterialDesc& desc)
{
	if (desc.vertexShaderPath)
	{
		D3DCompileShader(desc.vertexShaderPath, "main", "vs_4_0", &vertexshaderBuffer, &pVSReflector);
		if (pVSReflector)
		{
			CreateD3D11Shader(&m_Shader, pVSReflector);
		}
		g_objDeviecManager.CreateVertexShader(vertexshaderBuffer, &m_vertexShader);
		m_Shader.m_pShader = m_vertexShader;
		m_Shader.pReflector = pVSReflector;
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
			ID3D11ShaderReflection* pReflector = NULL;
			for (uint32 i = 0; i < desc.vecPass.size(); ++i)
			{
				D3DCompileShader(desc.pixelShaderPath, desc.vecPass[i].c_str(), "ps_4_0", &shaderBuffer, &pReflector);
				g_objDeviecManager.CreatePixelShader(shaderBuffer, &pPixelShader);
				m_pixelShader.push_back(pPixelShader);
				if (i == 0)
				{
					CreateD3D11Shader(&m_Shader1, pReflector);
				}
				
			}
				
		}
	}
}

void D3D11RendererMaterial::setShaders(uint32 i)
{
	g_objDeviecManager.GetImmediateContext()->VSSetShader(m_vertexShader, NULL, 0);
	if (i < m_pixelShader.size())
	{
		g_objDeviecManager.GetImmediateContext()->PSSetShader(m_pixelShader[i], NULL, 0);
	}
	g_objDeviecManager.GetImmediateContext()->GSSetShader(m_geometryShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->HSSetShader(m_hullShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->DSSetShader(m_domainShader, NULL, 0);
}


void D3D11RendererMaterial::PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView *ppShaderResourceViews)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->PSSetShaderResources(StartSlot, NumViews, &ppShaderResourceViews);
}

void D3D11RendererMaterial::PSSetConstantBuffers(const char* name, void* pBuffer)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	ID3D11ShaderReflectionVariable* pReflectionVariable = m_Shader1.pReflector->GetVariableByName(name);
	D3D11_SHADER_VARIABLE_DESC pVariableDesc;
	int nIndex = m_Shader1.GetConstantIndex(name);
	if (pReflectionVariable)
	{
		pReflectionVariable->GetDesc(&pVariableDesc);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		void* dataPtr;
		HRESULT result = m_deviceContext->Map(m_Shader1.vecConstantBuffer[nIndex], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return;
		}
		dataPtr = mappedResource.pData;
		unsigned char*  pDataBuffer = (unsigned char*)dataPtr;
		memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, pBuffer, pVariableDesc.Size);
		m_deviceContext->Unmap(m_Shader1.vecConstantBuffer[nIndex], 0);
		m_deviceContext->PSSetConstantBuffers(nIndex, 1, &m_Shader1.vecConstantBuffer[nIndex]);
	}
}

void D3D11RendererMaterial::PSSetShaderResources(const char* name, void* ppBuffer)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	D3D11_SHADER_INPUT_BIND_DESC pBindDesc;
	//memset(&pBindDesc, 0, sizeof(D3D11_SHADER_VARIABLE_DESC));
	m_Shader1.pReflector->GetResourceBindingDescByName(name, &pBindDesc);
	if (D3D_SIT_TEXTURE == pBindDesc.Type)
	{
		m_deviceContext->PSSetShaderResources(pBindDesc.BindPoint, pBindDesc.BindCount, (ID3D11ShaderResourceView **)&ppBuffer);
	}
	else if (D3D_SIT_CBUFFER == pBindDesc.Type)
	{
		ID3D11ShaderReflectionConstantBuffer* pBuffer = m_Shader1.pReflector->GetConstantBufferByName(name);
		if (pBuffer)
		{
			D3D11_SHADER_VARIABLE_DESC pVariableDesc;
			ID3D11ShaderReflectionVariable* pWorldVariable = pBuffer->GetVariableByIndex(0);
			pWorldVariable->GetDesc(&pVariableDesc);
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			void* dataPtr;
			HRESULT result = m_deviceContext->Map(m_Shader1.vecConstantBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(result))
			{
				return;
			}
			dataPtr = mappedResource.pData;
			unsigned char*  pDataBuffer = (unsigned char*)dataPtr;
			memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, ppBuffer, pVariableDesc.Size);
			m_deviceContext->Unmap(m_Shader1.vecConstantBuffer[0], 0);
			m_deviceContext->PSSetConstantBuffers(0, 1, &m_Shader1.vecConstantBuffer[0]);
		}
	}
}

void D3D11RendererMaterial::SetMatrix(Matrix world, Matrix view, Matrix proj)
{
	D3D11_SHADER_INPUT_BIND_DESC pBindDesc;
	m_Shader.pReflector->GetResourceBindingDescByName("MatrixBuffer", &pBindDesc);
	ID3D11ShaderReflectionConstantBuffer* pBuffer = m_Shader.pReflector->GetConstantBufferByName("MatrixBuffer");
	D3D11_SHADER_BUFFER_DESC pDesc;
	pBuffer->GetDesc(&pDesc);
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	void* dataPtr;
	result = m_deviceContext->Map(m_Shader.vecConstantBuffer[0], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	dataPtr = mappedResource.pData;
	D3D11_SHADER_VARIABLE_DESC pVariableDesc;
	ID3D11ShaderReflectionVariable* pWorldVariable = pBuffer->GetVariableByName("worldMatrix");
	pWorldVariable->GetDesc(&pVariableDesc);
	unsigned char*  pDataBuffer = (unsigned char*)dataPtr;
	memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, (void*)&world, 1 * sizeof(Matrix));
	ID3D11ShaderReflectionVariable* pViewVariable = pBuffer->GetVariableByName("viewMatrix");
	pViewVariable->GetDesc(&pVariableDesc);
	memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, (void*)&view, 1 * sizeof(Matrix));
	ID3D11ShaderReflectionVariable* pProjVariable = pBuffer->GetVariableByName("projectionMatrix");
	pProjVariable->GetDesc(&pVariableDesc);
	memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, (void*)&proj, 1 * sizeof(Matrix));
	m_deviceContext->Unmap(m_Shader.vecConstantBuffer[0], 0);
	m_deviceContext->VSSetConstantBuffers(pBindDesc.BindPoint, pBindDesc.BindCount, &m_Shader.vecConstantBuffer[0]);
}
