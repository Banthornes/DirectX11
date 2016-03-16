#include "DirectX11PCH.h"
#include "Application.h"
#include "RendererDX11.h"
#include "imgui.h"

Application::Application()
{
    
}

int Application::Initialize(HINSTANCE hInstance, int /*cmdShow*/)
{
    m_window = Window("DirectX Template", hInstance, 1280, 720, true);
    m_renderer = new RendererDX11();
    m_renderer->Initialize(hInstance, m_window.m_windowHandle, true);
    m_renderer->LoadContent(m_window.m_windowHandle);

    return 0;
}

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_col = ImColor(114, 144, 154);

int Application::Run()
{
    MSG msg = { 0 };

    static DWORD previousTime = timeGetTime();
    static const float targetFramerate = 30.0f;
    static const float maxTimeStep = 1.0f / targetFramerate;

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            DWORD currentTime = timeGetTime();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;

            // Cap the delta time to the max time step (useful if your 
            // debugging and you don't want the deltaTime value to explode.
            deltaTime = std::min<float>(deltaTime, maxTimeStep);

            NewFrame(m_window.m_windowHandle);

            // 1. Show a simple window
            // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
            {
                static float f = 0.0f;
                static float rad = 0.0f;
                static int i = 0;
                static double d = 0.0;
                ImGui::Text("Hello, world!");
                ImGui::Text("Float slider");
                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
                ImGui::Text("Integer slider");
                ImGui::SliderInt("int", &i, 0, 255);
                ImGui::SliderAngle("Angle", &rad);
                ImGui::ColorEdit3("clear color", (float*)&clear_col);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            
            // 2. Show another simple window, this time using an explicit Begin/End pair
            if (show_another_window)
            {
                ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
                ImGui::Begin("Another Window", &show_another_window);
                ImGui::Text("Hello");
                ImGui::End();
            }

            m_renderer->Update(deltaTime);
            m_renderer->Render();
        }
    }

    return static_cast<int>(msg.wParam);
}

void Application::NewFrame(HWND /*hwnd*/)
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    GetClientRect(m_window.m_windowHandle, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    //INT64 current_time;
    //QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
    //io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    //g_Time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
    // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
    // io.MousePos : filled by WM_MOUSEMOVE events
    // io.MouseDown : filled by WM_*BUTTON* events
    // io.MouseWheel : filled by WM_MOUSEWHEEL events

    // Hide OS mouse cursor if ImGui is drawing it
    SetCursor(io.MouseDrawCursor ? NULL : LoadCursor(NULL, IDC_ARROW));

    // Start the frame
    ImGui::NewFrame();
}
