#include "D3D11RendererMesh.h"
#include "VertexTypes.h"
#include "Texture/DDSTextureLoader.h"
#include "DeviceManager.h"
#include "D3D11RendererMaterial.h"
#include "CommonStates.h"


D3D11RendererMesh::D3D11RendererMesh()
{
	m_d3dDevice = g_objDeviecManager.GetDevice();
	m_deviceContext = g_objDeviecManager.GetImmediateContext();
	m_nVBSize = 0;
	m_nIBSize = 0;
	m_ShaderResourceView = NULL;
	m_VB = NULL;
	m_IB = NULL;
	mPickedTriangle = -1;
	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.AlphaToCoverageEnable = FALSE;
	desc.IndependentBlendEnable = FALSE;
	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;

	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HRESULT hr = m_d3dDevice->CreateBlendState(&desc, &m_pBlendState);
}

D3D11RendererMesh::~D3D11RendererMesh()
{
	SAFE_RELEASE(m_VB);
	SAFE_RELEASE(m_IB);
	SAFE_RELEASE(m_ShaderResourceView);
	SAFE_RELEASE(m_pBlendState);
}

bool D3D11RendererMesh::BuildBuffers(const GeoGen::MeshData& mesh)
{
	D3D11_BUFFER_DESC desc = { 0 };
	desc.ByteWidth = sizeof(VertexPositionNormalTexture) * mesh.vertices.size();
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.Usage = D3D11_USAGE_DEFAULT;
	m_nVBSize = mesh.vertices.size();
	m_nIBSize = mesh.indices.size();

	vertices.resize(mesh.vertices.size());
	float Infinity = FLT_MAX;
	XMFLOAT3 vMinf3(+Infinity, +Infinity, +Infinity);
	XMFLOAT3 vMaxf3(-Infinity, -Infinity, -Infinity);
	vMin = XMLoadFloat3(&vMinf3);
	vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < mesh.vertices.size(); ++i)
	{
		vertices[i].position = mesh.vertices[i].pos;
		vertices[i].normal = mesh.vertices[i].normal;
		vertices[i].textureCoordinate = mesh.vertices[i].tex;
		XMVECTOR P = XMLoadFloat3(&vertices[i].position);
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
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
	indices.resize(mesh.indices.size());
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
//	BuildInstancedBuffer();
	return true;
}

void D3D11RendererMesh::render(D3D11RendererMaterial* pMaterial, uint32 pass)
{

	/*D3D11_MAPPED_SUBRESOURCE mappedData;
	m_deviceContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);
	int mVisibleObjectCount = 0;
	for (UINT i = 0; i < mInstancedData.size(); ++i)
	{
		dataView[mVisibleObjectCount++] = mInstancedData[i];
	}

	m_deviceContext->Unmap(mInstancedBuffer, 0);*/

	m_deviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_deviceContext->IASetInputLayout(pMaterial->getLayout<VertexPositionNormalTexture>());

	FLOAT BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };// 0xFFFFFFFF
	//m_deviceContext->OMSetBlendState(m_pBlendState, BlendFactor, 0xFFFFFFFF);
	//1输入布局阶段
	ID3D11Buffer* vbs[2] = { m_VB, mInstancedBuffer };

	UINT stride[2] = { sizeof(VertexPositionNormalTexture), sizeof(InstancedData) };
	UINT offset[2] = { 0, 0 };

	m_deviceContext->IASetVertexBuffers(0, 1, vbs, stride, offset);
	m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
	if (m_nIBSize <= 0)
	{
		m_deviceContext->Draw(m_nIBSize, 0);
	}
	else
	{
		pMaterial->setShaders(pass);
		m_deviceContext->DrawIndexed(m_nIBSize, 0, 0);
	//	m_deviceContext->DrawIndexedInstanced(m_nIBSize, mVisibleObjectCount, 0, 0, 0);
		//m_deviceContext->DrawIndexed(m_nIBSize, 0, 0);

	}

}


void D3D11RendererMesh::renderHelp(D3D11RendererMaterial* pMaterial)
{
	if (mPickedTriangle != -1)
	{
		m_deviceContext->IASetPrimitiveTopology((D3D_PRIMITIVE_TOPOLOGY)D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_deviceContext->IASetInputLayout(pMaterial->getLayout<VertexPositionNormalTexture>());
		unsigned int stride;
		unsigned int offset;
		stride = sizeof(VertexPositionNormalTexture);
		offset = 0;

		//1输入布局阶段
		m_deviceContext->IASetVertexBuffers(0, 1, &m_VB, &stride, &offset);
		m_deviceContext->IASetIndexBuffer(m_IB, DXGI_FORMAT_R32_UINT, 0);
		pMaterial->setShaders(1);
		m_deviceContext->DrawIndexed(3, 3*mPickedTriangle, 0);
	}
}


void D3D11RendererMesh::Pick(DirectX::SimpleMath::Ray ray)
{
	m_bPick = false;
	mPickedTriangle = -1;
	XMFLOAT3 Center;            // Center of the box.
	XMFLOAT3 Extents;           // Distance from the center to each side.
	XMStoreFloat3(&Center, 0.5f*(vMin + vMax));
	XMStoreFloat3(&Extents, 0.5f*(vMax - vMin));
	float tmin = 0.0f;
	DirectX::BoundingBox box(Center,Extents);
	if (ray.Intersects(box, tmin))
	{
		tmin = FLT_MAX;
		for (UINT i = 0; i < indices.size() / 3; ++i)
		{
			// Indices for this triangle.
			UINT i0 = indices[i * 3 + 0];
			UINT i1 = indices[i * 3 + 1];
			UINT i2 = indices[i * 3 + 2];

			// Vertices for this triangle.
			XMVECTOR v0 = XMLoadFloat3(&vertices[i0].position);
			XMVECTOR v1 = XMLoadFloat3(&vertices[i1].position);
			XMVECTOR v2 = XMLoadFloat3(&vertices[i2].position);
			DirectX::SimpleMath::Vector3 tri0(v0);
			DirectX::SimpleMath::Vector3 tri1(v1);
			DirectX::SimpleMath::Vector3 tri2(v2);
			float t = 0.0f;
			if (ray.Intersects(tri0, tri1, tri2, t))
			{
				if (t < tmin)
				{
					tmin = t;
					mPickedTriangle = i;
					m_bPick = true;
				}
			}
		}
	}
}

void D3D11RendererMesh::RenderSystem()
{
	
}

void D3D11RendererMesh::BuildInstancedBuffer()
{
	mInstancedData.resize(2);

	int i;
	float red, green, blue;

	srand((unsigned int)time(NULL));

	// Go through all the models and randomly generate the model color and position.
	for (i = 0; i < mInstancedData.size(); i++)
	{
		// Generate a random color for the model.
		red = (float)rand() / RAND_MAX;
		green = (float)rand() / RAND_MAX;
		blue = (float)rand() / RAND_MAX;

		mInstancedData[i].Color = Vector4(red, green, blue, 1.0f);

		// Generate a random position in front of the viewer for the mode.
		float positionX = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		float positionY = (((float)rand() - (float)rand()) / RAND_MAX) * 10.0f;
		float positionZ = ((((float)rand() - (float)rand()) / RAND_MAX) * 10.0f) + 5.0f;
		mInstancedData[i].World = Matrix::CreateTranslation(positionX, positionY, positionZ);
	}

	
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * mInstancedData.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	(m_d3dDevice->CreateBuffer(&vbd, 0, &mInstancedBuffer));
}
