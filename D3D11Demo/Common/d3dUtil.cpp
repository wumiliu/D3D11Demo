#include "d3dUtil.h"
#include "BaseFunction.h"
#include <iostream>
#include <fstream>
#include <guiddef.h >
using namespace std;

#pragma comment(lib, "d3dcompiler.lib")
void OutputShaderErrorMessage(ID3D10Blob*errorMessage, const char*shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i < bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;
	printf("Error compiling shader.  Check shader-error.txt for message.");
}


DirectX::XMMATRIX GetShow2DMatrix(int nWidth, int nHegith)
{
	// Transform NDC space [-1,+1]^2 to screen space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f * nWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f * nHegith, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f * nWidth, 0.5f * nHegith, 0.0f, 1.0f);
	return XMMatrixInverse(NULL, toTexSpace);
}

HRESULT D3DCompileShader(LPCSTR pFileName, LPCSTR pEntrypoint,
	LPCSTR pTarget, ID3DBlob** ppCode, ID3D11ShaderReflection** ppReflector)
{
	ID3DBlob* errorMessage;
	HRESULT result = S_OK;
	//列主序矩阵（默认设置）
	//行主序矩阵
	DWORD shaderFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;	
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
//	shaderFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
//	shaderFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE_1", "1",
		NULL, NULL
	};
	// Compile the pixel shader code.
	result = D3DCompileFromFile(BaseFunction::gAnsiToUnicode(pFileName), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, pEntrypoint, pTarget, shaderFlags, 0,
		ppCode, &errorMessage);
	const GUID IID_ID3D11ShaderReflection11 = { 0x8d536ca1, 0x0cca, 0x4956, 0xa8, 0x37, 0x78, 0x69, 0x63, 0x75, 0x55, 0x84 };
	if (result == S_OK)
	{
		D3DReflect((*ppCode)->GetBufferPointer(), (*ppCode)->GetBufferSize(),
			IID_ID3D11ShaderReflection11, (void**)ppReflector);
	}
	else
	{
		OutputShaderErrorMessage(errorMessage, "error.txt");
	}

	return result;
}