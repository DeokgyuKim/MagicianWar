#include "InstanceInfo.h"

InstanceInfo::InstanceInfo(string instanceID)
	: InstanceCount(0)
{
	m_InstanceMap[instanceID] = InstanceCount++; // 첫 생성 ㅊㅊ

}

void InstanceInfo::AddInstance(string instanceID)
{
	auto iter = m_InstanceMap.find(instanceID);
	if (iter == m_InstanceMap.end()) {
		return; // 없음
	}
	m_InstanceMap[instanceID] = InstanceCount++; // 두번째 생성부터는 ++Instance
}
