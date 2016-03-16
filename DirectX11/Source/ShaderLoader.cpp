#include "DirectX11PCH.h"
#include "ShaderLoader.h"

template<>
std::string ShaderLoader::GetLatestProfile<ID3D11VertexShader>(ID3D11Device *d3dDevice)
{
    assert(d3dDevice);

    // Query the current feature level:
    D3D_FEATURE_LEVEL featureLevel = d3dDevice->GetFeatureLevel();

    switch (featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "vs_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "vs_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "vs_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "vs_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "vs_4_0_level_9_1";
    }
    break;
    } // switch( featureLevel )

    return "";
}

template<>
std::string ShaderLoader::GetLatestProfile<ID3D11PixelShader>(ID3D11Device *d3dDevice)
{
    assert(d3dDevice);

    // Query the current feature level:
    D3D_FEATURE_LEVEL featureLevel = d3dDevice->GetFeatureLevel();
    switch (featureLevel)
    {
    case D3D_FEATURE_LEVEL_11_1:
    case D3D_FEATURE_LEVEL_11_0:
    {
        return "ps_5_0";
    }
    break;
    case D3D_FEATURE_LEVEL_10_1:
    {
        return "ps_4_1";
    }
    break;
    case D3D_FEATURE_LEVEL_10_0:
    {
        return "ps_4_0";
    }
    break;
    case D3D_FEATURE_LEVEL_9_3:
    {
        return "ps_4_0_level_9_3";
    }
    break;
    case D3D_FEATURE_LEVEL_9_2:
    case D3D_FEATURE_LEVEL_9_1:
    {
        return "ps_4_0_level_9_1";
    }
    break;
    }
    return "";
}

template<>
ID3D11VertexShader* ShaderLoader::CreateShader<ID3D11VertexShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device *d3dDevice)
{
    assert(d3dDevice);
    assert(pShaderBlob);

    ID3D11VertexShader* pVertexShader = nullptr;
    d3dDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader);

    return pVertexShader;
}

template<>
ID3D11PixelShader* ShaderLoader::CreateShader<ID3D11PixelShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device *d3dDevice)
{
    assert(d3dDevice);
    assert(pShaderBlob);

    ID3D11PixelShader* pPixelShader = nullptr;
    d3dDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pPixelShader);

    return pPixelShader;
}

template< class ShaderClass >
ShaderClass* ShaderLoader::LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& _profile)
{
    ID3DBlob* pShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;
    ShaderClass* pShader = nullptr;

    std::string profile = _profile;
    if (profile == "latest")
    {
        profile = GetLatestProfile<ShaderClass>();
    }

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), profile.c_str(),
        flags, 0, &pShaderBlob, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
            OutputDebugStringA(errorMessage.c_str());

            SafeRelease(pShaderBlob);
            SafeRelease(pErrorBlob);
        }

        return false;
    }

    pShader = CreateShader<ShaderClass>(pShaderBlob, nullptr);

    SafeRelease(pShaderBlob);
    SafeRelease(pErrorBlob);

    return pShader;
}
