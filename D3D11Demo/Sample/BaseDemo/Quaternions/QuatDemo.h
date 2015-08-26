#pragma once

#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "VertexTypes.h"
#include "AnimationHelper.h"
class QuatDemo : public D3D11App
{
public:
	QuatDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~QuatDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
protected:
	void InitResource();
	void RenderSystemAxis();
	
private:

	std::shared_ptr<class D3D11RendererMesh> m_MeshModel;
	std::shared_ptr<class D3D11RendererMaterial> m_Material;
	std::shared_ptr<class D3D11RendererMesh> m_FloorModel;
	Vector3 m_Pos;
	Matrix m_Rotate;
	float mAnimTimePos;
	BoneAnimation mSkullAnimation;
	XMFLOAT4X4 mSkullWorld;
};