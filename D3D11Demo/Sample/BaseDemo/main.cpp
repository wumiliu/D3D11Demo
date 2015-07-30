#include "BaseDemo.h"
#include "Diffuse Lighting/DiffuseLight.h"
#include "SpecularLight/SpecularLight.h"

#include "LightMaps/LightMaps.h"
#include "SkyBox/SkyBoxApp.h"

#pragma  comment(lib,"Common.lib")
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
#if defined(DEBUG) || defined(_DEBUG)
	FILE *f(NULL);
	if (AllocConsole())
	{
		freopen_s(&f, "CONOUT$", "w", stdout);
	}
#endif

	SkyBoxApp theApp(hInstance, 1024, 600);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

