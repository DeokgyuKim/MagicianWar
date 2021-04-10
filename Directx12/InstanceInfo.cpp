#include "InstanceInfo.h"

InstanceInfo::InstanceInfo(string instanceID)
	: InstanceCount(0)
{
	m_InstanceMap[instanceID] = InstanceCount++; // ù ���� ����

}

void InstanceInfo::AddInstance(string instanceID)
{
	auto iter = m_InstanceMap.find(instanceID);
	if (iter == m_InstanceMap.end()) {
		return; // ����
	}
	m_InstanceMap[instanceID] = InstanceCount++; // �ι�° �������ʹ� ++Instance
}
