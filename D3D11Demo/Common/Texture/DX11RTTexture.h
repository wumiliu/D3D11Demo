#pragma once
#include "../HpD3D9Type.h"
class DX11RTTexture
{
public:
	DX11RTTexture();
	~DX11RTTexture();
	void Create(int nWidth, int nHeight);
	void Begin();
	void End();
	void Save();
	void SetBackBufferRenderTarget();
	void Save(const char* fileName);

	ID3D11ShaderResourceView* GetRTView(){ return mSRV; }
public:
	int m_nWidth;
	int m_nHeight;
private:

	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
	D3D11_VIEWPORT mViewport;

	ID3D11DepthStencilView* mDSV;
	ID3D11RenderTargetView* mRTV;
	ID3D11ShaderResourceView* mSRV;
	ID3D11RenderTargetView	*m_renderTargetView;		//渲染对象视图
	ID3D11DepthStencilView	*m_depthStencilView;		//深度/模板视图

};

