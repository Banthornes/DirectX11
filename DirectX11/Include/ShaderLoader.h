#pragma once

#include "Shader.h"
#include <d3d11.h>

class ShaderLoader
{
public:
    // Get the latest profile for the specified shader type.
    template< class ShaderClass >
    std::string GetLatestProfile(ID3D11Device* d3dDevice);

    template< class ShaderClass >
    ShaderClass* CreateShader(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage, ID3D11Device *d3dDevice);

    template< class ShaderClass >
    ShaderClass* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& profile);

    
private:
};