#pragma once
#include "HpD3D9Type.h"

class  DeviceManager
{
public:
	static DeviceManager* GetInstancePtr();
	static DeviceManager& GetInstance();

	ID3D11Device* GetDevice()
	{
		return m_pd3dDevice;
	}

	IDXGIFactory* GetDXGIFactory()
	{
		return m_pDXGIFactory;
	}

	ID3D11DeviceContext* GetImmediateContext()
	{
		return m_pImmediateContext;
	}

	HRESULT CreateVertexShader(ID3DBlob* shaderBuffer, ID3D11VertexShader **ppVertexShader);
	HRESULT CreatePixelShader(ID3DBlob* shaderBuffer, ID3D11PixelShader **ppPixelShader);
	HRESULT CreateInputLayout(LayoutVector vecLayout, ID3DBlob* shaderBuffer, ID3D11InputLayout **ppInputLayout);

	template< class ConstantBuffer>
	HRESULT CreateConstantBuffer(ID3D11Buffer **ppBuffer);


private:
	DeviceManager();
	~DeviceManager();
private:
	ID3D11Device*           m_pd3dDevice;
	ID3D11DeviceContext*    m_pImmediateContext;
	IDXGIFactory*          m_pDXGIFactory;
};
template< class ConstantBuffer>
HRESULT DeviceManager::CreateConstantBuffer(ID3D11Buffer **ppBuffer)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = m_pd3dDevice->CreateBuffer(&matrixBufferDesc, NULL, ppBuffer);
	return result;
}


#define g_objDeviecManager  DeviceManager::GetInstance()