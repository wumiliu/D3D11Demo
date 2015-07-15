#pragma once
#include "D3D11App.h"
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

class BaseDemo : public D3D11App
{
public:
	BaseDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~BaseDemo();
	virtual bool Init();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	virtual void OnResize();
	void ShowBlock(int x1, int y1, int x2, int y2, const XMFLOAT4& color, float dt = 0);

	void DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount,
		VertexPositionColor *pVertexs, const XMMATRIX &model = XMMatrixIdentity(), ID3D11ShaderResourceView*pTexture = NULL);

	void DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount,
		VertexPositionColorTexture *pVertexs, const XMMATRIX &model = XMMatrixIdentity(), ID3D11ShaderResourceView*pTexture = NULL);



protected:
	void InitResource();
private:
	ID3D11Buffer	*m_vertexBuffer;
	ID3D11Buffer	*m_indexBuffer;
	ID3D11ShaderResourceView* srv;
	std::shared_ptr<class AnimationCurve> m_Curve;
};

