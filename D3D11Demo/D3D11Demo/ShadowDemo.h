#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"

struct LightClass
{
	void SetLookAt(float, float, float);
	void GenerateViewMatrix();
	void GenerateProjectionMatrix(float screenDepth, float screenNear);

	void GetViewMatrix(Matrix&);
	void GetProjectionMatrix(Matrix&);
	Vector4 GetAmbientColor();
	Vector4 GetDiffuseColor();
	Vector3 GetPosition();


	Vector4 m_ambientColor;
	Vector4 m_diffuseColor;
	Vector3 m_position;
private:
	Vector3 m_lookAt;
	Matrix m_viewMatrix;
	Matrix m_projectionMatrix;
};


class ShadowDemo : public D3D11App
{
public:
	ShadowDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~ShadowDemo();

	virtual bool Init();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	void RenderSceneToTexture();
	void Render(Matrix worldMatrix, Matrix viewMatrix,
		Matrix projectionMatrix);

	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);
	virtual void OnMouseWheel(short zDelta, int x, int y);
protected:
	void InitResource();

private:

	std::shared_ptr<class DX11RTTexture> DX11RTTexturePtr;
	std::shared_ptr<class MeshRender> m_CubeModel;
	std::shared_ptr<class MeshRender> m_GroundModel;
	std::shared_ptr<class MeshRender> m_SphereModel;

	LightClass m_Light;
	bool bMouseDown;
	MousePos mouseLast;
};

