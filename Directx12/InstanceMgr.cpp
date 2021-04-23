#include "InstanceMgr.h"
#include "InstanceInfo.h"
#include "Object.h"
#include "Skybox.h"
#include "Terrain.h"
#include "Player.h"
#include "StaticObject.h"
#include "Bullet.h"

// Compo
#include "Transform.h"
#include "Material.h"

InstanceMgr* InstanceMgr::m_pInstance = nullptr;

void InstanceMgr::InitInstanceMgr(Core* _core, ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
	m_pCore = _core;
	m_pDevice = _device;
	m_pCmdLst = _cmdList;
}

void InstanceMgr::BuildInstanceBuffers()
{
	for (auto& iter : m_InstanceObjects) {
		m_InstanceCBs[iter.second->GetmeshName()] = make_unique<UploadBuffer<ObjectCB>>(m_pDevice, iter.second->GetInstanceCount(), false);
	}

}

void InstanceMgr::UpdateInstanceBuffer(Object* _obj)
{
	if (_obj->GetMeshName() == "" || !_obj->GetIsInstancing()) { // 메시를 다루지 않는 것 - Camera
		return;
	}
	ObjectCB data;
	data.MaterialIndex = 0;
	XMStoreFloat4x4(&data.World, XMMatrixTranspose(dynamic_cast<Transform*>(_obj->GetTransController())->GetWorldMatrix()));
	data.MaterialIndex = _obj->GetMaterialIndex();
	
	m_InstanceCBs[_obj->GetMeshName()]->CopyData(_obj->GetIndex(), data);
}


