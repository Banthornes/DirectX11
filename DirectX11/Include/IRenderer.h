#pragma once

__interface IRenderer
{
    int Initialize(HINSTANCE hInstance, HWND windowHandle, BOOL vSync);
    int Update(const float deltaTime);
    void Render();
    bool LoadContent(HWND windowHandle);
};
