#include "InstanceInfo.h"

InstanceInfo::InstanceInfo(string instanceID, UINT instanceIndex)
	: InstanceCount(0)
{
	AddInstance(instanceID, instanceIndex);
}

void InstanceInfo::AddInstance(string instanceID, UINT istanceIndex)
{
	if (m_InstanceMap.find(instanceID) == m_InstanceMap.end()) {
		m_InstanceMap[instanceID] = istanceIndex;

		InstanceCount += 1;	
	}
}
