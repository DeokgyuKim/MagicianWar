#pragma once
#include "framework.h"

class InstanceInfo
{ // instance data
public:
	InstanceInfo(string instanceID);
	~InstanceInfo() {}

	void AddInstance(string instanceIDx);
public:
	// Get
	UINT GetInstanceIndex(string instanceID) { return m_InstanceMap[instanceID]; }
	UINT GetInstanceCount() { return InstanceCount; }
	string GetmeshName() { return meshName; }
public:
	// Set
	void SetMeshName(string _name) { meshName = _name; }

private:
	map<string, UINT> m_InstanceMap; // <meshName, Count>
	string meshName;
	UINT InstanceCount; // 인스턴스 갯수

};

