#include "D3D11RendererMaterial.h"

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
			g_objDeviecManager.CreateConstantBuffer(&m_Shader->vecConstantBuffer[i].pConstantBuffer, shaderBufferDesc.Size);
		}
	}
}


D3D11RendererMaterial::D3D11RendererMaterial(const RendererMaterialDesc& desc)
{
	m_geometryShader = NULL;
	m_hullShader = NULL;
	m_domainShader = NULL;
	m_pInputLayout = NULL;
	loadShaders(desc);
}


D3D11RendererMaterial::~D3D11RendererMaterial()
{
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
		ID3D11ShaderReflection* pVSReflector;
		D3DCompileShader(desc.vertexShaderPath, "main", "vs_4_0", &vertexshaderBuffer, &pVSReflector);
		g_objDeviecManager.CreateVertexShader(vertexshaderBuffer, &m_vertexShader.m_pShader);
		if (pVSReflector)
		{
			CreateD3D11Shader(&m_vertexShader, pVSReflector);
		}
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

void D3D11RendererMaterial::setShaders(uint32 nIndex)
{
//	g_objDeviecManager.GetImmediateContext()->VSSetShaderResources(0, 1, NULL);
	for (uint32 i = 0; i < m_vertexShader.vecConstantBuffer.size();++i)
	{
		m_vertexShader.vecConstantBuffer[i].Update(g_objDeviecManager.GetImmediateContext());
		g_objDeviecManager.GetImmediateContext()->VSSetConstantBuffers(i, 1,
			&m_vertexShader.vecConstantBuffer[i].pConstantBuffer);
		
	}
	for (uint32 i = 0; i < m_Shader1.vecConstantBuffer.size(); ++i)
	{
		m_Shader1.vecConstantBuffer[i].Update(g_objDeviecManager.GetImmediateContext());
		g_objDeviecManager.GetImmediateContext()->PSSetConstantBuffers(i, 1,
			&m_Shader1.vecConstantBuffer[i].pConstantBuffer);
	}
	g_objDeviecManager.GetImmediateContext()->VSSetShader(m_vertexShader.m_pShader, NULL, 0);
	if (nIndex < m_pixelShader.size())
	{
		g_objDeviecManager.GetImmediateContext()->PSSetShader(m_pixelShader[nIndex], NULL, 0);
	}
	g_objDeviecManager.GetImmediateContext()->GSSetShader(m_geometryShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->HSSetShader(m_hullShader, NULL, 0);
	g_objDeviecManager.GetImmediateContext()->DSSetShader(m_domainShader, NULL, 0);
}


void D3D11RendererMaterial::PSSetConstantBuffers(const char* name, void* pBuffer)
{
	if (m_Shader1.vecConstantBuffer.size() <= 0)
	{
		return;
	}
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	ID3D11ShaderReflectionVariable* pReflectionVariable = m_Shader1.pReflector->GetVariableByName(name);
	D3D11_SHADER_VARIABLE_DESC pVariableDesc;
	int nIndex = m_Shader1.GetConstantIndex(name);
	
	D3D11_SHADER_INPUT_BIND_DESC pBindDesc;
	memset(&pBindDesc, 0, sizeof(D3D11_SHADER_VARIABLE_DESC));
	m_Shader1.pReflector->GetResourceBindingDescByName("$Globals", &pBindDesc);
	if (pReflectionVariable)
	{
	/*	if (pBindDesc.BindCount <=0)
		{
			return;
		}*/
		pReflectionVariable->GetDesc(&pVariableDesc);
		void* dataPtr;
		//D3D11_MAP_WRITE_NO_OVERWRITE 
		//D3D11_MAP_WRITE_DISCARD
		dataPtr = m_Shader1.vecConstantBuffer[nIndex].Map(m_deviceContext);
		unsigned char*  pDataBuffer = (unsigned char*)dataPtr;
		memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, pBuffer, pVariableDesc.Size);
	}
}

void D3D11RendererMaterial::PSSetShaderResources(const char* name, ID3D11ShaderResourceView** ppShaderResourceViews)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	D3D11_SHADER_INPUT_BIND_DESC pBindDesc;
	//memset(&pBindDesc, 0, sizeof(D3D11_SHADER_VARIABLE_DESC));
	m_Shader1.pReflector->GetResourceBindingDescByName(name, &pBindDesc);
	if (D3D_SIT_TEXTURE == pBindDesc.Type)
	{
		m_deviceContext->PSSetShaderResources(pBindDesc.BindPoint, pBindDesc.BindCount, ppShaderResourceViews);
	}
}

void D3D11RendererMaterial::PSSetShaderResources(uint32 StartSlot, uint32 NumViews, ID3D11ShaderResourceView* pShaderResourceViews)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	m_deviceContext->PSSetShaderResources(StartSlot, NumViews, &pShaderResourceViews);
}

void D3D11RendererMaterial::VSSetConstantBuffers(const char* name, void* pBuffer)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	ID3D11ShaderReflectionVariable* pReflectionVariable = m_vertexShader.pReflector->GetVariableByName(name);
	D3D11_SHADER_VARIABLE_DESC pVariableDesc;
	memset(&pVariableDesc, 0, sizeof(D3D11_SHADER_VARIABLE_DESC));
	int nIndex = m_vertexShader.GetConstantIndex(name);
	if (pReflectionVariable)
	{
		pReflectionVariable->GetDesc(&pVariableDesc);
		if (pVariableDesc.Size> 0)
		{
			void* dataPtr = m_vertexShader.vecConstantBuffer[nIndex].Map(m_deviceContext);
			unsigned char*  pDataBuffer = (unsigned char*)dataPtr;
			memcpy_s(pDataBuffer + pVariableDesc.StartOffset, pVariableDesc.Size, pBuffer, pVariableDesc.Size);
			GO* dataPtr1 = (GO*)dataPtr;
		}

	}
}

void D3D11RendererMaterial::SetMatrix(Matrix world, Matrix view, Matrix proj)
{
	D3D11_SHADER_INPUT_BIND_DESC pBindDesc;
	m_vertexShader.pReflector->GetResourceBindingDescByName("MatrixBuffer", &pBindDesc);
	ID3D11ShaderReflectionConstantBuffer* pBuffer = m_vertexShader.pReflector->GetConstantBufferByName("MatrixBuffer");
	D3D11_SHADER_BUFFER_DESC pDesc;
	pBuffer->GetDesc(&pDesc);
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	void* dataPtr = m_vertexShader.vecConstantBuffer[pBindDesc.BindPoint].Map(m_deviceContext);
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
	
	MatrixBufferShader* dataPtr1 = (MatrixBufferShader*)dataPtr;
	m_vertexShader.vecConstantBuffer[pBindDesc.BindPoint].Update(m_deviceContext);
	m_deviceContext->VSSetConstantBuffers(pBindDesc.BindPoint, pBindDesc.BindCount, &m_vertexShader.vecConstantBuffer[pBindDesc.BindPoint].pConstantBuffer);
}

void D3D11RendererMaterial::VSSetShaderResources(uint32 StartSlot, uint32 NumViews, ID3D11ShaderResourceView* pShaderResourceViews)
{
	ID3D11DeviceContext		*m_deviceContext = g_objDeviecManager.GetImmediateContext();
	m_deviceContext->VSSetShaderResources(StartSlot, NumViews,&pShaderResourceViews);
}
