#pragma once
#include "HpD3D9Type.h"
#include "D3D11App.h"
class ReflectionApp :public D3D11App
{
public:
	ReflectionApp(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);

	~ReflectionApp();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	void RenderSceneToTexture();
protected:
	virtual void InitResource();
	std::shared_ptr<class D3D11RendererMesh> m_MeshModel;
	std::shared_ptr<class D3D11RendererMaterial> m_Material;
	std::shared_ptr<class DX11RTTexture> DX11RTTexturePtr;
	ID3D11ShaderResourceView* srv;
};

