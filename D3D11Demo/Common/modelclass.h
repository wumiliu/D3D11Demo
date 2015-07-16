////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "HpD3D9Type.h"


///////////////////////
// MY CLASS INCLUDES //
///////////////////////


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
private:
	struct VertexType
	{
		Vector3 position;
		Vector3 normal;
		Vector2 texture;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*textureFilename);
	void Shutdown();
	
	void RenderBuffers(ID3D11DeviceContext*);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	void SetPosition(float, float, float);
	void GetPosition(float&, float&, float&);

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();

	bool LoadModel(char*);
	void ReleaseModel();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	float m_positionX, m_positionY, m_positionZ;
	std::wstring m_fileName;
	ID3D11ShaderResourceView* m_ShaderResourceView;
};

#endif