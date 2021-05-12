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
	if (_obj->GetInstName() == "" || !_obj->GetIsInstancing()) { // 메시를 다루지 않는 것 - Camera
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

	m_InstanceCBs[_obj->GetInstName()]->CopyData(_obj->GetIndex(), data);
}

void InstanceMgr::UpdateInstanceBuffer(UINT uiIdx, string instname, XMMATRIX world)
{
	if (instname == "") { // 메시를 다루지 않는 것 - Camera
		return;
	}
	ObjectCB data;
	XMStoreFloat4x4(&data.World, XMMatrixTranspose(world));
	data.MaterialIndex = uiIdx;
	
	m_InstanceCBs[instname]->CopyData(uiIdx, data);
}

void InstanceMgr::SetInstaneCount(string instname, UINT iCnt)
{
	auto iter = m_InstanceObjects.find(instname);
	if (iter == m_InstanceObjects.end())
		return;
	(*iter).second->SetInstanceCount(iCnt);
}

void InstanceMgr::UpdateSkinnedBuffers(Object* _obj)
{
	if (_obj->GetInstName() == "" || !_obj->GetIsInstancing() || _obj->GetMeshType() != MESH_TYPE::CHARACTER) { // 캐릭터 외에는 뼈가없어
		return;
	}
	SkinnedCB data;

	XMFLOAT4X4 root = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0];
	XMMATRIX matRoot = XMLoadFloat4x4(&root);
	matRoot = XMMatrixTranspose(matRoot);
	XMStoreFloat4x4(&root, matRoot);

	memcpy(&data.right[0], &root._11, sizeof(XMFLOAT3));
	memcpy(&data.up[0], &root._21, sizeof(XMFLOAT3));
	memcpy(&data.look[0], &root._31, sizeof(XMFLOAT3));
	memcpy(&data.pos[0], &root._41, sizeof(XMFLOAT3));

	root = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1];
	matRoot = XMLoadFloat4x4(&root);
	matRoot = XMMatrixTranspose(matRoot);
	XMStoreFloat4x4(&root, matRoot);
	
	memcpy(&data.right[1], &root._11, sizeof(XMFLOAT3));
	memcpy(&data.up[1], &root._21, sizeof(XMFLOAT3));
	memcpy(&data.look[1], &root._31, sizeof(XMFLOAT3));
	memcpy(&data.pos[1], &root._41, sizeof(XMFLOAT3));

	//data.BoneTransforms[0] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[0]; // Root 뼈대 == 하체 중심
	//data.BoneTransforms[1] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[1]; // Root 뼈대 == 하체 중심
	for (int upper = 2; upper < 27; ++upper) { // 상체
		root = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper];
		matRoot = XMLoadFloat4x4(&root);
		matRoot = XMMatrixTranspose(matRoot);
		XMStoreFloat4x4(&root, matRoot);
		memcpy(&data.right[upper],	&root._11, sizeof(XMFLOAT3));
		memcpy(&data.up[upper],		&root._21, sizeof(XMFLOAT3));
		memcpy(&data.look[upper],	&root._31, sizeof(XMFLOAT3));
		memcpy(&data.pos[upper],	&root._41, sizeof(XMFLOAT3));

		//data.BoneTransforms[upper] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetUpperAniController())->GetSkinnedModellnst()->FinalTransforms[upper];
	}
	for (int Root = 27; Root < 33; ++Root) { // 하체
		root = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root];
		matRoot = XMLoadFloat4x4(&root);
		matRoot = XMMatrixTranspose(matRoot);
		XMStoreFloat4x4(&root, matRoot);
		memcpy(&data.right[Root],	&root._11, sizeof(XMFLOAT3));
		memcpy(&data.up[Root],		&root._21, sizeof(XMFLOAT3));
		memcpy(&data.look[Root],	&root._31, sizeof(XMFLOAT3));
		memcpy(&data.pos[Root],		&root._41, sizeof(XMFLOAT3));
		
		//data.BoneTransforms[Root] = dynamic_cast<AnimationCom*>(dynamic_cast<Player*>(_obj)->GetRootAniController())->GetSkinnedModellnst()->FinalTransforms[Root];
	}




	m_SkinnedCBs[_obj->GetInstName()]->CopyData(_obj->GetIndex(), data);

}


