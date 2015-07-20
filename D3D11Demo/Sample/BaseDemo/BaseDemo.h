#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "VertexTypes.h"

class BaseDemo : public D3D11App
{
public:
	BaseDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~BaseDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();

protected:
	void InitResource();

};

