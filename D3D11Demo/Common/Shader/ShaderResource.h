#pragma once
#include "../HpD3D9Type.h"
struct MatrixBufferShader
{
public:
	Matrix world;
	Matrix view;
	Matrix projection;
};
struct LightBufferShader
{
	XMVECTOR diffuseColor;
	XMFLOAT3 lightDirection;
	float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
};

struct IShaderResource
{
public:
	IShaderResource();
	~IShaderResource();
	virtual void InitBuffer();
	void PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView  *ppShaderResourceViews);
	void SetShaderParameters(Matrix world, Matrix view, Matrix proj, ID3D11ShaderResourceView* texture = NULL);
	void RenderShader();



	template< class BufferShader >
	HRESULT CreateBuffer(ID3D11Buffer **ppBuffer);
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
};

struct LightShader : IShaderResource
{
public:
	LightShader();
	~LightShader();
	void SetLightBuffer(Vector3 lightDirection, Vector4 diffuseColor);
	ID3D11Buffer* m_lightBuffer;
	virtual void InitBuffer();
};


struct ShadowShader : IShaderResource
{
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct LightBufferType
	{
		Vector4 ambientColor;
		Vector4 diffuseColor;
	};

	struct LightBufferType2
	{
		Vector3 lightPosition;
		float padding;
	};
public:
	ShadowShader();
	virtual void InitBuffer();

	void SetLightBuffer(Vector3 lightPosition,
		Vector4 ambientColor, Vector4 diffuseColor);
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
	void SetShaderParameters(Matrix world, Matrix view, Matrix proj, Matrix lightViewMatrix, Matrix lightProjectionMatrix,
		ID3D11ShaderResourceView* texture,
		ID3D11ShaderResourceView* depthMapTexture);
	void Render(int indexCount);

};

