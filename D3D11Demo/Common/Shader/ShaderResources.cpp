#include "ShaderResources.h"


#include "../DemandCreate.h"
#include "../Texture/DirectXHelpers.h"
#include "../SharedResourcePool.h"
#include "ShaderBufferManager.h"
#include "../VertexTypes.h"
#include "../DeviceManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;

struct MatrixBufferType
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct cbSkinned
{
	XMMATRIX gBoneTransforms[58];
};

class ShaderResources::Impl
{
public:
	Impl(_In_ ID3D11Device* device)
		: device(device)
	{ 
		light = NULL;
		base = NULL;
		Skin = NULL;

	}
	void Clear()
	{
		if (base)
		{
			delete base;
			base = NULL;
		}
		if (Skin)
		{
			delete Skin;
			Skin = NULL;
		}
		if (light)
		{
			delete light;
			light = NULL;
		}
	}
	HRESULT CreateShader(const char* vsPath, const char* psPath, _Out_ ShaderResource*& pResult);
	HRESULT CreateShaderEx(const char* vsPath, const char* psPath, _Out_ ShaderResource*& pResult);

	HRESULT CreateShader(const char* vsPath, const char* psPath, _Out_ LightShaderResource*& pResult);
	ShaderResource* base;
	ShaderResource* Skin;
	ShaderResource* baseTexture;
	LightShaderResource* light;
	ComPtr<ID3D11Device> device;
	std::mutex mutex;

	static SharedResourcePool<ID3D11Device*, Impl> instancePool;
};

// Global instance pool.
SharedResourcePool<ID3D11Device*, ShaderResources::Impl> ShaderResources::Impl::instancePool;

HRESULT ShaderResources::Impl::CreateShader(const char* vsPath, const char* psPath, _Out_ ShaderResource*& pResult)
{
	HRESULT  result = S_OK;
	ShaderBuffer pBuffer;
	g_objShaderManager.GetShaderBuffer(vsPath, pBuffer);
	g_objShaderManager.GetVertexByData(pBuffer, NULL, (void**)&(pResult->m_vertexShader));
	result = g_objShaderManager.GetLayoutData(pBuffer,
		(void*)VertexPositionColorTexture::InputElements,
		VertexPositionColorTexture::InputElementCount,
		(void**)&(pResult->m_layout));
	result = g_objShaderManager.GetPixelShader(
		psPath,
		NULL,
		(void**)&pResult->m_pixelShader);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = g_objShaderManager.CreateBuffer(&matrixBufferDesc, NULL, &pResult->m_matrixBuffer);
	return result;
}

HRESULT ShaderResources::Impl::CreateShader(const char* vsPath, const char* psPath, _Out_ LightShaderResource*& pResult)
{
	HRESULT  result = S_OK;
	ShaderBuffer pBuffer;
	g_objShaderManager.GetShaderBuffer(vsPath, pBuffer);
	g_objShaderManager.GetVertexByData(pBuffer, NULL, (void**)&(pResult->m_vertexShader));
	result = g_objShaderManager.GetLayoutData(pBuffer,
		(void*)VertexPositionNormalTexture::InputElements,
		VertexPositionNormalTexture::InputElementCount,
		(void**)&(pResult->m_layout));
	result = g_objShaderManager.GetPixelShader(
		psPath,
		NULL,
		(void**)&pResult->m_pixelShader);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(LightShaderResource::MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = g_objShaderManager.CreateBuffer(&matrixBufferDesc, NULL, &pResult->m_matrixBuffer);

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightShaderResource::LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = g_objShaderManager.CreateBuffer(&lightBufferDesc, NULL, &pResult->m_lightBuffer);

	return result;
}

HRESULT ShaderResources::Impl::CreateShaderEx(const char* vsPath, const char* psPath, _Out_ ShaderResource*& pResult)
{
	HRESULT  result = S_OK;
	ShaderBuffer pBuffer;
	g_objShaderManager.GetShaderBuffer(vsPath, pBuffer);
	g_objShaderManager.GetVertexByData(pBuffer, NULL, (void**)&(pResult->m_vertexShader));
	const D3D11_INPUT_ELEMENT_DESC  PosNormalTexTanSkinned[6] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	result = g_objShaderManager.GetLayoutData(pBuffer,
		PosNormalTexTanSkinned,
		4,
		(void**)&(pResult->m_layout));
/*
	result = g_objShaderManager.GetLayoutData(pBuffer,
		(void*)VertexPositionTexture::InputElements,
		VertexPositionTexture::InputElementCount,
		(void**)&(pResult->m_layout));*/
	result = g_objShaderManager.GetPixelShader(
		psPath,
		NULL,
		(void**)&pResult->m_pixelShader);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = g_objShaderManager.CreateBuffer(&matrixBufferDesc, NULL, &pResult->m_matrixBuffer);


	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(cbSkinned);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = g_objShaderManager.CreateBuffer(&matrixBufferDesc, NULL, &pResult->BoneTransforms);

	return result;
}

ShaderResources::ShaderResources()
{
	ID3D11Device* d3dDevice = g_objDeviecManager.GetDevice();
	pImpl = Impl::instancePool.DemandCreate(d3dDevice);
}


ShaderResources::~ShaderResources()
{
	pImpl->Clear();
}




ShaderResources* ShaderResources::GetInstancePtr()
{
	static ShaderResources mResource;
	return &mResource;
}

ShaderResources& ShaderResources::GetInstance()
{
	return *GetInstancePtr();
}
ShaderResource* __cdecl ShaderResources::BaseShader() const
{
	const char* vsPath = "Shaders\\DynamicPrimitiveVS.shader";
	const char* psPath = "Shaders\\DynamicPrimitivePS.shader";
	if (pImpl->base == NULL)
	{
		pImpl->base = new ShaderResource();
		pImpl->CreateShader(vsPath, psPath, pImpl->base);
	}
	return pImpl->base;

}

ShaderResource* __cdecl ShaderResources::BaseTextureShader() const
{
	const char* vsPath = "Shaders\\TexTureVS.shader";
	const char* psPath = "Shaders\\TexTurePS.shader";
	if (pImpl->baseTexture == NULL)
	{
		pImpl->baseTexture = new ShaderResource();
		pImpl->CreateShader(vsPath, psPath, pImpl->base);
	}
	return pImpl->baseTexture;
}

LightShaderResource* __cdecl ShaderResources::LightShader() const
{
	const char* vsPath = "Shaders\\lightVS.shader";
	const char* psPath = "Shaders\\lightPS.shader";
	if (pImpl->light == NULL)
	{
		pImpl->light = new LightShaderResource();
		pImpl->CreateShader(vsPath, psPath, pImpl->light);
	}
	return pImpl->light;
}

ShaderResource* __cdecl ShaderResources::SkinShader() const
{
	const char* vsPath = "Shaders\\SkinVS.shader";
	const char* psPath = "Shaders\\SkinPS.shader";
	if (pImpl->Skin == NULL)
	{
		pImpl->Skin = new ShaderResource();
		pImpl->CreateShaderEx(vsPath, psPath, pImpl->Skin);
	}
	return pImpl->Skin;
}



void ShaderResource::SetTextureResource(ID3D11ShaderResourceView* pResourceView)
{
	// Set shader texture resource in the pixel shader.
	m_deviceContext->PSSetShaderResources(0, 1, &pResourceView);
	
}
void ShaderResource::SetLayout()
{
	m_deviceContext->IASetInputLayout(m_layout);
}

void ShaderResource::SetVSResource(Matrix world, Matrix view, Matrix proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	result = m_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set the vertex input layout.
	//将顶点着色器和像素着色器绑定到管线上
	//当我们使用Draw方法时，将顶点缓存信息Vertex Buffer传入到绘图管线中进行绘制，这样关于顶点着色器和像素着色器就了解完毕了
	// Set the vertex and pixel shaders that will be used to render this triangle.

}


void ShaderResource::SetBoneTransforms(const XMMATRIX* M, int cnt)
{
	XMMATRIX tmp[58];
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	cbSkinned* dataPtr;
	unsigned int bufferNumber;
	result = m_deviceContext->Map(BoneTransforms, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (cbSkinned*)mappedResource.pData;
	int nSize = sizeof(XMMATRIX)* cnt;
	//memcpy(tmp, dataPtr, nSize* cnt);
	memcpy(dataPtr, M, nSize);
	// Unlock the constant buffer.
	m_deviceContext->Unmap(BoneTransforms, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 1;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &BoneTransforms);
}


ShaderResource::ShaderResource()
{
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
}

void ShaderResource::Render()
{
	m_deviceContext->IASetInputLayout(m_layout);
	m_deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	m_deviceContext->PSSetShader(m_pixelShader, NULL, 0);
}


LightShaderResource::LightShaderResource()
{
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
}

void LightShaderResource::SetLayout()
{
	
}

void LightShaderResource::SetVSResource(Matrix world, Matrix view, Matrix proj)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	result = m_deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = world;
	dataPtr->view = view;
	dataPtr->projection = proj;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	m_deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set the vertex input layout.
	//将顶点着色器和像素着色器绑定到管线上
	//当我们使用Draw方法时，将顶点缓存信息Vertex Buffer传入到绘图管线中进行绘制，这样关于顶点着色器和像素着色器就了解完毕了
	// Set the vertex and pixel shaders that will be used to render this triangle.

}

void LightShaderResource::SetTextureResource(ID3D11ShaderResourceView* pResourceView)
{
	// Set shader texture resource in the pixel shader.
	m_deviceContext->PSSetShaderResources(0, 1, &pResourceView);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT	result = m_deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return;
	}
	XMVECTOR diffuseColor = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 lightDirection = { 0.0f, 0.0f, 1.0f };
	// Initialize the light object.


	LightBufferType* dataPtr2;
	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	m_deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	int bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	m_deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

}

void LightShaderResource::Render()
{
	m_deviceContext->IASetInputLayout(m_layout);
	m_deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	m_deviceContext->PSSetShader(m_pixelShader, NULL, 0);

}
