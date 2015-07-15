#include "stdafx.h"
#include "ShaderTools.h"
#include <iostream>
#include <fstream>
using namespace std;
ShaderTools::ShaderTools()
{
}

ShaderTools::~ShaderTools()
{

}

ShaderTools* ShaderTools::GetInstancePtr()
{
	static ShaderTools _instance;
	return &_instance;
}

ShaderTools& ShaderTools::GetInstance()
{
	return *(GetInstancePtr());
}

bool ShaderTools::CompileShader(const char* sourceFile, const char* destFile,
	const char* pEntrypoint, const char* pTarget)
{
	FILE * m_IFileRead;
	fopen_s(&m_IFileRead, sourceFile, "rb");
	fseek(m_IFileRead, 0, SEEK_END);
	int fileSize = ftell(m_IFileRead);
	unsigned char* sourceBuffer = new unsigned char[fileSize];
	fseek(m_IFileRead, 0, SEEK_SET);
	fread(sourceBuffer, sizeof(char), fileSize, m_IFileRead);
	fclose(m_IFileRead);
	ID3DBlob* shaderBuffer;
	ID3DBlob* errorMessage;
	//D3D10_SHADER_PACK_MATRIX_ROW_MAJOR
	//http://msdn.microsoft.com/zh-cn/library/windows/apps/dn166865/html
	//行主序矩阵
	//列主序矩阵（默认设置）
	DWORD shaderFlags = D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
#if defined( DEBUG ) || defined( _DEBUG )
	//  shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3DCOMPILE_DEBUG;
	//shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif


	HRESULT result = D3DCompile(sourceBuffer, fileSize,
		sourceFile, NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		pEntrypoint,
		pTarget,
		shaderFlags,
		0, &shaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, sourceFile);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			printf("%s is Missing Shader File\n", sourceFile);
		}
		return false;
	}
	m_IFileRead = NULL;
	LPVOID pBuffer = shaderBuffer->GetBufferPointer();
	fopen_s(&m_IFileRead, destFile, "wb");
	fwrite(pBuffer, sizeof(unsigned char), shaderBuffer->GetBufferSize(), m_IFileRead);
	fclose(m_IFileRead);
	m_IFileRead = NULL;
	delete[]sourceBuffer;
	return true;
}

void ShaderTools::OutputShaderErrorMessage(ID3D10Blob*errorMessage, const char*shaderFilename)
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
