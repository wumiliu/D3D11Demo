#include "PickDemo.h"


#pragma  comment(lib,"Common.lib")
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
#if defined(DEBUG) || defined(_DEBUG)
	FILE *f(NULL);
	if (AllocConsole())
	{
		//freopen_s(&f, "CONOUT$", "w", stdout);
		freopen_s(&f, "CONOUT$", "w+t", stdout);// …Í«Î–¥
		freopen_s(&f, "CONIN$", "r+t", stdin);  // …Í«Î∂¡
	}
#endif
	PickDemo theApp(hInstance, 1024, 600);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

