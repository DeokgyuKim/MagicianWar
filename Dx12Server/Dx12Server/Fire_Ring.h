#pragma once
#include "Skill.h"
class Fire_Ring :
    public Skill
{
    // Skill��(��) ���� ��ӵ�
    virtual void initialize() override;
    virtual int Update() override;
    virtual int LateUpdate() override;

    void setDissolveC(float _variable) { m_DissolveC = _variable; }

private:
    float m_DissolveC;
};

