#pragma once
#include "HpD3D9Type.h"
#include "GeometryGens.h"
#include "VertexTypes.h"
using namespace GeoGen;

class D3D11RendererMaterial;
class D3D11RendererMesh
{
public:
	D3D11RendererMesh();
	~D3D11RendererMesh();
	bool BuildBuffers(const GeoGen::MeshData& mesh);
	void         render(D3D11RendererMaterial* pMaterial);

	void Pick(DirectX::SimpleMath::Ray ray);
private:
	std::vector<VertexPositionNormalTexture> vertices;
	std::vector<UINT> indices;
protected:
	ID3D11Buffer	*m_VB;
	ID3D11Buffer	*m_IB;
	int m_nVBSize;
	int m_nIBSize;
	XMVECTOR vMin;
	XMVECTOR vMax;

	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
	ID3D11ShaderResourceView* m_ShaderResourceView;
	UINT mPickedTriangle;
};

