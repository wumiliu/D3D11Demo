#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "VertexTypes.h"

#include "Camera/CameraComponent.h"

/************************************************************************/
/* 简单的视锥体裁剪                                                                     */
/************************************************************************/

class BaseDemo : public D3D11App
{
	struct ModelInfoType
	{
		Vector4 color;
		float positionX, positionY, positionZ;
	};

public:
	BaseDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~BaseDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	virtual void OnMouseMove(WPARAM btnState, int x, int y);

protected:
	void InitResource();
	void InitPos(int nCount = 26);
private:
	std::vector<ModelInfoType> vecModels;
	std::shared_ptr<class D3D11RendererMesh> m_MeshModel;
	std::shared_ptr<class D3D11RendererMaterial> m_Material;
	CameraComponent cameraComponent;
	ID3D11ShaderResourceView* srv;
};

