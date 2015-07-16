#include "ShaderManager.h"
#include "../DemandCreate.h"
#include "../Texture/DirectXHelpers.h"
#include "../SharedResourcePool.h"
#include "../DeviceManager.h"
#include "../VertexTypes.h"
#include "../Shader/ShaderBufferManager.h"
#include "../CommonStates.h"

using namespace DirectX;
using namespace Microsoft::WRL;


class ShaderManager::Impl
{
public:

	Impl(_In_ ID3D11Device* device)
		: device(device)
	{
		base = NULL;
		light = NULL;
		shadow = NULL;
		DepthShader = NULL;
	}
	void Clear()
	{
		SAFE_DELETE(base);
		SAFE_DELETE(light);
		SAFE_DELETE(shadow);
		SAFE_DELETE(DepthShader);

	}
	template< class ShaderClass >
	HRESULT CreateShader(const char* vsPath, const char* psPath, const LPVOID pInputElementDescs, UINT NumElements, _Out_ ShaderClass*& pResult);


	IShaderResource* base;
	IShaderResource* DepthShader;

	LightShader* light;
	ShadowShader* shadow;

	ComPtr<ID3D11Device> device;
	std::mutex mutex;
	static SharedResourcePool<ID3D11Device*, Impl> instancePool;

};

template< class ShaderClass >
HRESULT ShaderManager::Impl::CreateShader(const char* vsPath, const char* psPath,
	const LPVOID pInputElementDescs,
	UINT NumElements,
	_Out_ ShaderClass*& pResult)
{
	HRESULT  result = S_OK;
	ShaderBuffer pBuffer;
	g_objShaderManager.GetShaderBuffer(vsPath, pBuffer);
	g_objShaderManager.GetVertexByData(pBuffer, NULL, (void**)&(pResult->m_vertexShader));
	result = g_objShaderManager.GetLayoutData(pBuffer,
		pInputElementDescs,
		NumElements,
		(void**)&(pResult->m_layout));
	result = g_objShaderManager.GetPixelShader(
		psPath,
		NULL,
		(void**)&pResult->m_pixelShader);
	return result;
}


// Global instance pool.
SharedResourcePool<ID3D11Device*, ShaderManager::Impl> ShaderManager::Impl::instancePool;
ShaderManager::ShaderManager()
{
	ID3D11Device* d3dDevice = g_objDeviecManager.GetDevice();
	pImpl = Impl::instancePool.DemandCreate(d3dDevice);
}

ShaderManager::~ShaderManager()
{
	pImpl->Clear();
}
IShaderResource* __cdecl ShaderManager::BaseShader() const
{
	const char* vsPath = "Shaders\\DynamicPrimitiveVS.shader";
	const char* psPath = "Shaders\\DynamicPrimitivePS.shader";
	if (pImpl->base == NULL)
	{
		pImpl->base = new IShaderResource();
		pImpl->CreateShader(vsPath, psPath,
			(void*)VertexPositionColorTexture::InputElements,
			VertexPositionColorTexture::InputElementCount,
			pImpl->base);
		pImpl->base->InitBuffer();
	}
	return pImpl->base;
}

IShaderResource* __cdecl ShaderManager::DepthShader() const
{
	const char* vsPath = "Shaders\\DepthVS.shader";
	const char* psPath = "Shaders\\DepthPS.shader";
	if (pImpl->DepthShader == NULL)
	{
		pImpl->DepthShader = new IShaderResource();
		pImpl->CreateShader(vsPath, psPath, (void*)VertexPositionNormalTexture::InputElements,
			VertexPositionNormalTexture::InputElementCount, pImpl->DepthShader);
		pImpl->DepthShader->InitBuffer();
	}
	return pImpl->DepthShader;
}

LightShader* __cdecl ShaderManager::BaseLightShader() const
{
	const char* vsPath = "Shaders\\lightVS.shader";
	const char* psPath = "Shaders\\lightPS.shader";
	if (pImpl->light == NULL)
	{
		pImpl->light = new LightShader();
		pImpl->CreateShader(vsPath, psPath, (void*)VertexPositionColorTexture::InputElements,
			VertexPositionColorTexture::InputElementCount, pImpl->light);
		pImpl->light->InitBuffer();
	}
	return pImpl->light;
}

ShadowShader* __cdecl ShaderManager::BaseShadowShader() const
{
	const char* vsPath = "Shaders\\ShadowVS.shader";
	const char* psPath = "Shaders\\ShadowPS.shader";
	if (pImpl->shadow == NULL)
	{
		pImpl->shadow = new ShadowShader();
		D3D11_INPUT_ELEMENT_DESC polygonLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT ,D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		unsigned int numElements = 3;
		pImpl->CreateShader(vsPath, psPath, (void*)polygonLayout,
			numElements, pImpl->shadow);
		pImpl->shadow->InitBuffer();
	}
	return pImpl->shadow;
}

