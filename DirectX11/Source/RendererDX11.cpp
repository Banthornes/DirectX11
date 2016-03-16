#include "DirectX11PCH.h"
#include "IRenderer.h"
#include "RendererDX11.h"
#include "Mesh.h"
#include "Cube.h"
#include "Camera.h"

#define NUM_LAYERS 5
DirectX::XMFLOAT3 startSize(3.0f, 3.0f, 3.0f);
// Data
static INT64                    g_Time = 0;
static INT64                    g_TicksPerSecond = 0;

static ID3D11Buffer*            g_pVB = NULL;
static ID3D11Buffer*            g_pIB = NULL;
static ID3D10Blob *             g_pVertexShaderBlob = NULL;
static ID3D11VertexShader*      g_pVertexShader = NULL;
static ID3D11InputLayout*       g_pInputLayout = NULL;
static ID3D11Buffer*            g_pVertexConstantBuffer = NULL;
static ID3D10Blob *             g_pPixelShaderBlob = NULL;
static ID3D11PixelShader*       g_pPixelShader = NULL;
static ID3D11SamplerState*      g_pFontSampler = NULL;
static ID3D11ShaderResourceView*g_pFontTextureView = NULL;
static ID3D11RasterizerState*   g_pRasterizerState = NULL;
static ID3D11BlendState*        g_pBlendState = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct VERTEX_CONSTANT_BUFFER
{
    float        mvp[4][4];
};

// This function was inspired by:
// http://www.rastertek.com/dx11tut03.html
DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
    DXGI_RATIONAL refreshRate = { 0, 1 };
    if (vsync)
    {
        IDXGIFactory* factory;
        IDXGIAdapter* adapter;
        IDXGIOutput* adapterOutput;
        DXGI_MODE_DESC* displayModeList;

        // Create a DirectX graphics interface factory.
        HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Could not create DXGIFactory instance."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to create DXGIFactory.");
        }

        hr = factory->EnumAdapters(0, &adapter);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to enumerate adapters."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to enumerate adapters.");
        }

        hr = adapter->EnumOutputs(0, &adapterOutput);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to enumerate adapter outputs."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to enumerate adapter outputs.");
        }

        UINT numDisplayModes;
        hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to query display mode list."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to query display mode list.");
        }

        displayModeList = new DXGI_MODE_DESC[numDisplayModes];
        assert(displayModeList);

        hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
        if (FAILED(hr))
        {
            MessageBox(0,
                TEXT("Failed to query display mode list."),
                TEXT("Query Refresh Rate"),
                MB_OK);

            throw new std::exception("Failed to query display mode list.");
        }

        // Now store the refresh rate of the monitor that matches the width and height of the requested screen.
        for (UINT i = 0; i < numDisplayModes; ++i)
        {
            if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
            {
                refreshRate = displayModeList[i].RefreshRate;
            }
        }

        delete[] displayModeList;
        SafeRelease(adapterOutput);
        SafeRelease(adapter);
        SafeRelease(factory);
    }

    return refreshRate;
}

void RendererDX11::InitFractalCube(DirectX::XMFLOAT3 previousSize, DirectX::XMFLOAT3 previousPosition, DirectX::XMFLOAT3 normal, int layer)
{
    if (layer == 0)
    {
        DirectX::XMFLOAT3 s = previousSize;
        DirectX::XMFLOAT3 p = previousPosition;
        m_meshes.push_back(new Cube(s, p));
        InitFractalCube(previousSize, previousPosition, normal, layer + 1);
    }
    else if(layer == 1)
    {
        DirectX::XMFLOAT3 s = DirectX::XMFLOAT3(previousSize.x / 2, previousSize.y / 2, previousSize.z / 2);
        DirectX::XMFLOAT3 p = DirectX::XMFLOAT3(previousPosition.x + previousSize.x / 2 + s.x / 2, previousPosition.y + previousSize.x / 2 + s.y / 2, previousPosition.z + previousSize.z / 2 + s.z / 2);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3(-p.x, 0.0f, 0.0f)));
        InitFractalCube(s, DirectX::XMFLOAT3(-p.x, 0.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), layer + 1);

        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3( p.x, 0.0f, 0.0f)));
        InitFractalCube(s, DirectX::XMFLOAT3(p.x, 0.0f, 0.0f), DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f), layer + 1);

        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3( 0.0f,-p.y, 0.0f)));
        InitFractalCube(s, DirectX::XMFLOAT3(0.0f,-p.y, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), layer + 1);
        
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3( 0.0f, p.y, 0.0f)));
        InitFractalCube(s, DirectX::XMFLOAT3(0.0f, p.y, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), layer + 1);
        
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3( 0.0f, 0.0f,-p.z)));
        InitFractalCube(s, DirectX::XMFLOAT3(0.0f, 0.0f,-p.z), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), layer + 1);
        
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3( 0.0f, 0.0f, p.z)));
        InitFractalCube(s, DirectX::XMFLOAT3(0.0f, 0.0f, p.z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), layer + 1);
    }
    else
    {
        if (layer > NUM_LAYERS)
            return;

        DirectX::XMFLOAT3 s = DirectX::XMFLOAT3(previousSize.x / 2, previousSize.y / 2, previousSize.z / 2);

        auto vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        auto vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        auto dfloat3 = DirectX::XMFLOAT3(previousSize.x / 2 + s.x / 2, 0.0f, 0.0f);
        auto dvector = DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3));
        auto cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), dvector);
        
        DirectX::XMFLOAT3 currentPosition;
        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), layer + 1);

        vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        dfloat3 = DirectX::XMFLOAT3(-(previousSize.x / 2 + s.x / 2), 0.0f, 0.0f);
        cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3)));

        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), layer + 1);

        //m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3(0.0f, -p.y, 0.0f)));
        //InitFractalCube(s, p, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), layer + 1);
        vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        dfloat3 = DirectX::XMFLOAT3(0.0f, -(previousSize.y / 2 + s.y / 2), 0.0f);
        cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3)));

        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(0.0f,-1.0f, 0.0f), layer + 1);

        //m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3(0.0f, p.y, 0.0f)));
        //InitFractalCube(s, p, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), layer + 1);
        vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        dfloat3 = DirectX::XMFLOAT3(0.0f, (previousSize.y / 2 + s.y / 2), 0.0f);
        cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3)));

        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), layer + 1);

        //m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3(0.0f, 0.0f, -p.z)));
        //InitFractalCube(s, p, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), layer + 1);
        vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        dfloat3 = DirectX::XMFLOAT3(0.0f, 0.0f, -(previousSize.y / 2 + s.y / 2));
        cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3)));

        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(0.0f, 0.0f,-1.0f), layer + 1);
        //m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), DirectX::XMFLOAT3(0.0f, 0.0f, p.z)));
        //InitFractalCube(s, p, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), layer + 1);
        vpreviousPosition = DirectX::XMLoadFloat3(&previousPosition);
        vpreviousSize = DirectX::XMLoadFloat3(&previousSize);
        dfloat3 = DirectX::XMFLOAT3(0.0f, 0.0f, (previousSize.y / 2 + s.y / 2));
        cp = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&previousPosition), DirectX::XMVECTOR(DirectX::XMLoadFloat3(&dfloat3)));

        DirectX::XMStoreFloat3(&currentPosition, cp);
        m_meshes.push_back(new Cube(DirectX::XMFLOAT3(s.x, s.y, s.z), currentPosition));
        InitFractalCube(s, currentPosition, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), layer + 1);
    }
}

bool RendererDX11::LoadContent(HWND windowHandle)
{
    assert(m_d3dDevice);
    
    InitFractalCube(startSize, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 0);

    for (auto i = 0; i < m_meshes.size(); ++i)
    {
        m_meshes[i]->InitializeBuffers(m_d3dDevice);
    }
    
    m_camera = Camera();

    HRESULT hr;
    // Load the compiled vertex shader.
    ID3DBlob* vertexShaderBlob;
    LPCWSTR compiledVertexShaderObject = L"SimpleVertexShader.cso";
    // Load the compiled pixel shader.
    ID3DBlob* pixelShaderBlob;
    LPCWSTR compiledPixelShaderObject = L"SimplePixelShader.cso";

    hr = D3DReadFileToBlob(compiledVertexShaderObject, &vertexShaderBlob);
    if (FAILED(hr))
    {
        return false;
    }

    hr = D3DReadFileToBlob(compiledPixelShaderObject, &pixelShaderBlob);
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_d3dVertexShader);
    if (FAILED(hr))
    {
        return false;
    }

    hr = m_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_d3dPixelShader);
    if (FAILED(hr))
    {
        return false;
    }

    // Create the input layout for the vertex shader.
    D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_d3dInputLayout);
    if (FAILED(hr))
    {
        return false;
    }

    SafeRelease(vertexShaderBlob);
    SafeRelease(pixelShaderBlob);

    // Create the constant buffers for the variables defined in the vertex shader.
    D3D11_BUFFER_DESC constantBufferDesc;
    ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
    constantBufferDesc.CPUAccessFlags = 0;
    constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_d3dConstantBuffers[CB_Appliation]);
    if (FAILED(hr))
    {
        return false;
    }
    hr = m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_d3dConstantBuffers[CB_Frame]);
    if (FAILED(hr))
    {
        return false;
    }
    hr = m_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &m_d3dConstantBuffers[CB_Object]);
    if (FAILED(hr))
    {
        return false;
    }

    // Setup the projection matrix.
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions.
    // This is required for a correct projection matrix.
    float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
    float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

    m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

    m_d3dDeviceContext->UpdateSubresource(m_d3dConstantBuffers[CB_Appliation], 0, nullptr, &m_projectionMatrix, 0, 0);

    return true;
}

void RendererDX11::ImGui_ImplDX11_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Upload texture to graphics system
    {
        D3D11_TEXTURE2D_DESC texDesc;
        ZeroMemory(&texDesc, sizeof(texDesc));
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = 0;

        ID3D11Texture2D *pTexture = NULL;
        D3D11_SUBRESOURCE_DATA subResource;
        subResource.pSysMem = pixels;
        subResource.SysMemPitch = texDesc.Width * 4;
        subResource.SysMemSlicePitch = 0;
        m_d3dDevice->CreateTexture2D(&texDesc, &subResource, &pTexture);

        // Create texture view
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;
        m_d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, &g_pFontTextureView);
        pTexture->Release();
    }

    // Store our identifier
    io.Fonts->TexID = (void *)g_pFontTextureView;

    // Create texture sampler
    {
        D3D11_SAMPLER_DESC samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.f;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samplerDesc.MinLOD = 0.f;
        samplerDesc.MaxLOD = 0.f;
        m_d3dDevice->CreateSamplerState(&samplerDesc, &g_pFontSampler);
    }
}

int RendererDX11::CreateDeviceAndSwapChain(HWND windowHandle, BOOL vSync)
{
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = clientWidth;
    swapChainDesc.BufferDesc.Height = clientHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, vSync);
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = windowHandle;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Windowed = TRUE;

    UINT createDeviceFlags = 0;

    // Apperantly this is no longer supported/is intergrated automaticly
    //#if _DEBUG
    //    createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
    //#endif

    // These are the feature levels that we will accept.
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    // This will be the feature level that 
    // is used to create our device and swap chain.
    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = E_INVALIDARG;
    for (int i = 0; i < _countof(featureLevels); ++i)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
            nullptr, createDeviceFlags, &featureLevels[i], _countof(featureLevels) - i,
            D3D11_SDK_VERSION, &swapChainDesc, &m_d3dSwapChain, &m_d3dDevice, &featureLevel,
            &m_d3dDeviceContext);
        if (hr == S_OK)
            break;
    }

    if (FAILED(hr))
    {
        return -1;
    }

    return 0;
}

void RendererDX11::CreateRenderTargetView()
{
    DXGI_SWAP_CHAIN_DESC sd;
    m_d3dSwapChain->GetDesc(&sd);

    // Create the render target
    ID3D11Texture2D* pBackBuffer;
    D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
    ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
    render_target_view_desc.Format = sd.BufferDesc.Format;
    render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    m_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    m_d3dDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &m_d3dRenderTargetView);
    pBackBuffer->Release();
}

int RendererDX11::CreateDepthStencilBuffer(HWND windowHandle)
{
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    // Create the depth buffer for use with the depth/stencil view.
    D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

    depthStencilBufferDesc.ArraySize = 1;
    depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
    depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilBufferDesc.Width = clientWidth;
    depthStencilBufferDesc.Height = clientHeight;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

    HRESULT hr = m_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &m_d3dDepthStencilBuffer);
    if (FAILED(hr))
    {
        return -1;
    }

    hr = m_d3dDevice->CreateDepthStencilView(m_d3dDepthStencilBuffer, nullptr, &m_d3dDepthStencilView);
    if (FAILED(hr))
    {
        return -1;
    }

    return 0;
}

void RendererDX11::CreateBlendingState()
{
    // Create the blending setup
    {
        D3D11_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.AlphaToCoverageEnable = false;
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        m_d3dDevice->CreateBlendState(&desc, &g_pBlendState);
    }
}

void RendererDX11::CreateRasterizerState()
{
    DXGI_SWAP_CHAIN_DESC sd;
    m_d3dSwapChain->GetDesc(&sd);

    // Setup rasterizer state.
    D3D11_RASTERIZER_DESC rasterizerDesc;
    ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

    rasterizerDesc.AntialiasedLineEnable = FALSE;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.DepthBias = 0;
    rasterizerDesc.DepthBiasClamp = 0.0f;
    rasterizerDesc.DepthClipEnable = TRUE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.FrontCounterClockwise = FALSE;
    rasterizerDesc.MultisampleEnable = FALSE;
    rasterizerDesc.ScissorEnable = FALSE;
    rasterizerDesc.SlopeScaledDepthBias = 0.0f;

    // Setup rasterizer
    {
        D3D11_RASTERIZER_DESC RSDesc;
        memset(&RSDesc, 0, sizeof(D3D11_RASTERIZER_DESC));
        RSDesc.FillMode = D3D11_FILL_SOLID;
        RSDesc.CullMode = D3D11_CULL_NONE;
        RSDesc.FrontCounterClockwise = FALSE;
        RSDesc.DepthBias = 0;
        RSDesc.SlopeScaledDepthBias = 0.0f;
        RSDesc.DepthBiasClamp = 0;
        RSDesc.DepthClipEnable = TRUE;
        RSDesc.ScissorEnable = TRUE;
        RSDesc.AntialiasedLineEnable = FALSE;
        RSDesc.MultisampleEnable = (sd.SampleDesc.Count > 1) ? TRUE : FALSE;

        ID3D11RasterizerState* pRState = NULL;
        m_d3dDevice->CreateRasterizerState(&RSDesc, &pRState);
        m_d3dDeviceContext->RSSetState(pRState);
        pRState->Release();
    }
}

void RendererDX11::SetupViewport(HWND windowHandle)
{
    RECT clientRect;
    GetClientRect(windowHandle, &clientRect);

    // Compute the exact client dimensions. This will be used
    // to initialize the render targets for our swap chain.
    unsigned int clientWidth = clientRect.right - clientRect.left;
    unsigned int clientHeight = clientRect.bottom - clientRect.top;

    // Initialize the viewport to occupy the entire client area.
    m_viewport.Width = static_cast<float>(clientWidth);
    m_viewport.Height = static_cast<float>(clientHeight);
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
}

bool RendererDX11::LoadImguiContent()
{
    // Create the vertex shader
    {
        static const char* vertexShader =
            "cbuffer vertexBuffer : register(b0) \
            {\
            float4x4 ProjectionMatrix; \
            };\
            struct VS_INPUT\
            {\
            float2 pos : POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            \
            PS_INPUT main(VS_INPUT input)\
            {\
            PS_INPUT output;\
            output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));\
            output.col = input.col;\
            output.uv  = input.uv;\
            return output;\
            }";

        D3DCompile(vertexShader, strlen(vertexShader), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, &g_pVertexShaderBlob, NULL);
        if (g_pVertexShaderBlob == NULL) // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (m_d3dDevice->CreateVertexShader((DWORD*)g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), NULL, &g_pVertexShader) != S_OK)
            return false;

        // Create the input layout
        D3D11_INPUT_ELEMENT_DESC localLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,   0, (size_t)(&((ImDrawVert*)0)->uv),  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, (size_t)(&((ImDrawVert*)0)->col), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        if (m_d3dDevice->CreateInputLayout(localLayout, 3, g_pVertexShaderBlob->GetBufferPointer(), g_pVertexShaderBlob->GetBufferSize(), &g_pInputLayout) != S_OK)
            return false;

        // Create the constant buffer
        {
            D3D11_BUFFER_DESC cbDesc;
            cbDesc.ByteWidth = sizeof(VERTEX_CONSTANT_BUFFER);
            cbDesc.Usage = D3D11_USAGE_DYNAMIC;
            cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            cbDesc.MiscFlags = 0;
            m_d3dDevice->CreateBuffer(&cbDesc, NULL, &g_pVertexConstantBuffer);
        }
    }

    // Create the pixel shader
    {
        static const char* pixelShader =
            "struct PS_INPUT\
            {\
            float4 pos : SV_POSITION;\
            float4 col : COLOR0;\
            float2 uv  : TEXCOORD0;\
            };\
            sampler sampler0;\
            Texture2D texture0;\
            \
            float4 main(PS_INPUT input) : SV_Target\
            {\
            float4 out_col = input.col * texture0.Sample(sampler0, input.uv); \
            return out_col; \
            }";

        D3DCompile(pixelShader, strlen(pixelShader), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, &g_pPixelShaderBlob, NULL);
        if (g_pPixelShaderBlob == NULL)  // NB: Pass ID3D10Blob* pErrorBlob to D3DCompile() to get error showing in (const char*)pErrorBlob->GetBufferPointer(). Make sure to Release() the blob!
            return false;
        if (m_d3dDevice->CreatePixelShader((DWORD*)g_pPixelShaderBlob->GetBufferPointer(), g_pPixelShaderBlob->GetBufferSize(), NULL, &g_pPixelShader) != S_OK)
            return false;
    }
    return true;
}

int RendererDX11::Initialize(HINSTANCE /*hInstance*/, HWND windowHandle, BOOL vSync)
{
    angle = 0.0f;
    ImGuiIO& io = ImGui::GetIO();
    io.RenderDrawListsFn = NULL;  // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.ImeWindowHandle = windowHandle;

    // A window handle must have been created already
    //assert(windowHandle != 0);
    // Check for DirectX Math library support.
    if (!DirectX::XMVerifyCPUSupport())
    {
        MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
        return -1;
    }

    // A window handle must have been created already.
    assert(windowHandle != 0);

    int result = CreateDeviceAndSwapChain(windowHandle, vSync);
    if (result == E_FAIL)
        return E_FAIL;
   
    CreateRenderTargetView();
    CreateDepthStencilBuffer(windowHandle);
    CreateBlendingState();
    CreateRasterizerState();
    SetupViewport(windowHandle);

    LoadImguiContent();
    ImGui_ImplDX11_CreateFontsTexture();
    
    return 0;
}


int RendererDX11::Update(const float deltaTime)
{
    m_camera.Update();

    for (int i = 0; i < m_meshes.size(); ++i)
    {
        m_meshes[i]->Update(deltaTime);
    }


    DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
    DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
    DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);

    auto rotationFloat3 = DirectX::XMFLOAT3(angle, angle, angle);
    auto rotationMatrix = DirectX::XMMatrixRotationRollPitchYawFromVector(DirectX::XMLoadFloat3(&rotationFloat3));

    //angle += 1 * deltaTime;

    ImGui::Begin("Demo GUI");
    ImGui::SetWindowSize(ImVec2(340, 75));
    ImGui::SliderAngle("Angle", &angle);
    ImGui::End();

    auto eyePos = DirectX::XMVector3Transform(eyePosition, rotationMatrix);
    auto upDir = DirectX::XMVector3Transform(upDirection, rotationMatrix);
    m_camera.m_viewMatrix = DirectX::XMMatrixLookAtLH(eyePos, focusPoint, upDir);
    m_d3dDeviceContext->UpdateSubresource(m_d3dConstantBuffers[CB_Frame], 0, nullptr, &m_camera.m_viewMatrix, 0, 0);

    return 0;
}

// Clear the color and depth buffers.
void RendererDX11::Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
    m_d3dDeviceContext->ClearRenderTargetView(m_d3dRenderTargetView, clearColor);
    m_d3dDeviceContext->ClearDepthStencilView(m_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void RendererDX11::Present(bool vSync)
{
    if (vSync)
    {
        m_d3dSwapChain->Present(1, 0);
    }
    else
    {
        m_d3dSwapChain->Present(0, 0);
    }
}

void RendererDX11::Render()
{
    assert(m_d3dDevice);
    assert(m_d3dDeviceContext);

    ImVec4 clear_col = ImColor(114, 144, 154);
    ImGui::Begin("ClearColor");
    ImGui::ColorEdit3("clear color", (float*)&clear_col);
    ImGui::End();
    
    Clear((float*)&clear_col, 1.0f, 0);
    
    //Clear(DirectX::Colors::CornflowerBlue, 1.0f, 0);

    const UINT vertexStride = sizeof(VertexPosColor);
    const UINT offset = 0;
    
    m_d3dDeviceContext->RSSetState(m_d3dRasterizerState);
    m_d3dDeviceContext->RSSetViewports(1, &m_viewport);
    
    m_d3dDeviceContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, m_d3dDepthStencilView);
    m_d3dDeviceContext->OMSetDepthStencilState(m_d3dDepthStencilState, 1);
    
    m_d3dDeviceContext->VSSetShader(m_d3dVertexShader, nullptr, 0);
    m_d3dDeviceContext->VSSetConstantBuffers(0, 3, m_d3dConstantBuffers);
    
    m_d3dDeviceContext->PSSetShader(m_d3dPixelShader, nullptr, 0);
    
    m_d3dDeviceContext->IASetInputLayout(m_d3dInputLayout);
    
    for (auto i = 0; i < m_meshes.size(); ++i)
    {
        m_d3dDeviceContext->UpdateSubresource(m_d3dConstantBuffers[CB_Object], 0, nullptr, &m_meshes[i]->m_matrix, 0, 0);
        m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        m_d3dDeviceContext->IASetVertexBuffers(0, 1, &m_meshes[i]->m_d3dVertexBuffer, &vertexStride, &offset);
        m_d3dDeviceContext->IASetIndexBuffer(m_meshes[i]->m_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        m_d3dDeviceContext->DrawIndexed(UINT(m_meshes[i]->m_indices.size()), 0, 0);
    }
    
    m_d3dDeviceContext->OMSetRenderTargets(1, &m_d3dRenderTargetView, NULL);

    ImGui::Render();
    auto imguiData = ImGui::GetDrawData();
    assert(imguiData != nullptr);
    //if (imguiData != nullptr)
    {
        RenderDrawImgui(imguiData);
    }

    Present(true);
}

void RendererDX11::RenderDrawImgui(ImDrawData * draw_data)
{
    // Create and grow vertex/index buffers if needed
    if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
    {
        if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
        g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = g_VertexBufferSize * sizeof(ImDrawVert);
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        if (m_d3dDevice->CreateBuffer(&desc, NULL, &g_pVB) < 0)
            return;
    }
    if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
    {
        if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
        g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
        D3D11_BUFFER_DESC bufferDesc;
        memset(&bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        bufferDesc.ByteWidth = g_IndexBufferSize * sizeof(ImDrawIdx);
        bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (m_d3dDevice->CreateBuffer(&bufferDesc, NULL, &g_pIB) < 0)
            return;
    }

    // Copy and convert all vertices into a single contiguous buffer
    D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
    if (m_d3dDeviceContext->Map(g_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource) != S_OK)
        return;
    if (m_d3dDeviceContext->Map(g_pIB, 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource) != S_OK)
        return;
    ImDrawVert* vtx_dst = (ImDrawVert*)vtx_resource.pData;
    ImDrawIdx* idx_dst = (ImDrawIdx*)idx_resource.pData;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        memcpy(vtx_dst, &cmd_list->VtxBuffer[0], cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));
        memcpy(idx_dst, &cmd_list->IdxBuffer[0], cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx));
        vtx_dst += cmd_list->VtxBuffer.size();
        idx_dst += cmd_list->IdxBuffer.size();
    }
    m_d3dDeviceContext->Unmap(g_pVB, 0);
    m_d3dDeviceContext->Unmap(g_pIB, 0);

    // Setup orthographic projection matrix into our constant buffer
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (m_d3dDeviceContext->Map(g_pVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource) != S_OK)
            return;

        VERTEX_CONSTANT_BUFFER* pConstantBuffer = (VERTEX_CONSTANT_BUFFER*)mappedResource.pData;
        const float L = 0.0f;
        const float R = ImGui::GetIO().DisplaySize.x;
        const float B = ImGui::GetIO().DisplaySize.y;
        const float T = 0.0f;
        const float mvp[4][4] =
        {
            { 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
            { 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
            { 0.0f,         0.0f,           0.5f,       0.0f },
            { (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
        };
        memcpy(&pConstantBuffer->mvp, mvp, sizeof(mvp));
        m_d3dDeviceContext->Unmap(g_pVertexConstantBuffer, 0);
    }

    // Setup viewport
    {
        D3D11_VIEWPORT vp;
        memset(&vp, 0, sizeof(D3D11_VIEWPORT));
        vp.Width = ImGui::GetIO().DisplaySize.x;
        vp.Height = ImGui::GetIO().DisplaySize.y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_d3dDeviceContext->RSSetViewports(1, &vp);
    }

    // Bind shader and vertex buffers
    unsigned int stride = sizeof(ImDrawVert);
    unsigned int offset = 0;
    m_d3dDeviceContext->IASetInputLayout(g_pInputLayout);
    m_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_pVB, &stride, &offset);
    m_d3dDeviceContext->IASetIndexBuffer(g_pIB, sizeof(ImDrawIdx) == 2 ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT, 0);
    m_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_d3dDeviceContext->VSSetShader(g_pVertexShader, NULL, 0);
    m_d3dDeviceContext->VSSetConstantBuffers(0, 1, &g_pVertexConstantBuffer);
    m_d3dDeviceContext->PSSetShader(g_pPixelShader, NULL, 0);
    m_d3dDeviceContext->PSSetSamplers(0, 1, &g_pFontSampler);

    // Setup render state
    const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    m_d3dDeviceContext->OMSetBlendState(g_pBlendState, blendFactor, 0xffffffff);
    m_d3dDeviceContext->RSSetState(g_pRasterizerState);

    // Render command lists
    int vtx_offset = 0;
    int idx_offset = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback)
            {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                const D3D11_RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
                m_d3dDeviceContext->PSSetShaderResources(0, 1, (ID3D11ShaderResourceView**)&pcmd->TextureId);
                m_d3dDeviceContext->RSSetScissorRects(1, &r);
                m_d3dDeviceContext->DrawIndexed(pcmd->ElemCount, idx_offset, vtx_offset);
            }
            idx_offset += pcmd->ElemCount;
        }
        vtx_offset += cmd_list->VtxBuffer.size();
    }

    // Restore modified state
    m_d3dDeviceContext->IASetInputLayout(NULL);
    m_d3dDeviceContext->PSSetShader(NULL, NULL, 0);
    m_d3dDeviceContext->VSSetShader(NULL, NULL, 0);
}

void RendererDX11::UnloadContent()
{
    SafeRelease(m_d3dConstantBuffers[CB_Appliation]);
    SafeRelease(m_d3dConstantBuffers[CB_Frame]);
    SafeRelease(m_d3dConstantBuffers[CB_Object]);

    m_meshes.clear();
    //SafeRelease(m_d3dIndexBuffer);
    //SafeRelease(m_d3dVertexBuffer);
    SafeRelease(m_d3dInputLayout);
    SafeRelease(m_d3dVertexShader);
    SafeRelease(m_d3dPixelShader);
}

void RendererDX11::Cleanup()
{
    SafeRelease(m_d3dDepthStencilView);
    SafeRelease(m_d3dRenderTargetView);
    SafeRelease(m_d3dDepthStencilBuffer);
    SafeRelease(m_d3dDepthStencilState);
    SafeRelease(m_d3dRasterizerState);
    SafeRelease(m_d3dSwapChain);
    SafeRelease(m_d3dDeviceContext);
    SafeRelease(m_d3dDevice);
}