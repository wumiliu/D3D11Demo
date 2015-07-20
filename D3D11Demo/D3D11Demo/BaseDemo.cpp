#include "BaseDemo.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "VertexTypes.h"
#include "Shader/ShaderManager.h"
#include "Cure/Curve.h"
#include "Cure/spline.h"
#include "d3dUtil.h"
#include "p3types.h"
#include "CommonStates.h"
#include "Texture/DirectXHelpers.h"
BaseDemo::BaseDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;

	m_Curve = AnimationCurve::Cubic(0, 0, mClientWidth, mClientHeight);
	m_Curve->AddKey(300, 400);
	m_Curve->AddKey(600, 200);
	m_bMove = false;
}

BaseDemo::~BaseDemo()
{
	SAFE_RELEASE(srv);
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
}


void BaseDemo::UpdateScene(float dt)
{

}

void BaseDemo::DrawScene()
{
	SwapChainPtr->Begin();
	ShowBlock(100, 100, 300, 200, { 0, 0, 1, 1 }, mTimer.TotalTime());
	SwapChainPtr->Flip();
	return;
	vector<VertexPositionColor> vertices;
	vertices.resize(mClientWidth);
	for (int xval = 0; xval < mClientWidth; ++xval)
	{
		float yval = (float)m_Curve->Evaluate(xval);
		vertices[xval] = DirectX::VertexPositionColor(XMFLOAT3((float)xval, yval, 0.0f), XMFLOAT4(0.5f, 1.0f, 0.5f, 1));
	}
	XMMATRIX toTexSpace = GetShow2DMatrix(mClientWidth, mClientHeight);
	DrawPrimitiveUP(PRIMITIVE_LINESTRIP, mClientWidth, &vertices[0], toTexSpace);

	static int kkkkkk = 0;
	if (kkkkkk >= mClientWidth - 1)
	{
		kkkkkk = 0;
	}
	else
	{
		kkkkkk++;
	}
	int xPos = (int)vertices[kkkkkk].position.x;
	int yPos = (int)vertices[kkkkkk].position.y;

	ShowBlock(xPos - 5, yPos - 5, xPos + 5, yPos + 5, XMFLOAT4(DirectX::Colors::Blue));

	for (int i = 0; i < m_Curve->Size(); ++i)
	{
		int xPos = (int)m_Curve->Key(i);// +mClientWidth / 2;
		int yPos = (int)m_Curve->Value(i);
		ShowBlock(xPos - 5, yPos - 5, xPos + 5, yPos + 5, XMFLOAT4(DirectX::Colors::Red));
	}

	SwapChainPtr->Flip();
}

void BaseDemo::ShowBlock(int x1, int y1, int x2, int y2, const XMFLOAT4& color, float dt /*= 0*/)
{
	float x = (float)x1;
	float y = (float)y1;
	float h = (float)y2 - y1;
	float w = (float)x2 - x1;

	XMFLOAT3 LeftTop = XMFLOAT3(x, y, 0.0f);
	XMFLOAT3 RightTop = XMFLOAT3(x + w, y, 0.0f);

	XMFLOAT3 LeftBottom = XMFLOAT3(x, (y + h), 0.0f);
	XMFLOAT3 RightBottom = XMFLOAT3(x + w, (y + h), 0.0f);


	VertexPositionColorTexture vertices[] =
	{
		//正面的四个点
		{ LeftBottom, color, XMFLOAT2(0, 1) },
		{ LeftTop, color, XMFLOAT2(0, 0) },
		{ RightTop, color, XMFLOAT2(1, 0) },
		{ LeftBottom, color, XMFLOAT2(0, 1) },
		{ RightTop, color, XMFLOAT2(1, 0) },
		{ RightBottom, color, XMFLOAT2(1, 1) }
	};
	XMMATRIX rz = XMMatrixRotationZ(XM_PI *dt);
	//先平移为原点，绕着原点旋转, 转换到平行投影的中点为（w/2,h/2）坐标系。Y 是颠倒的，在旋转
	XMMATRIX rTmp = XMMatrixTranslation(-x, -y, 0.0)* rz * XMMatrixTranslation(x, y, 0.0);

	XMMATRIX toTexSpace = GetShow2DMatrix(mClientWidth, mClientHeight);
	toTexSpace = rTmp * toTexSpace;
	DrawPrimitiveUP(PRIMITIVE_TRIANGLELIST, 6, vertices, toTexSpace);
}

void BaseDemo::OnResize()
{
	D3D11App::OnResize();
}

void BaseDemo::InitResource()
{
	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		L"1.dds",
		(ID3D11Resource**)&pTexture2D,
		&srv
		);
	SAFE_RELEASE(pTexture2D);
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //动态缓存
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColorTexture) * 10000;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU 可写
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	HRESULT result = S_FALSE;
	result = m_d3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);

	SetDebugObjectName(m_vertexBuffer, "BaseDemo:m_vertexBuffer");

}
void BaseDemo::DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount, VertexPositionColorTexture *pVertexs, const XMMATRIX &model /*= XMMatrixIdentity()*/, ID3D11ShaderResourceView*pTexture /*= NULL*/)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_deviceContext->Map(m_vertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource);
	//	Update the vertex buffer here.
	int nSize = PrimitiveCount;
	if (nSize > 10000)
	{
		nSize = 10000;
	}
	memcpy(mappedResource.pData, pVertexs, nSize * sizeof(VertexPositionColorTexture));
	//	Reenable GPU access to the vertex buffer data.
	m_deviceContext->Unmap(m_vertexBuffer, 0);

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexPositionColorTexture);
	offset = 0;



	//1输入布局阶段
	g_objShaders.BaseShader()->SetShaderParameters(XMMatrixIdentity(), XMMatrixIdentity(), model, pTexture);
	g_objShaders.BaseShader()->RenderShader();

	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	m_deviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)PrimitiveType);


	m_deviceContext->Draw(PrimitiveCount, 0);
}

void BaseDemo::DrawPrimitiveUP(PrimitiveType PrimitiveType, unsigned int PrimitiveCount, VertexPositionColor *pVertexs, const XMMATRIX &model /*= XMMatrixIdentity()*/, ID3D11ShaderResourceView*pTexture /*= NULL*/)
{
	std::vector<VertexPositionColorTexture> vertex;
	vertex.resize(PrimitiveCount);
	for (uint32 i = 0; i < PrimitiveCount; ++i)
	{
		VertexPositionColorTexture& pPoint = vertex[i];
		pPoint.color = pVertexs[i].color;
		pPoint.position = pVertexs[i].position;
	}
	DrawPrimitiveUP(PrimitiveType, PrimitiveCount, &vertex[0], model, pTexture);
}

void BaseDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	for (int i = 0; i < m_Curve->Size(); ++i)
	{
		int xPos = (int)m_Curve->Key(i);
		int yPos = (int)m_Curve->Value(i);
		CMyRect _rect;
		_rect.left = xPos - 5;
		_rect.top = yPos - 5;
		_rect.right = _rect.left + 10;
		_rect.bottom = _rect.top + 10;
		P3_POS _point;
		_point.x = x;
		_point.y = y;
		if (_rect.PtInRect(_point))
		{
			std::cout << "OK" << std::endl;
			m_bMove = true;
			m_index = i;
			return;
		}
	}
}

void BaseDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_bMove = false;
}

void BaseDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (m_bMove)
	{
		m_Curve->RemoveKey(m_index);
		int nPosX = x;
		int nPosY = y;
		m_Curve->AddKey(nPosX, nPosY);
	}
}

void BaseDemo::OnLButtonDblClk(WPARAM btnState, int x, int y)
{
	if (m_bMove)
	{
		return;
	}
	m_Curve->AddKey(x, y);
}

