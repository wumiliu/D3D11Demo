#pragma once
#include "D3D11App.h"
#include "HpD3D9Type.h"
#include "nanovg/demo.h"
class NanovgDemo : public D3D11App
{
public:
	NanovgDemo(HINSTANCE hInstance, int nWidth = 1024, int nHeight = 600);
	~NanovgDemo();
	virtual void UpdateScene(float dt);
	virtual void DrawScene();
protected:
	void InitResource();
	struct NVGcontext* NVGcontextPtr;
	DemoData data;
};

