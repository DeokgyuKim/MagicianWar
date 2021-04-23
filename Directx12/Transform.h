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

public:
    XMFLOAT3    GetScale() { return m_xmfScale; }
    XMFLOAT3    GetRotate() { return m_xmfRotate; }
    XMFLOAT3    GetPosition() { return m_xmfPosition; }
    XMFLOAT3    GetLook();
    XMMATRIX    GetWorldMatrix(); 
    XMFLOAT4X4* GetWorldMatrixPointer() { return &m_xmmWorld; }
public:
    void        SetScale(XMFLOAT3 xmfSclae) { m_xmfScale = xmfSclae; }
    void        SetRotate(XMFLOAT3 xmfRotate) { m_xmfRotate = xmfRotate; }
    void        SetPosition(XMFLOAT3 xmfPosition) { m_xmfPosition = xmfPosition; }
    void        SetMeshRotate(XMFLOAT3 xmfMeshRotate) { m_xmfMeshRotate = xmfMeshRotate; }
    void        SetWorld(XMFLOAT4X4 world) { m_xmmWorld = world; }
    void        SetParentMatrix(XMFLOAT4X4* pParentWorld) { m_pxmmParentWorld = pParentWorld; }
public:
    void        MoveForward(float speed);
    void        MoveBackward(float speed);
    void        MoveLeft(float speed);
    void        MoveRight(float speed);
    int         Jump(DWORD iKey);
private:
    XMFLOAT3    m_xmfScale;
    XMFLOAT3	m_xmfRotate;
    XMFLOAT3    m_xmfPosition;
    XMFLOAT4X4	m_xmmOriginWorld;
    XMFLOAT4X4	m_xmmWorld;
    XMFLOAT4X4* m_pxmmParentWorld = NULL;

    XMFLOAT3    m_xmfMeshRotate;

    bool m_bJump;

    // Èû
    float m_fJumpTotal; 
    float m_fJumpPower;
    float m_fJumpSpeed;
    

    // ½Ã°£
    clock_t Current_tick;
    clock_t Next_tick;
    DWORD   InputKey = 0;

    float m_jumpForwardSpeed;
    float m_jumpBackwardSpeed;
    float m_jumpLeftSpeed;
    float m_jumpRightSpeed;

};

