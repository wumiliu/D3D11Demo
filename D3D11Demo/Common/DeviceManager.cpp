#include "DeviceManager.h"

#pragma comment(lib, "d3d11.lib")
DeviceManager::DeviceManager()
{
	// create DXGIFactory, d3d device

	HRESULT hr = S_OK;
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	/*D3D_DRIVER_TYPE driverTypes[] =
	{
	D3D_DRIVER_TYPE_HARDWARE,
	D3D_DRIVER_TYPE_WARP,
	D3D_DRIVER_TYPE_REFERENCE,

	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );*/

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	D3D_FEATURE_LEVEL	curLevel;
	//创建设备ID3D11Device和设备上下文ID3D11DeviceContext
	hr = D3D11CreateDevice(
		NULL, //设为NULL以选择默认的适配器
		D3D_DRIVER_TYPE_HARDWARE,//DriverType设置驱动类型，一般毫无疑问选择硬件加速
		NULL,//Flags为可选参数，一般为NULL ,D3D11_CREATE_DEVICE_DEBUG
		createDeviceFlags,
		featureLevels,//为我们提供给程序的特征等级的一个数组
		numFeatureLevels,//数组中元素个数
		D3D11_SDK_VERSION,//SDKVersion恒定为D3D11_SDK_VERSION
		&m_pd3dDevice,
		&curLevel,//pFeatureLevel为最后程序选中的特征等级
		&m_pImmediateContext);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Craete device failed!", L"ERROR", MB_OK);
		return ;
	}
	//创建交换链需要获得接口IDXGIFactory，过程是固定的
	//通过如下三步获得接口IDXGIFactory，来创建交换链 
	IDXGIDevice *pDxgiDevice(NULL);
	hr = m_pd3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (FAILED(hr))
	{
		return ;
	}
	IDXGIAdapter *pDxgiAdapter(NULL);
	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pDxgiAdapter));
	hr = pDxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));	
	SAFE_RELEASE(pDxgiAdapter);
	SAFE_RELEASE(pDxgiDevice);

	D3D_FEATURE_LEVEL featureLevel = m_pd3dDevice->GetFeatureLevel();
	wchar_t* strFeatureLevel = NULL;

	if (featureLevel == D3D_FEATURE_LEVEL_9_1)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_9_1";
	}
	else if (featureLevel == D3D_FEATURE_LEVEL_9_2)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_9_2";
	}
	else if (featureLevel == D3D_FEATURE_LEVEL_9_3)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_9_3";
	}
	else if (featureLevel == D3D_FEATURE_LEVEL_10_0)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_10_0";
	}
	else if (featureLevel == D3D_FEATURE_LEVEL_10_1)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_10_1";
	}
	else if (featureLevel == D3D_FEATURE_LEVEL_11_0)
	{
		strFeatureLevel = L"D3D_FEATURE_LEVEL_11_0";
	}
	else
	{
		strFeatureLevel = L"Newer than D3D_FEATURE_LEVEL_11_0";
	}
}


DeviceManager::~DeviceManager()
{

	SAFE_RELEASE(m_pImmediateContext);
	SAFE_RELEASE(m_pDXGIFactory);


/*
#if defined(DEBUG) || defined(_DEBUG)
	ID3D11Debug *d3dDebug;
	HRESULT hr = m_pd3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	if (SUCCEEDED(hr))
	{
		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	if (d3dDebug != nullptr)			d3dDebug->Release();
#endif*/
	SAFE_RELEASE(m_pd3dDevice);
}

DeviceManager* DeviceManager::GetInstancePtr()
{
	static DeviceManager _instance;
	return &_instance;
}

DeviceManager& DeviceManager::GetInstance()
{
	return *GetInstancePtr();
}

HRESULT DeviceManager::CreateVertexShader(ID3DBlob* shaderBuffer, ID3D11VertexShader **ppVertexShader)
{
	HRESULT result = m_pd3dDevice->CreateVertexShader(
		shaderBuffer->GetBufferPointer(),
		shaderBuffer->GetBufferSize(),
		(ID3D11ClassLinkage *)NULL,
		ppVertexShader);
	return result;
}

HRESULT DeviceManager::CreatePixelShader(ID3DBlob* shaderBuffer, ID3D11PixelShader **ppPixelShader)
{
	HRESULT result = m_pd3dDevice->CreatePixelShader(
		shaderBuffer->GetBufferPointer(),
		shaderBuffer->GetBufferSize(),
		(ID3D11ClassLinkage *)NULL,
		ppPixelShader);
	return result;
}

HRESULT DeviceManager::CreateGeometryShader(ID3DBlob* shaderBuffer, ID3D11GeometryShader **ppGeometryShader)
{
	HRESULT result = m_pd3dDevice->CreateGeometryShader(
		shaderBuffer->GetBufferPointer(),
		shaderBuffer->GetBufferSize(),
		(ID3D11ClassLinkage *)NULL,
		ppGeometryShader);
	return result;
}

HRESULT DeviceManager::CreateInputLayout(LayoutVector vecLayout, ID3DBlob* shaderBuffer, ID3D11InputLayout **ppInputLayout)
{
	HRESULT result = m_pd3dDevice->CreateInputLayout(
		&vecLayout[0],
		vecLayout.size(),
		shaderBuffer->GetBufferPointer(),
		shaderBuffer->GetBufferSize(),
		(ID3D11InputLayout**)ppInputLayout);
	return result;

}

HRESULT DeviceManager::CreateConstantBuffer(ID3D11Buffer **ppBuffer, int nSize)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = nSize;
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	HRESULT result = m_pd3dDevice->CreateBuffer(&matrixBufferDesc, NULL, ppBuffer);
	return result;
}


