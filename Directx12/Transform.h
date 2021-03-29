#pragma once
#include "Component.h"
#include <ctime>

class Transform :
    public Component
{
public:
    Transform(XMFLOAT3 xmfScale, XMFLOAT3 xmfRotate, XMFLOAT3 xmfPosition);
    ~Transform();
public:
    virtual void LateUpdate(const float& fTimeDelta) override;
public:
    int        KeyInput(DWORD iKey);
public:
    XMFLOAT3    GetScale() { return m_xmfScale; }
    XMFLOAT3    GetRotate() { return m_xmfRotate; }
    XMFLOAT3    GetPosition() { return m_xmfPosition; }
    XMFLOAT3    GetLook();
    XMMATRIX    GetWorldMatrix(); 
public:
    void        SetScale(XMFLOAT3 xmfSclae) { m_xmfScale = xmfSclae; }
    void        SetRotate(XMFLOAT3 xmfRotate) { m_xmfRotate = xmfRotate; }
    void        SetPosition(XMFLOAT3 xmfPosition) { m_xmfPosition = xmfPosition; }
    void        SetMeshRotate(XMFLOAT3 xmfMeshRotate) { m_xmfMeshRotate = xmfMeshRotate; }
public:
    void        MoveForward(float speed);
private:
    XMFLOAT3    m_xmfScale;
    XMFLOAT3	m_xmfRotate;
    XMFLOAT3    m_xmfPosition;
    XMFLOAT4X4	m_xmmWorld;

    XMFLOAT3    m_xmfMeshRotate;

    bool m_bJump;

    // Èû
    float m_fJumpTotal; 
    float m_fJumpPower;
    float Gravity;

    // ½Ã°£
    clock_t Current_tick;
    clock_t Next_tick;
    DWORD   InputKey = 0;

};

