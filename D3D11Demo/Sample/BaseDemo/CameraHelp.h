#pragma once
#include "HpD3D9Type.h"
class CameraHelp
{
public:
	CameraHelp();
	~CameraHelp();

	bool CheckPoint(float x, float y, float z);
	bool CheckCube(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckSphere(float xCenter, float yCenter, float zCenter, float radius);
	bool CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize);

	//º∆À„ ”◊∂ÃÂ
	void ConstructFrustum(Matrix viewMatrix, Matrix projectionMatrix);

private:
	Plane m_planes[6];

};

