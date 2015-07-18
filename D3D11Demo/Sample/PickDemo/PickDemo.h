#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "VertexTypes.h"

class PickDemo : public D3D11App
{
public:
	PickDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~PickDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	void Pick(int x, int y);
protected:
	void InitResource();
	void ResetState();
	std::shared_ptr<class D3D11RendererMesh> m_CubeModel;
	std::shared_ptr<class D3D11RendererMaterial> m_Material;
	ID3D11ShaderResourceView* srv;

};

