#include "DirectX11PCH.h"
#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
    if (m_d3dIndexBuffer)
    {
        m_d3dIndexBuffer->Release();
        m_d3dIndexBuffer = NULL;
    }

    if (m_d3dVertexBuffer)
    {
        m_d3dVertexBuffer->Release();
        m_d3dVertexBuffer = NULL;
    }
}

// Helper for creating a D3D vertex or index buffer.
template<typename T>
static void CreateBuffer(_In_ ID3D11Device* device, T const& data, D3D11_BIND_FLAG bindFlags, _Outptr_ ID3D11Buffer** pBuffer)
{
    assert(pBuffer != 0);

    D3D11_BUFFER_DESC bufferDesc = { 0 };

    bufferDesc.ByteWidth = (UINT)data.size() * sizeof(T::value_type);
    bufferDesc.BindFlags = bindFlags;
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA dataDesc = { 0 };

    dataDesc.pSysMem = data.data();

    HRESULT hr = device->CreateBuffer(&bufferDesc, &dataDesc, pBuffer);
    if (FAILED(hr))
    {
        throw std::exception("Failed to create buffer.");
    }
}

bool Mesh::InitializeBuffers(ID3D11Device * d3dDevice)
{
    //// Create an initialize the vertex buffer.
    //D3D11_BUFFER_DESC vertexBufferDesc;
    //ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    //
    //vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    //vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * m_vertices.size();
    //vertexBufferDesc.CPUAccessFlags = 0;
    //vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //
    //D3D11_SUBRESOURCE_DATA resourceData;
    //ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));
    //
    //resourceData.pSysMem = &m_vertices.data;
    //
    //HRESULT hr = d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &m_d3dVertexBuffer);
    //if (FAILED(hr))
    //{
    //    return false;
    //}
    //
    //// Create and initialize the index buffer.
    //D3D11_BUFFER_DESC indexBufferDesc;
    //ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
    //
    //indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    //indexBufferDesc.ByteWidth = sizeof(WORD) * m_indices.size();
    //indexBufferDesc.CPUAccessFlags = 0;
    //indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    //resourceData.pSysMem = &m_indices[0];
    //
    //hr = d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &m_d3dIndexBuffer);
    //if (FAILED(hr))
    //{
    //    return false;
    //}

    CreateBuffer(d3dDevice, m_vertices, D3D11_BIND_VERTEX_BUFFER, &m_d3dVertexBuffer);
    CreateBuffer(d3dDevice, m_indices, D3D11_BIND_INDEX_BUFFER, &m_d3dIndexBuffer);

    return true;
}

void Mesh::CleanupBuffers()
{
    if (m_d3dIndexBuffer)
    {
        m_d3dIndexBuffer->Release();
        m_d3dIndexBuffer = 0;
    }

    if (m_d3dVertexBuffer)
    {
        m_d3dVertexBuffer->Release();
        m_d3dVertexBuffer = 0;
    }
}

void Mesh::SetPosition(const float x, const float y, const float z)
{
    m_position = DirectX::XMFLOAT3(x, y, z);
}

void Mesh::SetPosition(const DirectX::XMFLOAT3 position)
{
    m_position = position;
}

void Mesh::Update(const float /*deltaTime*/)
{
}
