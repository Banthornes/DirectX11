#include "DirectX11PCH.h"
#include "Cube.h"

Cube::Cube()
    :   m_size(1.0f, 1.0f, 1.0f)
{
    m_position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

Cube::Cube(const DirectX::XMFLOAT3 size, const DirectX::XMFLOAT3 position)
    : m_size(size)
{
    m_position = position;
    DirectX::XMFLOAT3 s = DirectX::XMFLOAT3(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);
    VertexPosColor vertices[8] =
    {
        { DirectX::XMFLOAT3(-s.x,-s.y,-s.z), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
        { DirectX::XMFLOAT3(-s.x, s.y,-s.z), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
        { DirectX::XMFLOAT3( s.x, s.y,-s.z), DirectX::XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
        { DirectX::XMFLOAT3( s.x,-s.y,-s.z), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
        { DirectX::XMFLOAT3(-s.x,-s.y, s.z), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
        { DirectX::XMFLOAT3(-s.x, s.y, s.z), DirectX::XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
        { DirectX::XMFLOAT3( s.x, s.y, s.z), DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
        { DirectX::XMFLOAT3( s.x,-s.y, s.z), DirectX::XMFLOAT3(1.0f, 0.0f, 1.0f) }  // 7
    };

    for (int i = 0; i < 8; ++i)
    {
        m_vertices.push_back(vertices[i]);
    }

    WORD indices[36] =
    {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        4, 5, 1, 4, 1, 0,
        3, 2, 6, 3, 6, 7,
        1, 5, 6, 1, 6, 2,
        4, 0, 3, 4, 3, 7
    };

    for (int i = 0; i < 36; ++i)
    {
        m_indices.push_back(indices[i]);
    }
}

float Cube::GetWidth() const
{
    return m_size.x;
}

float Cube::GetHeight() const
{
    return m_size.y;
}

float Cube::GetDepth() const
{
    return m_size.z;
}

DirectX::XMFLOAT3 Cube::GetSize() const
{
    return m_size;
}

void Cube::Update(const float /*deltaTime*/)
{
    //static float angle = 0.0f;
    //DirectX::XMVECTOR rotationAxis = DirectX::XMVectorSet(0, 1, 1, 0);
    
    //DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationAxis(rotationAxis, DirectX::XMConvertToRadians(angle));
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    m_matrix = rotationMatrix * translationMatrix;

    //angle += 90.0f * deltaTime;
}