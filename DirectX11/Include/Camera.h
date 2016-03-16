#pragma once

class Camera
{
public:
    Camera();

    void SetPosition(float x, float y, float z);
    void SetPosition(DirectX::XMFLOAT3 position);
    void SetRotation(float x, float y, float z);
    void SetRotation(DirectX::XMFLOAT3 rotation);

    DirectX::XMFLOAT3 GetPosition();
    DirectX::XMFLOAT3 GetRotation();

    DirectX::XMMATRIX GetViewMatrix();
    
    void Update();
//private:
    DirectX::XMMATRIX m_viewMatrix;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
};