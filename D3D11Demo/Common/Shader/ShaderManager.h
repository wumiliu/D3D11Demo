#pragma once
#include "../HpD3D9Type.h"
#include "ShaderResource.h"

class ShaderManager
{
public:
	static ShaderManager* GetInstancePtr()
	{
		static ShaderManager _instance;
		return &_instance;
	}
	static ShaderManager& ShaderManager::GetInstance()
	{
		return *GetInstancePtr();
	}
	// Blend states.
	IShaderResource* __cdecl BaseShader() const;
	IShaderResource* __cdecl DepthShader() const;

	LightShader* __cdecl BaseLightShader() const;
	ShadowShader* __cdecl BaseShadowShader() const;

	
private:
	explicit ShaderManager();
	~ShaderManager();
	// Private implementation.
	class Impl;

	std::shared_ptr<Impl> pImpl;

	// Prevent copying.
	ShaderManager(ShaderManager const&);
	ShaderManager& operator= (ShaderManager const&);

};

#define g_objShaders ShaderManager::GetInstance()
