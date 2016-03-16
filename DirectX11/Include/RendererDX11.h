#pragma once

#include "IRenderer.h"
#include "Mesh.h"
#include "Camera.h"
#include "imgui.h"

// Shader resources
enum ConstanBuffer
{
    CB_Appliation,
    CB_Frame,
    CB_Object,
    NumConstantBuffers
};

struct MatrixBufferType
{
    DirectX::XMMATRIX world;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
};

//struct LightBufferType
//{
//
//};

class RendererDX11 : public IRenderer
{
public:
    int Initialize(HINSTANCE hInstance, HWND windowHandle, BOOL vSync) override;
    int Update(const float deltaTime) override;
    void Render() override;
    
    void RenderDrawImgui(ImDrawData* draw_data);

    bool LoadContent(HWND windowHandle) override;
    void UnloadContent();
    void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
    void Present(bool vSync);
    void Cleanup();
    void InitFractalCube(DirectX::XMFLOAT3 previousSize, DirectX::XMFLOAT3 previousPosition, DirectX::XMFLOAT3 normal, int layer);
    void ImGui_ImplDX11_CreateFontsTexture();
    int CreateDeviceAndSwapChain(HWND windowHandle, BOOL vSync);
    void CreateRenderTargetView();
    int CreateDepthStencilBuffer(HWND windowHandle);
    void CreateBlendingState();
    void CreateRasterizerState();
    void SetupViewport(HWND windowHandle);
    bool LoadImguiContent();

//private:
    // Direct3D device and swap chain.
    ID3D11Device* m_d3dDevice = nullptr;
    ID3D11DeviceContext* m_d3dDeviceContext = nullptr;
    IDXGISwapChain* m_d3dSwapChain = nullptr;

    // Render target view for the back buffer of the swap chain.
    ID3D11RenderTargetView* m_d3dRenderTargetView = nullptr;
    // Depth/stencil view for use as a depth buffer.
    ID3D11DepthStencilView* m_d3dDepthStencilView = nullptr;
    // A texture to associate to the depth stencil view.
    ID3D11Texture2D* m_d3dDepthStencilBuffer = nullptr;

    // Define the functionality of the depth/stencil stages.
    ID3D11DepthStencilState* m_d3dDepthStencilState = nullptr;
    // Define the functionality of the rasterizer stage.
    ID3D11RasterizerState* m_d3dRasterizerState = nullptr;
    D3D11_VIEWPORT m_viewport = { 0 };

    std::vector<Mesh*> m_meshes;
    Camera m_camera;

    ID3D11Buffer* m_d3dConstantBuffers[NumConstantBuffers];

    DirectX::XMMATRIX m_projectionMatrix;
    DirectX::XMMATRIX m_worldMatrix;

    // Shader data
    ID3D11VertexShader* m_d3dVertexShader = nullptr;
    ID3D11PixelShader* m_d3dPixelShader = nullptr;
    ID3D11InputLayout* m_d3dInputLayout = nullptr;

    float angle = 0.0f;
};
