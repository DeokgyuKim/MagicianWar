#pragma once

#include "framework.h"
#include "protocol.h"

class Bullet
{
public:
	Bullet();
	Bullet(const Bullet& rhs);
	Bullet operator=(Bullet& rhs)
	{
		m_Info.user = rhs.m_Info.user;
		m_Info.Damage = rhs.m_Info.Damage;
		m_Info.ElementType = rhs.m_Info.ElementType;
		m_Info.matWorld = rhs.m_Info.matWorld;
		m_Info.Team = rhs.m_Info.Team;
		m_TotalLifeTime = rhs.m_TotalLifeTime;
		m_lifeTime = rhs.m_lifeTime;
		m_speed = rhs.m_speed;

		m_xmfScale = rhs.m_xmfScale;
		m_xmfRotate = rhs.m_xmfRotate;
		m_xmfPosition = rhs.m_xmfPosition;
		m_dir = rhs.m_dir;
		m_Dead = false;
		Initialize();

		return *this;
	}
	~Bullet() { /*Release();*/ }
public:
	void Release();
public:
	void Initialize();
	int Update(const float gTime);
	void LateUpdate(const float gTime);

public:
	// mutex
	void Lock() { m_mutex.lock(); }
	void Unlock() { m_mutex.unlock(); }

	// Set
	void SetUser(int _user) { m_Info.user = _user; }
	void setElementType(unsigned char _element);
	void setWorldMatrix(XMFLOAT4X4 _matWorld);
	void setCheckUserTeam(unsigned char _team) { m_Info.Team = _team; }
	void setLifeTime(float _lifeTime) { m_lifeTime = _lifeTime; }
	void setTotalLifeTime(float _lifeTime) { m_TotalLifeTime = _lifeTime; }
	void setSpeed(float _speed) { m_speed = _speed; }
	void setScale(XMFLOAT3 _scale) { m_xmfScale = _scale; }
	void setRotate(XMFLOAT3 _rotate) { m_xmfRotate = _rotate; }
	void SetDead(bool _dead) { m_Dead = _dead; }
	void setPosition(XMFLOAT3 _position) { m_xmfPosition = _position; }
	void setDirection(XMFLOAT3 _dir) { m_dir = _dir; }
	void setDamage(int _damage) { m_Info.Damage = _damage; }

	// Get
	int getUser() const { return m_Info.user; }
	unsigned char getCheckUserTeam() const { return m_Info.Team; }
	unsigned char getElementType() const { return m_Info.ElementType; }
	XMFLOAT4X4 getWorld() const { return m_Info.matWorld; }
	float getLifeTime() const { return m_lifeTime; }
	float getTotalLifeTime() const { return m_TotalLifeTime; }
	float getSpeed() const { return m_speed; }
	int getDamage() const { return m_Info.Damage; }
	PxRigidDynamic* GetRigidDynamic() { return m_pRigidDynamic; }

private:
	BulletInfo m_Info;

	bool m_Dead = false;
	float m_lifeTime;
	float m_TotalLifeTime;
	float m_speed;
	XMFLOAT3    m_xmfScale;
	XMFLOAT3	m_xmfRotate;
	XMFLOAT3    m_xmfPosition;

	mutex m_mutex;

	XMFLOAT3 m_dir;
	PxRigidDynamic* m_pRigidDynamic = NULL;

};
