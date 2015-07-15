#pragma once
#include <iostream>
#include <stdio.h>
#include <D3Dcompiler.h >
#include <windows.h>
#include <d3d11_1.h>
#include <d3d11.h>

#pragma comment(lib, "d3dcompiler.lib")

class ShaderTools
{
public:
	ShaderTools();
	~ShaderTools();
public:
	static ShaderTools* GetInstancePtr();
	static ShaderTools& GetInstance();

	bool CompileShader(const char* sourceFile, const char* destFile,
		const char* pEntrypoint,
		const char* pTarget
		);
	bool CompileShaderEx(const char* sourceFile, const char* destFile,
		const char* pEntrypoint,
		const char* pTarget
		);
private:
	void OutputShaderErrorMessage(ID3D10Blob*,const char*pszFile);
};

