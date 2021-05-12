#pragma once

#include "include.h"
#include <mutex>

class Bullet
{
public:
	Bullet();
	Bullet(const Bullet& rhs);
	~Bullet() { Release(); }
private:
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
	void SetUser(unsigned char _user) { m_User = _user; }
	void setInstanceName(unsigned char _InstanceName) { m_InstanceName = _InstanceName; }
	void setWorldMatrix(XMFLOAT4X4 _matWorld);
	void setLifeTime(float _lifeTime) { m_lifeTime = _lifeTime; }
	void setTotalLifeTime(float _lifeTime) { m_TotalLifeTime = _lifeTime; }
	void setSpeed(float _speed) { m_speed = _speed; }
	void setScale(XMFLOAT3 _scale) { m_xmfScale = _scale; }
	void setRotate(XMFLOAT3 _rotate) { m_xmfRotate = _rotate; }
	void setPosition(XMFLOAT3 _position) { m_xmfPosition = _position; }
	void setDirection(XMFLOAT3 _dir) { m_dir = _dir; }

	// Get
	unsigned char getUser() const { return m_User; }
	unsigned char getInstanceName() const { return m_InstanceName; }
	XMFLOAT4X4 getWorld() const { return m_matWorld; }
	float getLifeTime() const { return m_lifeTime; }
	float getTotalLifeTime() const { return m_TotalLifeTime; }
	float getSpeed() const { return m_speed; }
	PxRigidDynamic* GetRigidDynamic() { return m_pRigidDynamic; }
	



private:
	unsigned char m_User;
	unsigned char m_InstanceName;
	XMFLOAT4X4 m_matWorld;
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

