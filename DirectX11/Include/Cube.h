#pragma once

#include "Mesh.h"

class Cube : public Mesh
{
public:
    Cube();
    Cube(const DirectX::XMFLOAT3 size, const DirectX::XMFLOAT3 position);

    float GetWidth() const;
    float GetHeight() const;
    float GetDepth() const;
    DirectX::XMFLOAT3 GetSize() const;

    virtual void Update(const float deltaTime) override;
private:
    DirectX::XMFLOAT3 m_size;
};