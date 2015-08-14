#pragma once
#include "HpD3D9Type.h"
#include "GeometryGens.h"
#include "VertexTypes.h"
using namespace GeoGen;


struct InstancedData
{
	XMFLOAT4X4 World;
	XMFLOAT4 Color;
};
class D3D11RendererMaterial;
class D3D11RendererMesh
{
public:
	D3D11RendererMesh();
	~D3D11RendererMesh();
	bool BuildBuffers(const GeoGen::MeshData& mesh);
	void         render(D3D11RendererMaterial* pMaterial,uint32 pass = 0);
	void         renderHelp(D3D11RendererMaterial* pMaterial);
	void BuildInstancedBuffer();
	void Pick(DirectX::SimpleMath::Ray ray);
protected:
	void RenderSystem();
private:
	std::vector<InstancedData> mInstancedData;
	std::vector<VertexPositionNormalTexture> vertices;
	std::vector<UINT> indices;
protected:
	ID3D11Buffer	*m_VB;
	ID3D11Buffer	*m_IB;
	ID3D11Buffer* mInstancedBuffer;
	int m_nVBSize;
	int m_nIBSize;
	XMVECTOR vMin;
	XMVECTOR vMax;

	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
	ID3D11ShaderResourceView* m_ShaderResourceView;
	UINT mPickedTriangle;
	ID3D11BlendState *m_pBlendState;
	bool m_bPick;
	Matrix m_WorldMatrix;
};

