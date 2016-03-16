#include <DirectX11PCH.h>

#include "Application.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
    UNREFERENCED_PARAMETER(prevInstance);
    UNREFERENCED_PARAMETER(cmdLine);

    Application application = Application();
    application.Initialize(hInstance, cmdShow);
    return application.Run();
}




