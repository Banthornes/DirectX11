#include "DirectX11PCH.h"
#include "Window.h"
#include "imgui.h"

LPCWSTR g_WindowClassName = L"DirectXWindowClass";
LPCWSTR g_WindowName = L"DirectX Template";
const LONG g_WindowWidth = 1280;
const LONG g_WindowHeight = 720;


Window::Window()
{
}

Window::Window(const std::string &projectTitle, HINSTANCE hInstance, uint32_t width, uint32_t height, int cmdShow)
    : m_projectTitle(projectTitle)
    , m_size(width, height)
{
    WNDCLASSEX wndClass = { 0 };
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = &WndProc;
    wndClass.hInstance = hInstance;
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndClass.lpszMenuName = nullptr;
    wndClass.lpszClassName = g_WindowClassName;

    if (!RegisterClassEx(&wndClass))
    {
        return;
    }

    RECT windowRect = { 0, 0, g_WindowWidth, g_WindowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    m_windowHandle = CreateWindowW(g_WindowClassName, g_WindowName,
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!m_windowHandle)
    {
        return;
    }

    ShowWindow(m_windowHandle, cmdShow);
    UpdateWindow(m_windowHandle);

    //// Create Window
    //RECT windowRect = { 0, 0, g_WindowWidth, g_WindowHeight };
    //AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
    //
    //g_WindowHandle = CreateWindowA(g_WindowClassName, g_WindowName,
    //    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
    //    windowRect.right - windowRect.left,
    //    windowRect.bottom - windowRect.top,
    //    nullptr, nullptr, hInstance, nullptr);
    //
    //if (!g_WindowHandle)
    //{
    //    return -1;
    //}
    //
    //ShowWindow(g_WindowHandle, cmdShow);
    //UpdateWindow(g_WindowHandle);
    //
    //return 0;
}

std::string Window::GetProjectTitle() const
{
    return m_projectTitle;
}

uint32_t Window::GetWidth() const
{
    return m_size.x;
}

uint32_t Window::GetHeight() const
{
    return m_size.y;
}

void Window::Resize(DirectX::XMUINT2 /*size*/)
{
    // resize event
    throw std::exception("Not implemented");
}

void Window::Resize(uint32_t /*width*/, uint32_t /*height*/)
{
    throw std::exception("Not implemented");
}

LRESULT ImGui_ImplDX11_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
IMGUI_API LRESULT ImGui_ImplDX11_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        io.MouseDown[0] = true;
        return true;
    case WM_LBUTTONUP:
        io.MouseDown[0] = false;
        return true;
    case WM_RBUTTONDOWN:
        io.MouseDown[1] = true;
        return true;
    case WM_RBUTTONUP:
        io.MouseDown[1] = false;
        return true;
    case WM_MBUTTONDOWN:
        io.MouseDown[2] = true;
        return true;
    case WM_MBUTTONUP:
        io.MouseDown[2] = false;
        return true;
    case WM_MOUSEWHEEL:
        io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
        return true;
    case WM_MOUSEMOVE:
        io.MousePos.x = (signed short)(lParam);
        io.MousePos.y = (signed short)(lParam >> 16);
        return true;
    case WM_KEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = 1;
        return true;
    case WM_KEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = 0;
        return true;
    case WM_CHAR:
        // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        return true;
    }
    return 0;
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplDX11_WndProcHandler(hwnd, message, wParam, lParam))
        return true;

    PAINTSTRUCT paintStruct;
    HDC hDC;

    switch (message)
    {
    case WM_SIZE:
    {
        return 0;
    }
    case WM_PAINT:
    {
        hDC = BeginPaint(hwnd, &paintStruct);
        EndPaint(hwnd, &paintStruct);
    }
    break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
    }
    break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}