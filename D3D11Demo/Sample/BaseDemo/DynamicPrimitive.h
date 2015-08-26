#pragma once
#include "HpD3D9Type.h"
#include "VertexTypes.h"


enum PrimitiveType
{
	PRIMITIVE_UNDEFINED = 0,
	PRIMITIVE_POINTLIST = 1,
	PRIMITIVE_LINELIST = 2,
	PRIMITIVE_LINESTRIP = 3,
	PRIMITIVE_TRIANGLELIST = 4,
	PRIMITIVE_TRIANGLESTRIP = 5,
};

class DynamicPrimitive
{
public:
	static DynamicPrimitive& GetInstance();
	void ResetSize(int nWidth, int nHeight);
public:
	DynamicPrimitive(int initialBufferCapacity = 32, int maxBufferSizePerPrimitive = 32768);
	~DynamicPrimitive();
	void CreateVertexBuffer(int nType, int nSize);
	void CreateIndexBuffer(int nType, int nSize);
	void ShowRect(int x1, int y1, int x2, int y2, const XMFLOAT4& color, bool bDot = false,float dt = 0);

	void ShowBlock(int x1, int y1, int x2, int y2, const XMFLOAT4& color, float dt = 0);

	void DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount,
		VertexPositionColor *pVertexs, const XMMATRIX &model = XMMatrixIdentity(), ID3D11ShaderResourceView*pTexture = NULL);

	void DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount,
		VertexPositionColorTexture *pVertexs, const XMMATRIX &model = XMMatrixIdentity(), ID3D11ShaderResourceView*pTexture = NULL);

	void ShowTexEx(RECT *pDest, RECT *pSrc, int nWidth, int nHeight, ID3D11ShaderResourceView*pTexture);

private:
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer	*m_indexBuffer;
	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
	int m_maxBufferSizePerPrimitive;
	int m_initialBufferCapacity;
	std::shared_ptr<class D3D11RendererMaterial>m_MaterialPtr;

	int					m_nWidth;
	int					m_nHeight;
};

#define g_objSprite DynamicPrimitive::GetInstance()