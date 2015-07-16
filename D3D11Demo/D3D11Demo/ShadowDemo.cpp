#include "ShadowDemo.h"
#include "SwapChain.h"
#include "Texture/DDSTextureLoader.h"
#include "VertexTypes.h"
#include "Shader/ShaderManager.h"
#include "modelclass.h"
#include "Texture/DX11RTTexture.h"
#include "DeviceManager.h"
#include "TrackballCameraController.h"
#include "MeshRender.h"


const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 1.0f;

void LightClass::SetLookAt(float x, float y, float z)
{
	m_lookAt.x = x;
	m_lookAt.y = y;
	m_lookAt.z = z;
}

void LightClass::GenerateViewMatrix()
{
	Vector3 up;
	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Create the view matrix from the three vectors.
	m_viewMatrix = Matrix::CreateLookAtLh(m_position, m_lookAt, up);// XMMatrixLookAtLH(&m_position, &m_lookAt, &up);

	return;
}

void LightClass::GenerateProjectionMatrix(float screenDepth, float screenNear)
{
	float fieldOfView, screenAspect;


	// Setup field of view and screen aspect for a square light source.
	fieldOfView = (float)XM_PI / 2.0f;
	screenAspect = 1.0f;

	// Create the projection matrix for the light.
	m_projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	return;
}

void LightClass::GetViewMatrix(Matrix& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void LightClass::GetProjectionMatrix(Matrix& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}
Vector4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


Vector4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


Vector3 LightClass::GetPosition()
{
	return m_position;
}

ShadowDemo::ShadowDemo(HINSTANCE hInstance, int nWidth /*= 1024*/, int nHeight /*= 600*/)
	: D3D11App(hInstance)
{
	mClientWidth = nWidth;
	mClientHeight = nHeight;
	// Initialize the light object.
	m_Light.m_ambientColor = Vector4(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light.m_diffuseColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.SetLookAt(0.0f, 0.0f, 0.0f);
	m_Light.GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	bMouseDown = false;
}

ShadowDemo::~ShadowDemo()
{

}

bool ShadowDemo::Init()
{
	if (!D3D11App::Init())
		return false;
	InitResource();
	return true;
}

void ShadowDemo::InitResource()
{
	float AspectHByW = (float)mClientWidth / (float)mClientHeight;

	g_objTrackballCameraController.ProjParams(DirectX::XM_PI*0.25f, AspectHByW, 1.0f, 1000.0f);

	DX11RTTexturePtr = std::make_shared<DX11RTTexture>();
	m_CubeModel = std::make_shared<MeshRender>();
	m_GroundModel = std::make_shared<MeshRender>();
	m_SphereModel = std::make_shared<MeshRender>();
	MeshData meshData;
	GeoGen::CreateBox(2, 2, 2, meshData);
	bool bBuild = m_CubeModel->BuildBuffers(meshData, L"data\\wall01.dds");
	m_CubeModel->SetPosition(-2.0f, 2.0f, 0.0f);
	GeoGen::CreateSphere(1, 50, 50, meshData);
	bBuild = m_SphereModel->BuildBuffers(meshData, L"data\\ice.dds");
	m_SphereModel->SetPosition(2.0f, 2.0f, 0.0f);
	GeoGen::CreateGrid(10, 10, 1,1, meshData);
	bBuild = m_GroundModel->BuildBuffers(meshData, L"data\\metal001.dds");
	m_GroundModel->SetPosition(0.0f, 1.0f, 0.0f);
	
	DX11RTTexturePtr->Create(mClientWidth, mClientHeight);
}

void ShadowDemo::UpdateScene(float dt)
{
	static float lightPositionX = -5.0f;
	// Update the position of the light each frame.
	lightPositionX += 0.05f;
	if (lightPositionX > 5.0f)
	{
		lightPositionX = -5.0f;
	}
	// Update the position of the light.
	m_Light.m_position = Vector3(lightPositionX, 8.0f, -5.0f);
	printf("%f\n",dt);
}

void ShadowDemo::DrawScene()
{
	Matrix worldMatrix;
	Matrix viewMatrix;
	Matrix projectionMatrix;
	viewMatrix = g_objTrackballCameraController.View();
	projectionMatrix = g_objTrackballCameraController.Proj();
	SwapChainPtr->Begin();
	RenderSceneToTexture();
	Render(worldMatrix, viewMatrix, projectionMatrix);
	SwapChainPtr->Flip();
}

void ShadowDemo::RenderSceneToTexture()
{
	if(DX11RTTexturePtr == NULL)
	{
		return;
	}
	DX11RTTexturePtr->Begin();

	Matrix worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	// Generate the light view matrix based on the light's position.
	m_Light.GenerateViewMatrix();

	// Get the view and orthographic matrices from the light object.
	m_Light.GetViewMatrix(lightViewMatrix);
	m_Light.GetProjectionMatrix(lightProjectionMatrix);

	float posX, posY, posZ;
	// Setup the translation matrix for the cube model.
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_CubeModel->SetGPUBuffers();
	g_objShaders.DepthShader()->SetShaderParameters(worldMatrix, lightViewMatrix, lightProjectionMatrix);
	g_objShaders.DepthShader()->RenderShader();
	m_CubeModel->Render();


	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_SphereModel->SetGPUBuffers();
	g_objShaders.DepthShader()->SetShaderParameters(worldMatrix, lightViewMatrix, lightProjectionMatrix);
	g_objShaders.DepthShader()->RenderShader();
	m_SphereModel->Render();


	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_GroundModel->SetGPUBuffers();
	g_objShaders.DepthShader()->SetShaderParameters(worldMatrix, lightViewMatrix, lightProjectionMatrix);
	g_objShaders.DepthShader()->RenderShader();
	m_GroundModel->Render();


	DX11RTTexturePtr->End();

}

void ShadowDemo::Render(Matrix worldMatrix, Matrix viewMatrix, Matrix projectionMatrix)
{
	float posX = 0.0f;
	float posY = 2.0f;
	float posZ = -10.0f;

	float rotX = 0.0f;
	float rotY = 0.0f;
	float rotZ = 0.0f;


	Matrix translateMatrix;
	Matrix lightViewMatrix, lightProjectionMatrix;

	// Generate the light view matrix based on the light's position.
	m_Light.GenerateViewMatrix();
	// Get the light's view and projection matrices from the light object.
	m_Light.GetViewMatrix(lightViewMatrix);
	m_Light.GetProjectionMatrix(lightProjectionMatrix);


	// Setup the translation matrix for the cube model.
	m_CubeModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_CubeModel->SetGPUBuffers();
	g_objShaders.BaseShadowShader()->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_CubeModel->GetTexture(), DX11RTTexturePtr->GetRTView());
	g_objShaders.BaseShadowShader()->SetLightBuffer(m_Light.GetPosition(), m_Light.GetAmbientColor(), m_Light.GetDiffuseColor());
	g_objShaders.BaseShadowShader()->RenderShader();
	g_objShaders.BaseShadowShader()->Render(m_CubeModel->GetIndexCount());


	m_SphereModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_SphereModel->SetGPUBuffers();
	g_objShaders.BaseShadowShader()->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_SphereModel->GetTexture(), DX11RTTexturePtr->GetRTView());
	g_objShaders.BaseShadowShader()->SetLightBuffer(m_Light.GetPosition(), m_Light.GetAmbientColor(), m_Light.GetDiffuseColor());
	g_objShaders.BaseShadowShader()->RenderShader();
	g_objShaders.BaseShadowShader()->Render(m_SphereModel->GetIndexCount());


	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix.Translation(Vector3(posX, posY, posZ));
	m_GroundModel->SetGPUBuffers();
	g_objShaders.BaseShadowShader()->SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
		lightProjectionMatrix, m_GroundModel->GetTexture(), DX11RTTexturePtr->GetRTView());
	g_objShaders.BaseShadowShader()->SetLightBuffer(m_Light.GetPosition(), m_Light.GetAmbientColor(), m_Light.GetDiffuseColor());
	g_objShaders.BaseShadowShader()->RenderShader();
	g_objShaders.BaseShadowShader()->Render(m_GroundModel->GetIndexCount());
}

void ShadowDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	bMouseDown = true;
	mouseLast.X = x;
	mouseLast.Y = y;
}

void ShadowDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	bMouseDown = false;
	mouseLast.X = x;
	mouseLast.Y = y;
}

void ShadowDemo::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (bMouseDown)
	{
		MousePos _tmp(x, y);
		x -= mouseLast.X;
		y -= mouseLast.Y;
		mouseLast.X = _tmp.X;
		mouseLast.Y = _tmp.Y;
		g_objTrackballCameraController.Rotate((float)x, (float)y);
		printf("%d   %d\n", x, y);
	}
}

void ShadowDemo::OnMouseWheel(short zDelta, int x, int y)
{
	g_objTrackballCameraController.Zoom(zDelta, 0);
}

