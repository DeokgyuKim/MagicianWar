#pragma once

#include "framework.h"
#include "Global.h"

class Skill
{
public:
	Skill();
	virtual ~Skill();

public:
	virtual int Update(const float& fTime);
	virtual void LateUpdate(const float& fTime);

public:
	void setUser(int user) { m_Info.user = user; }
	void setPosition(XMFLOAT3 pos);
	void setRotate(XMFLOAT3 rotate);
	void setScale(XMFLOAT3 scale);
	void setWorld(XMFLOAT4X4 world);
	void setLifeTime(float time) { m_Info.LifeTime = time; }
	void setSkillType(unsigned char skill_type) { m_Info.SkillType = skill_type; }
	void setSlotNum(unsigned char _slotNum) { slotNum = _slotNum; }

	XMFLOAT3 getRotate() { return m_Info.xmfRotate; }
	XMFLOAT3 getPosition() { return m_Info.xmfPosition; }
	XMFLOAT3 getScale() { return m_Info.xmfScale; }
	XMFLOAT4X4 getWorld() { return m_Info.xmmWorld; }
	float getLifeTime() { return m_Info.LifeTime; }
	int getUser() { return m_Info.user; }
	unsigned char getSlotNum() { return slotNum; }
	unsigned char getSkillType() { return m_Info.SkillType; }
protected:
	SkillInfo m_Info;
	unsigned char slotNum;


};

