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
#include "Animation.h"
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
		if (iter.second->GetmeshType() == MESH_TYPE::CHARACTER) {  // 캐릭터는 뼈를 가지고 있다.
			m_SkinnedCBs[iter.second->GetmeshName()] = make_unique< UploadBuffer<SkinnedCB>>(m_pDevice, iter.second->GetInstanceCount(), false);
		}
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
	
	if (_obj->GetMeshType() == MESH_TYPE::CHARACTER)
		data.boolBone = 1;
	else
		data.boolBone = 0;
	
	m_InstanceCBs[_obj->GetMeshName()]->CopyData(_obj->GetIndex(), data);
}



void InstanceMgr::UpdateSkinnedBuffers(Object* _obj)
{
	if (_obj->GetMeshName() == "" || !_obj->GetIsInstancing() || _obj->GetMeshType() != MESH_TYPE::CHARACTER) { // 캐릭터 외에는 뼈가없어
		return;
	}
	SkinnedCB data;

	memcpy(&data.right[0], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]._11, sizeof(XMFLOAT3));
	memcpy(&data.up[0], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]._21, sizeof(XMFLOAT3));
	memcpy(&data.look[0], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]._31, sizeof(XMFLOAT3));
	memcpy(&data.pos[0], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]._41, sizeof(XMFLOAT3));
	
	memcpy(&data.right[1], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]._11, sizeof(XMFLOAT3));
	memcpy(&data.up[1], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]._21, sizeof(XMFLOAT3));
	memcpy(&data.look[1], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]._31, sizeof(XMFLOAT3));
	memcpy(&data.pos[1], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]._41, sizeof(XMFLOAT3));

	//data.BoneTransforms[0] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]; // Root 뼈대 == 하체 중심
	//data.BoneTransforms[1] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]; // Root 뼈대 == 하체 중심
	for (int upper = 2; upper < 27; ++upper) { // 상체
		memcpy(&data.right[upper], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper]._11, sizeof(XMFLOAT3));
		memcpy(&data.up[upper], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper]._21, sizeof(XMFLOAT3));
		memcpy(&data.look[upper], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper]._31, sizeof(XMFLOAT3));
		memcpy(&data.pos[upper], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper]._41, sizeof(XMFLOAT3));

		//data.BoneTransforms[upper] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper];
	}
	for (int Root = 27; Root < 33; ++Root) { // 하체

		memcpy(&data.right[Root], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root]._11, sizeof(XMFLOAT3));
		memcpy(&data.up[Root], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root]._21, sizeof(XMFLOAT3));
		memcpy(&data.look[Root], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root]._31, sizeof(XMFLOAT3));
		memcpy(&data.pos[Root], &dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root]._41, sizeof(XMFLOAT3));
		
		//data.BoneTransforms[Root] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root];
	}




	m_SkinnedCBs[_obj->GetMeshName()]->CopyData(_obj->GetIndex(), data);

}


