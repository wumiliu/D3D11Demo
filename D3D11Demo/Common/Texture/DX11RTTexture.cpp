#include "DX11RTTexture.h"
#include "../DeviceManager.h"

DX11RTTexture::DX11RTTexture()
{
	m_d3dDevice = g_objDeviecManager.GetDevice();
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
}


DX11RTTexture::~DX11RTTexture()
{

}

void DX11RTTexture::Create(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = nWidth;
	texDesc.Height = nHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
//	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;

	ID3D11Texture2D* cubeTex = 0;
	m_d3dDevice->CreateTexture2D(&texDesc, 0, &cubeTex);

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;


	rtvDesc.Texture2DArray.FirstArraySlice = 0;
	(m_d3dDevice->CreateRenderTargetView(cubeTex, &rtvDesc, &mRTV));


	//
	// Create a shader resource view to the cube map.
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	memset(&SRVDesc, 0, sizeof(SRVDesc));
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;
	m_d3dDevice->CreateShaderResourceView(cubeTex, &SRVDesc, &mSRV);



	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = nWidth;
	depthTexDesc.Height = nHeight;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ID3D11Texture2D* depthTex = 0;
	m_d3dDevice->CreateTexture2D(&depthTexDesc, 0, &depthTex);

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	(m_d3dDevice->CreateDepthStencilView(depthTex, &dsvDesc, &mDSV));



	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = (float)nWidth;
	mViewport.Height = (float)nHeight;
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
}

void DX11RTTexture::Begin()
{
	m_renderTargetView = NULL;
	m_depthStencilView = NULL;
	m_deviceContext->OMGetRenderTargets(1, &m_renderTargetView, &m_depthStencilView);
	m_deviceContext->RSSetViewports(1, &mViewport);
	// Bind cube map face as render target.

	m_deviceContext->OMSetRenderTargets(1, &mRTV, mDSV);
	float color[4];
	color[0] = 0.1921568627450980392156862745098f;
	color[1] = 0.30196078431372549019607843137255f;
	color[2] = 0.47450980392156862745098039215686f;
	color[3] = 1.0f;

	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;
	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(mRTV, color);

	// Clear the depth buffer.
	m_deviceContext->ClearDepthStencilView(mDSV, D3D10_CLEAR_DEPTH, 1.0f, 0);

	

}

void DX11RTTexture::End()
{
	SetBackBufferRenderTarget();
	//m_deviceContext->GenerateMips(mSRV);
//	g_objTextureMgr.Save(mSRV, "123aa1.png");
	//g_objBase.EndRTT();
}

void DX11RTTexture::Save()
{
	ID3D11Resource* ppResource;
	mRTV->GetResource(&ppResource);
	ID3D11Texture2D*  texEx;
	HRESULT hr = ppResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texEx));
}

void DX11RTTexture::Save(const char* fileName)
{
	ID3D11Resource* ppResource;
	mRTV->GetResource(&ppResource);
	ID3D11Texture2D*  texEx;
	HRESULT hr = ppResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texEx));
}

void DX11RTTexture::SetBackBufferRenderTarget()
{
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

}

