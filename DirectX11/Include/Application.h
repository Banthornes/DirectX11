#pragma once

#include "Window.h"
#include "IRenderer.h"

class Application
{
public:
    Application();

    int Initialize(HINSTANCE hInstance, int cmdShow);
    int Run();
    void NewFrame(HWND hwnd);

private:
    Window m_window;
    IRenderer* m_renderer;
};