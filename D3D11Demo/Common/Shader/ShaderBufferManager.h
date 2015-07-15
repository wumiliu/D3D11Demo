#pragma once
#include <string>
#define  LPVOID void*
struct ShaderBuffer
{
public:
	ShaderBuffer()
	{
		pszBuffer = NULL;
		nBufferSize = 0;
	}
	~ShaderBuffer()
	{
		if (pszBuffer)
		{
			delete[]pszBuffer;
			pszBuffer = NULL;
		}
	}
	void InitBuffer(int nSize)
	{
		if (pszBuffer)
		{
			delete[]pszBuffer;
			pszBuffer = NULL;
		}
		pszBuffer = new unsigned char[nSize];
		nBufferSize = nSize;
	}
	int GetBufferSize(){ return nBufferSize; }
	void* GetBuffer(){ return pszBuffer; }
	std::string m_fileName;
private:
	int nBufferSize;
	unsigned char* pszBuffer;
};

class ShaderBufferManager
{
private:
	ShaderBufferManager();
	~ShaderBufferManager();
public: 
	static ShaderBufferManager* GetInstancePtr();
	static ShaderBufferManager& GetInstance();

	//获取经过 Shader Tool 编译过的数据
	void GetShaderBuffer(const char* pszFileName, ShaderBuffer &pBuffer);

	HRESULT GetVertexShader(const char* pszFileName, LPVOID pClassLinkage, LPVOID *ppVertexShader);
	HRESULT GetPixelShader(const char* pszFileName, LPVOID pClassLinkage, LPVOID *ppPixelShader);

	HRESULT GetVertexByData(ShaderBuffer  &shaderBuffer, LPVOID pClassLinkage, LPVOID *ppVertexShader);

	HRESULT GetLayoutData(ShaderBuffer  &shaderBuffer, const LPVOID pInputElementDescs, UINT NumElements,
		LPVOID *ppInputLayout);

	HRESULT GetVertexShaderAndLayout(const char* pszFileName, LPVOID pClassLinkage, LPVOID *ppVertexShader,
		const LPVOID pInputElementDescs,
		UINT NumElements,
		LPVOID *ppInputLayout);

	HRESULT CreateBuffer(D3D11_BUFFER_DESC *pDesc, const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer);

	template< class BufferShaderType>
	HRESULT CreateBuffer(const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer);
private:
	ID3D11Device* m_d3dDevice;
};

template< class BufferShaderType>
HRESULT ShaderBufferManager::CreateBuffer(const D3D11_SUBRESOURCE_DATA *pInitialData, ID3D11Buffer **ppBuffer)
{
	D3D11_BUFFER_DESC pDesc;
	pDesc.Usage = D3D11_USAGE_DYNAMIC;
	pDesc.ByteWidth = sizeof(BufferShaderType);
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	pDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDesc.MiscFlags = 0;
	pDesc.StructureByteStride = 0;
	return m_d3dDevice->CreateBuffer(&pDesc, pInitialData, ppBuffer);
}

#define  g_objShaderManager ShaderBufferManager::GetInstance()

