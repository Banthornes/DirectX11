#pragma once

struct VertexPosColor
{
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT3 Color;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    bool InitializeBuffers(ID3D11Device *d3dDevice);
    void CleanupBuffers();

    void SetPosition(const float x, const float y, const float z);
    void SetPosition(const DirectX::XMFLOAT3 position);
    virtual void Update(const float deltaTime);
//protected:
    std::vector<VertexPosColor> m_vertices;
    std::vector<WORD> m_indices;

//private:
    DirectX::XMMATRIX m_matrix;
    // Vertex buffer data
    ID3D11Buffer* m_d3dVertexBuffer = nullptr;
    ID3D11Buffer* m_d3dIndexBuffer = nullptr;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
};