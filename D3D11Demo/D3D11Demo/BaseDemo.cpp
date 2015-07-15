#include "BaseDemo.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "VertexTypes.h"
#include "Shader/ShaderManager.h"
BaseDemo::BaseDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;

}

BaseDemo::~BaseDemo()
{

}

bool BaseDemo::Init()
{
	if (!D3D11App::Init())
		return false;
	InitResource();
	return true;
}

void BaseDemo::UpdateScene(float dt)
{

}

void BaseDemo::DrawScene()
{
	SwapChainPtr->Begin();
	ShowBlock(100, 100, 300, 200, { 0, 0, 1, 1 }, mTimer.TotalTime());
	SwapChainPtr->Flip();
}

void BaseDemo::ShowBlock(int x1, int y1, int x2, int y2, const XMFLOAT4& color, float dt /*= 0*/)
{
	float x = (float)x1;
	float y = (float)y1;
	float h = (float)y2 - y1;
	float w =(float) x2 - x1;

	XMFLOAT3 LeftTop = XMFLOAT3(x, y, 0.0f);
	XMFLOAT3 RightTop = XMFLOAT3(x + w, y, 0.0f);

	XMFLOAT3 LeftBottom = XMFLOAT3(x, (y + h), 0.0f);
	XMFLOAT3 RightBottom = XMFLOAT3(x + w, (y + h), 0.0f);
	float tmpX = (float)mClientWidth*1.0f / 2.0f - x;
	float tmpY = (float)mClientHeight*1.0f / 2.0f - y;

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
	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f * mClientWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f * mClientHeight, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f * mClientWidth, 0.5f * mClientHeight, 0.0f, 1.0f);
	XMMATRIX rTmp = XMMatrixTranslation(-tmpX, -tmpY, 0);
	XMMATRIX rz = XMMatrixRotationZ(XM_PI *dt);
	static float kk = 0;
	kk = dt;
	rz = XMMatrixRotationZ(XM_PI *kk);    // 绕Y轴旋转   
	//先平移为原点，绕着原点旋转, 转换到平行投影的中点为（w/2,h/2）坐标系。Y 是颠倒的，在旋转
	rTmp = XMMatrixTranslation(-x, -y, 0.0)* rz * XMMatrixTranslation(x, y, 0.0);

	XMVECTOR pDeterminant;
	//rTmp = XMMatrixInverse(&pDeterminant, rTmp );
	toTexSpace = XMMatrixInverse(&pDeterminant, toTexSpace);
	toTexSpace = rTmp * toTexSpace;


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	m_deviceContext->Map(m_vertexBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mappedResource);
	memcpy(mappedResource.pData, vertices, 6 * sizeof(VertexPositionColorTexture));
	m_deviceContext->Unmap(m_vertexBuffer, 0);

	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexPositionColorTexture);
	offset = 0;
	g_objShaders.BaseShader()->SetShaderParameters(XMMatrixIdentity(), XMMatrixIdentity(), toTexSpace, srv);
	g_objShaders.BaseShader()->RenderShader();
	m_deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	m_deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->Draw(6, 0);
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

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //动态缓存
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColorTexture) * 500;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU 可写
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	HRESULT result = S_FALSE;
	result = m_d3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);

}
