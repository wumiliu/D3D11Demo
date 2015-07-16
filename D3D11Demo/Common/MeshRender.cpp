#include "MeshRender.h"
#include "DeviceManager.h"
#include "VertexTypes.h"
#include "Texture/DDSTextureLoader.h"

MeshRender::MeshRender()
{
	m_d3dDevice = g_objDeviecManager.GetDevice();
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
}


MeshRender::~MeshRender()
{
}

bool MeshRender::BuildBuffers(const MeshData& mesh, WCHAR*textureFilename)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(VertexPositionNormalTexture) * mesh.vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	m_nVBSize = mesh.vertices.size();
	m_nIBSize = mesh.indices.size();

	std::vector<VertexPositionNormalTexture> vertices(mesh.vertices.size());
	for (UINT i = 0; i < mesh.vertices.size(); ++i)
	{
		vertices[i].position = mesh.vertices[i].pos;
		vertices[i].normal = mesh.vertices[i].normal;
		vertices[i].textureCoordinate = mesh.vertices[i].tex;
	}
	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = &vertices[0];
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&desc, &vData, &m_VB)))
	{
		MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", MB_OK);
		return false;
	}

	D3D11_BUFFER_DESC iDesc = { 0 };
	iDesc.ByteWidth = sizeof(UINT) * mesh.indices.size();
	iDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iDesc.Usage = D3D11_USAGE_DEFAULT;
	std::vector<UINT> indices(mesh.indices.size());
	for (UINT i = 0; i < mesh.indices.size(); ++i)
	{
		indices[i] = mesh.indices[i];
	}
	D3D11_SUBRESOURCE_DATA iData;
	iData.pSysMem = &indices[0];
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;
	if (FAILED(m_d3dDevice->CreateBuffer(&iDesc, &iData, &m_IB)))
	{
		MessageBox(NULL, L"Create Index Buffer failed!", L"Error", MB_OK);
		return false;
	}

	ID3D11Texture2D* pTexture2D;
	HRESULT ret = DirectX::CreateDDSTextureFromFile(
		m_d3dDevice,
		textureFilename,
		(ID3D11Resource**)&pTexture2D,
		&m_ShaderResourceView
		);
	return true;
}

void MeshRender::SetGPUBuffers()
{
	unsigned int stride;
	unsigned int offset;
	stride = sizeof(VertexPositionNormalTexture);
	offset = 0;

	//1ÊäÈë²¼¾Ö½×¶Î
	m_deviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	m_deviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void MeshRender::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_deviceContext->DrawIndexed(m_nIBSize, 0, 0);
}

void MeshRender::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void MeshRender::GetPosition(float& x, float& y, float& z)
{
	x = m_positionX;
	y = m_positionY;
	z = m_positionZ;
	return;
}
