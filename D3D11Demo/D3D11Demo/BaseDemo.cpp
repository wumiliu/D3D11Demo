#include "BaseDemo.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "VertexTypes.h"
#include "Shader/ShaderManager.h"
#include "Cure/Curve.h"
#include "Cure/spline.h"
BaseDemo::BaseDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;

	//m_Curve = AnimationCurve::Cubic(-507 + 20, 280, 507 - 20, -280);
	m_Curve = AnimationCurve::Cubic(0, 0, mClientWidth, mClientHeight);
	m_Curve->AddKey(300, 400);
	m_Curve->AddKey(600, 200);
	//m_Curve->AddKey(-400, -100);
	//m_Curve->AddKey(100, 200);
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



# define NDIM 1
# define NDATA 3
	int left;
	double xdata[NDATA];
	double xval;
	double ydata[NDIM*NDATA];
	double yval[NDIM];
	//y = -(x)**2
	xdata[0] = -507;
	ydata[0] = 300;
	xdata[1] = 0;
	ydata[1] = 0;
	xdata[2] = 107;
	ydata[2] = 300;
	xval = -200.0;
	DirectX::VertexPositionColor vertices[6000];

	int i = 0;
	for (double x = -mClientWidth / 2; (x < mClientHeight / 2) && (i < 1445); x++)
	{
		xval = x;
		left = i;
		if (NDATA - 2 < left)
		{
			left = NDATA - 2;
		}
		if (left < 1)
		{
			left = 1;
		}
		double yval1 = m_Curve->Evaluate(xval);
		parabola_val2(NDIM, NDATA, xdata, ydata, left, (double)xval, yval);
		double xtmp = xval;// xval / 28 * 10.f;
		double ytmp = yval[0];// yval[0] / nSceneHeight / 28 * 10.0f;
		ytmp = yval1;
		xtmp = xval + mClientWidth / 2;
		ytmp = mClientHeight / 2 - ytmp;

		vertices[i] = DirectX::VertexPositionColor(XMFLOAT3((float)xtmp, ytmp, 0.0f), XMFLOAT4(0.5f, 1.0f, 0.5f, 1));
		i++;
	}
	XMMATRIX toTexSpace(
		0.5f * mClientWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f * mClientHeight, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f * mClientWidth, 0.5f * mClientHeight, 0.0f, 1.0f);

	int tmpX = (float)mClientWidth*1.0f / 2.0f;
	int tmpY = (float)mClientHeight*1.0f / 2.0f;
	int x = vertices[0].position.x;
	int y = vertices[0].position.y;
	tmpX -= x;
	tmpY -= y;

	XMMATRIX m_OrthotMat = XMMatrixOrthographicLH((float)mClientWidth, (float)mClientHeight, 0.0f, 1.0f);
	
	DrawPrimitiveUP(PRIMITIVE_LINESTRIP, mClientWidth, vertices, toTexSpace);


	struct myPos
	{
		float x;
		float y;
	};
	vector<myPos> vertices1;
	for (int i = 0; i < mClientWidth; ++i)
	{
		float yval1 = m_Curve->Evaluate(i);
		myPos pos;
		pos.x = i;
		pos.y = yval1;
		vertices1.push_back(pos);
	}
	static int kkkkkk = 0;
	if (kkkkkk > mClientWidth - 1)
	{
		kkkkkk = 0;
	}
	kkkkkk++;
	int xPos = vertices1[kkkkkk].x;
	int yPos = vertices1[kkkkkk].y;

	ShowBlock(xPos - 5, yPos - 5, xPos + 5, yPos + 5, XMFLOAT4(DirectX::Colors::Blue));

	for (int i = 0; i < m_Curve->Size(); ++i)
	{
		int xPos = m_Curve->Key(i) + mClientWidth / 2;
		int yPos = mClientHeight / 2 - m_Curve->Value(i);
		ShowBlock(xPos - 5, yPos - 5, xPos + 5, yPos + 5, XMFLOAT4(DirectX::Colors::Blue));
	}

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

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC; //动态缓存
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColorTexture) * 10000;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU 可写
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	HRESULT result = S_FALSE;
	result = m_d3dDevice->CreateBuffer(&vertexBufferDesc, NULL, &m_vertexBuffer);

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
	for (int i = 0; i < PrimitiveCount; ++i)
	{
		VertexPositionColorTexture& pPoint = vertex[i];
		pPoint.color = pVertexs[i].color;
		pPoint.position = pVertexs[i].position;
	}
	DrawPrimitiveUP(PrimitiveType, PrimitiveCount, &vertex[0], model, pTexture);
}

