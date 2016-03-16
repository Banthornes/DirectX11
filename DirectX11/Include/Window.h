#pragma once

class Window
{
public:
    Window();
    Window(const std::string& projectTitle, HINSTANCE hInstance, uint32_t width, uint32_t height, int cmdShow);
    std::string GetProjectTitle() const;

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    
    void Resize(DirectX::XMUINT2 size);
    void Resize(uint32_t width, uint32_t height);

//private:
    std::string m_projectTitle;
    DirectX::XMUINT2 m_size;

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    LPCSTR m_windowClassName = "DirectXWindowClass";
    LPCSTR m_windowName = "DirectX Template";
    HWND m_windowHandle = 0;
    HINSTANCE m_hInstance;
};