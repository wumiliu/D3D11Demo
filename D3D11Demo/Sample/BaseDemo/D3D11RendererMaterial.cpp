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
			D3D11_SHADER_DESC shaderDesc;
			pVSReflector->GetDesc(&shaderDesc);
			//即使 hlsl 中定义了多个常量,如果在hlsl 代码中没有使用的话，常量的数量还是为0
			D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc0;
			D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc1;
			D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc2;

			pVSReflector->GetResourceBindingDesc(0, &resourceBindingDesc0);
			pVSReflector->GetResourceBindingDesc(1, &resourceBindingDesc1);
			pVSReflector->GetResourceBindingDesc(2, &resourceBindingDesc2);
			std::vector<D3D11_INPUT_ELEMENT_DESC> InputElements;
			for (uint32 i = 0; i < shaderDesc.InputParameters; ++i)
			{

				D3D11_SIGNATURE_PARAMETER_DESC pDesc2;
				pVSReflector->GetInputParameterDesc(i, &pDesc2);
				D3D11_INPUT_ELEMENT_DESC InputElement;
				InputElement.SemanticName = pDesc2.SemanticName;
				InputElement.SemanticIndex = pDesc2.SemanticIndex;


				const D3D11_INPUT_ELEMENT_DESC InputElements111[] =
				{
					{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};
				// pDesc1.uFlags
			}
			D3D11_SHADER_BUFFER_DESC pDesc;
			D3D11_SHADER_VARIABLE_DESC pVariableDesc;
			ID3D11ShaderReflectionVariable* pVariable = pVSReflector->GetVariableByName("MatrixBuffer");
			pVariable->GetDesc(&pVariableDesc);

			ID3D11ShaderReflectionConstantBuffer* pBuffer = pVSReflector->GetConstantBufferByIndex(0);
			if (pBuffer)
			{
				pBuffer->GetDesc(&pDesc);
				for (uint32 i = 0; i < pDesc.Variables; ++i)
				{
					ID3D11ShaderReflectionVariable* pReflection = pBuffer->GetVariableByIndex(i);
					if (pReflection)
					{
						pReflection->GetDesc(&pVariableDesc);
					}
				}
				m_Shader.vecConstantBuffer.resize(1);
				g_objDeviecManager.CreateConstantBuffer(&m_Shader.vecConstantBuffer[0], pDesc.Size);
			}
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

				if (pReflector)
				{
					D3D11_SHADER_DESC shaderDesc;
					pReflector->GetDesc(&shaderDesc);
					D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc0;
					D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc1;
					D3D11_SHADER_INPUT_BIND_DESC resourceBindingDesc2;

					pReflector->GetResourceBindingDesc(0, &resourceBindingDesc0);
					pReflector->GetResourceBindingDesc(1, &resourceBindingDesc1);
					pReflector->GetResourceBindingDesc(2, &resourceBindingDesc2);
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

void D3D11RendererMaterial::PSSetShaderResources(const char name, void** pBuffer)
{

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
