#pragma once
#include "HpD3D9Type.h"


struct RendererMaterialDesc
{
	const char                     *geometryShaderPath;
	const char                     *hullShaderPath;
	const char                     *domainShaderPath;
	const char                     *vertexShaderPath;
	const char                     *pixelShaderPath;

	std::vector<std::string>        vecPass;

};

struct MatrixBufferShader
{
public:
	Matrix world;
	Matrix view;
	Matrix projection;
};

class D3D11RendererMaterial
{
public:
	D3D11RendererMaterial(const RendererMaterialDesc& desc);
	~D3D11RendererMaterial();

	void setShaders(uint32 i = 0);
	void SetShaderParameters(Matrix world, Matrix view, Matrix proj, ID3D11ShaderResourceView* texture = NULL);
	void PSSetShaderResources(UINT StartSlot, UINT NumViews, ID3D11ShaderResourceView  *ppShaderResourceViews);
	ID3D11VertexShader*   getVS() const { return m_vertexShader; }
	ID3D11PixelShader*    getPS(int i = 0) const { return m_pixelShader[i]; }

	ID3D11GeometryShader* getGS() const { return m_geometryShader; }
	ID3D11HullShader*     getHS() const { return m_hullShader; }
	ID3D11DomainShader*   getDS() const { return m_domainShader; }

	ID3DBlob* getVSBlob() { return vertexshaderBuffer; }

	template< class VertexTypes >
	ID3D11InputLayout* getLayout();// { return m_pInputLayout; }

protected:
	void loadShaders(const RendererMaterialDesc& desc);
private:
	ID3D11VertexShader*  m_vertexShader;
	std::vector<ID3D11PixelShader*>m_pixelShader;
	ID3D11GeometryShader*        m_geometryShader;
	ID3D11HullShader*            m_hullShader;
	ID3D11DomainShader*          m_domainShader;

	ID3DBlob* vertexshaderBuffer;
	ID3D11InputLayout * m_pInputLayout;


	ID3D11Buffer* m_matrixBuffer;
};


template< class VertexTypes >
ID3D11InputLayout* D3D11RendererMaterial::getLayout()
{
	if (!m_pInputLayout)
	{
		LayoutVector vecLayout;
		for (int i = 0; i < VertexTypes::InputElementCount;++i)
		{
			vecLayout.push_back(VertexTypes::InputElements[i]);
		}
		g_objDeviecManager.CreateInputLayout(vecLayout, vertexshaderBuffer, &m_pInputLayout);
	}
	return m_pInputLayout;
}

