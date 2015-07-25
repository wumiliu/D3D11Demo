#include "D3DShader.h"
void*  D3D11ConstantBuffer::Map(ID3D11DeviceContext* pDeviceContext)
{
	if (bMap && dataPtr)
	{
		return dataPtr;
	}
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pDeviceContext->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	bMap = true;
	dataPtr = mappedResource.pData;
	return mappedResource.pData;
}

void D3D11ConstantBuffer::Update(ID3D11DeviceContext* pDeviceContext)
{
	if (bMap)
	{
		bMap = false;
		dataPtr = NULL;
		pDeviceContext->Unmap(pConstantBuffer, 0);
	}
}