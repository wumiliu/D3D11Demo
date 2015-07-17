#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "VertexTypes.h"

class LightDemo : public D3D11App
{
public:
	LightDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~LightDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
protected:
	void InitResource();
	std::shared_ptr<class MeshRender> m_CubeModel;
	std::shared_ptr<class MeshRender> m_GroundModel;
	std::shared_ptr<class MeshRender> m_SphereModel;
};

