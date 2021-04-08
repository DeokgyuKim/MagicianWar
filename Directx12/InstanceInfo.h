#pragma once
#include "framework.h"

class InstanceInfo
{ // instance data
public:
	InstanceInfo(string instanceID, UINT instanceIndex);
	~InstanceInfo() {}
	
	void AddInstance(string instanceID, UINT istanceIndex);
	UINT GetInstanceIndex(string instanceID) { return m_InstanceMap[instanceID]; }

private:
	map<string, UINT> m_InstanceMap;
	UINT InstanceCount; // 인스턴스 갯수

};

