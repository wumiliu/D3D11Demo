#pragma once
#include "HpD3D9Type.h"
#include "D3DShader.h"
#include "DeviceManager.h"
struct MatrixBufferShader
{
public:
	Matrix world;
	Matrix view;
	Matrix projection;
};

struct GO
{
public:
	Vector4 MyColor;
	Vector4 MyColor1;
	Vector4 MyColor2;
};
struct RendererMaterialDesc
{
	RendererMaterialDesc()
	{
		geometryShaderPath = NULL;
		hullShaderPath = NULL;
		domainShaderPath = NULL;
		vertexShaderPath = NULL;
		pixelShaderPath = NULL;
	}
	const char                     *geometryShaderPath;
	const char                     *hullShaderPath;
	const char                     *domainShaderPath;
	const char                     *vertexShaderPath;
	const char                     *pixelShaderPath;
	std::vector<std::string>        vecPass;
};

class D3D11RendererMaterial
{
public:
	D3D11RendererMaterial(const RendererMaterialDesc& desc);
	~D3D11RendererMaterial();

	void setShaders(uint32 i = 0);

	void SetMatrix(Matrix world, Matrix view, Matrix proj);
	//VS ConstantBuffers不能分开设置，需要在一个map 里面把所有的参数设置完
	void VSSetConstantBuffers(const char* name, void* pBuffer);
	//设置纹理资源等
	void PSSetShaderResources(const char* name, ID3D11ShaderResourceView** ppShaderResourceViews);
	//设置像素着色器常量
	//VS ConstantBuffers可以分开设置
	void PSSetConstantBuffers(const char* name, void* pBuffer);


	ID3D11VertexShader*   getVS() const { return m_vertexShader.m_pShader; }
	ID3D11PixelShader*    getPS(int i = 0) const { return m_pixelShader[i]; }

	ID3D11GeometryShader* getGS() const { return m_geometryShader; }
	ID3D11HullShader*     getHS() const { return m_hullShader; }
	ID3D11DomainShader*   getDS() const { return m_domainShader; }

	ID3DBlob* getVSBlob() { return vertexshaderBuffer; }

	template< class VertexTypes >
	ID3D11InputLayout* getLayout();// { return m_pInputLayout; }
	D3D11Shader<ID3D11VertexShader> m_vertexShader;
protected:
	void loadShaders(const RendererMaterialDesc& desc);
private:
	std::vector<ID3D11PixelShader*>m_pixelShader;
	ID3D11GeometryShader*        m_geometryShader;
	ID3D11HullShader*            m_hullShader;
	ID3D11DomainShader*          m_domainShader;

	ID3DBlob* vertexshaderBuffer;
	ID3D11InputLayout * m_pInputLayout;

	D3D11Shader<ID3D11PixelShader> m_Shader1;
};


template< class VertexTypes >
ID3D11InputLayout* D3D11RendererMaterial::getLayout()
{
	if (!m_pInputLayout)
	{
		LayoutVector vecLayout;
		for (int i = 0; i < VertexTypes::InputElementCount; ++i)
		{
			vecLayout.push_back(VertexTypes::InputElements[i]);
		}
		g_objDeviecManager.CreateInputLayout(vecLayout, vertexshaderBuffer, &m_pInputLayout);
	}
	return m_pInputLayout;
}

