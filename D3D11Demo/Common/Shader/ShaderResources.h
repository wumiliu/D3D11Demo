#pragma once
#include "../HpD3D9Type.h"


struct  ShaderResource
{
	ShaderResource();
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* BoneTransforms;

	void SetLayout();
	void SetVSResource(Matrix world, Matrix view, Matrix proj);
	void SetTextureResource(ID3D11ShaderResourceView* pResourceView);
	void Render();
	void SetBoneTransforms(const XMMATRIX* M, int cnt);
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
};

struct LightShaderResource
{
	LightShaderResource();
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};
	struct LightBufferType
	{
		XMVECTOR diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;  // Added extra padding so structure is a multiple of 16 for CreateBuffer function requirements.
	};
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	void SetLayout();
	void SetVSResource(Matrix world, Matrix view, Matrix proj);
	void SetTextureResource(ID3D11ShaderResourceView* pResourceView);
	void Render();
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
};

class ShaderResources
{

public:   
	static ShaderResources* GetInstancePtr();

	static ShaderResources& GetInstance();

	// Blend states.
	ShaderResource* __cdecl BaseShader() const;
	ShaderResource* __cdecl BaseTextureShader() const;

	
	LightShaderResource* __cdecl LightShader() const;
	ShaderResource* __cdecl SkinShader() const;


private:
	explicit ShaderResources();
	~ShaderResources();
	// Private implementation.
	class Impl;

	std::shared_ptr<Impl> pImpl;

	// Prevent copying.
	ShaderResources(ShaderResources const&);
	ShaderResources& operator= (ShaderResources const&);

};
#define  g_objShader ShaderResources::GetInstance()
