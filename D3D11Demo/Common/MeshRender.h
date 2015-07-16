#pragma once
#include "HpD3D9Type.h"
#include "GeometryGens.h"
using namespace GeoGen;

class MeshRender
{
public:
	MeshRender();
	~MeshRender();
	bool BuildBuffers(const GeoGen::MeshData& mesh, WCHAR*textureFilename);
	void SetGPUBuffers();
	void Render();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);
	int GetIndexCount(){ return m_nIBSize; }
	ID3D11ShaderResourceView* GetTexture(){ return m_ShaderResourceView; }

private:

	ID3D11Buffer	*m_VB;
	ID3D11Buffer	*m_IB;
	ID3D11Device			*m_d3dDevice;				//D3D11设备
	ID3D11DeviceContext		*m_deviceContext;			//设备上下文
	int m_nVBSize;
	int m_nIBSize;

	float m_positionX, m_positionY, m_positionZ;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};

