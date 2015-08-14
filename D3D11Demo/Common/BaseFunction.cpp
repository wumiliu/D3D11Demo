#include "BaseFunction.h"


wchar_t* BaseFunction::gAnsiToUnicode(const char* szStr)
{
	int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

std::string BaseFunction::UnicodeToANSI(const wchar_t* str)
{

	char*     pElementText;
	int    iTextLen;
	// wide char to multi char
	iTextLen = WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		NULL,
		0,
		NULL,
		NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, sizeof(char)* (iTextLen + 1));
	::WideCharToMultiByte(CP_ACP,
		0,
		str,
		-1,
		pElementText,
		iTextLen,
		NULL,
		NULL);
	std::string strText;
	strText = pElementText;
	delete[] pElementText;
	return strText;
}

std::string& BaseFunction::trim(std::string &s)
{
	if (s.empty())
	{
		return s;
	}
	const std::string drop = " ";
	std::string stTmp = "";
	int nPos = s.find_first_of(drop);
	int i = 0;
	while (nPos != std::string::npos && i <= 8)
	{
		stTmp = s.erase(nPos, 1);
		nPos = s.find_first_of(drop);
		i++;
	}
	// trim right
	s.erase(s.find_last_not_of(drop) + 1);
	// trim left
	return s.erase(0, s.find_first_not_of(drop));
	return s;
}
