// CompileShader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "ShaderTools.h"
#include <stdio.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 5)
	{

	}
	else
	{
		bool bRet = ShaderTools::GetInstance().CompileShader(argv[1], argv[2], argv[3], argv[4]);
		if (bRet)
		{
			printf("build %s is sucess%s", argv[1]);
		}
	}
	return 0;
}

