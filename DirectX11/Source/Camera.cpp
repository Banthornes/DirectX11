#include "DirectX11PCH.h"
#include "Camera.h"

Camera::Camera()
{
    m_position = DirectX::XMFLOAT3(0, 0, -10);
}

void Camera::SetPosition(float x, float y, float z)
{
    SetPosition(DirectX::XMFLOAT3(x, y, z));
}

void Camera::SetPosition(DirectX::XMFLOAT3 position)
{
    m_position = position;
}

void Camera::SetRotation(float x, float y, float z)
{
    SetRotation(DirectX::XMFLOAT3(x, y, z));
}

void Camera::SetRotation(DirectX::XMFLOAT3 rotation)
{
    m_rotation = rotation;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
    return m_position;
}

DirectX::XMFLOAT3 Camera::GetRotation()
{
    return m_rotation;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
    return m_viewMatrix;
}

void Camera::Update()
{
    DirectX::XMVECTOR eyePosition = DirectX::XMVectorSet(0, 0, -10, 1);
    DirectX::XMVECTOR focusPoint = DirectX::XMVectorSet(0, 0, 0, 1);
    DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0, 1, 0, 0);
    m_viewMatrix = DirectX::XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);

    //DirectX::XMFLOAT3 up(0.0f, 1.0f, 0.0f);
    //DirectX::XMFLOAT3 position(m_position);
    //DirectX::XMFLOAT3 lookAt(0.0f, 0.0f, 1.0f);
    //
    //float pitch = m_rotation.x * 0.0174532925f;
    //float yaw = m_rotation.y * 0.0174532925f;
    //float roll = m_rotation.z = 0.0174532925f;
    //
    //DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
    //
    //DirectX::XMStoreFloat3(&lookAt, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&lookAt), rotationMatrix));
    //DirectX::XMStoreFloat3(&up, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&up), rotationMatrix));
    //
    //DirectX::XMStoreFloat3(&lookAt, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&lookAt)));
    //
    //m_viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&position), DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&up));
}
