#include "AnimationMgr.h"
#include <fstream>
AnimationMgr* AnimationMgr::m_pInstance = nullptr;

void AnimationMgr::InitAnimationMgr(Core* pCore, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdLst)
{
	m_pCore = pCore;
	m_pDevice = pDevice;
	m_pCmdLst = pCmdLst;
	m_strFilePath = "";
}

void AnimationMgr::BuildAnimation(string meshName, MESH_TYPE eMesh, ANIMATION_TYPE eAnimation)
{
	if (!m_SkinnedModelInsts.count(meshName)) // value 1개니까 
	{
		cout << " Mesh 가 없습니다." << endl;
		return;
	}

	switch (eMesh)
	{
	case MESH_TYPE::CHARACTER:
		m_strFilePath = "..//Resources//Models//Characters//";
		break;
	case MESH_TYPE::COUNT:
		break;
	default:
		break;
	}
	m_strFilePath += meshName + "//Animation//";
	m_meshName = meshName;
	switch (eAnimation)
	{
	case ANIMATION_TYPE::IDLE:
		m_strFilePath += "Idle";
		break;
	case ANIMATION_TYPE::WALK_FOWARD:
		m_strFilePath += "Walk_Forward";
		break;
	case ANIMATION_TYPE::WALK_BACK:
		m_strFilePath += "Walk_Back";
		break;
	case ANIMATION_TYPE::WALK_LEFT:
		m_strFilePath += "Walk_Left";
		break;
	case ANIMATION_TYPE::WALK_RIGHT:
		m_strFilePath += "Walk_Right";
		break;
	case ANIMATION_TYPE::ATTACK:
		m_strFilePath += "Attack";
		break;
	case ANIMATION_TYPE::JUMP:
		m_strFilePath += "Jumping";
	case ANIMATION_TYPE::NONE:
		break;
	}
	m_strFilePath += ".anim";
	bool check;
	check = LoadAnimationFile(eAnimation);
	if (!check) {
		cout << "Animation Load Failed"<<endl;
	}


}

void AnimationMgr::SetSkinnedDatas(string meshName, unique_ptr<SkinnedData> _skinnedinfo)
{
	unique_ptr<SkinnedModelInstance> skinnedModelInst = make_unique<SkinnedModelInstance>();
	skinnedModelInst->SkinnedInfo = move(_skinnedinfo);
	skinnedModelInst->FinalTransforms.resize(skinnedModelInst->SkinnedInfo->GetBoneCount());
	m_SkinnedModelInsts[meshName] = move(skinnedModelInst);
}

bool AnimationMgr::LoadAnimationFile(const ANIMATION_TYPE& eAnimation)
{
	ifstream fileInput(m_strFilePath);

	AnimationClip animationclip;
	UINT boneAnimationSize, keyFrameSize;


	string ignore;
	if (fileInput)
	{
		fileInput >> ignore >> boneAnimationSize;
		fileInput >> ignore >> keyFrameSize;

		vector<XMFLOAT3> vecStartPos;
		for (UINT i = 0; i < boneAnimationSize; ++i) {
			BoneAnimation boneAnim;
			for (UINT j = 0; j < keyFrameSize; ++j) {
				Keyframe key;
				fileInput >> key.TimePos;
				fileInput >> key.Translation.x >> key.Translation.y >> key.Translation.z;
				fileInput >> key.Scale.x >> key.Scale.y >> key.Scale.z;
				fileInput >> key.RotationQuat.x >> key.RotationQuat.y >> key.RotationQuat.z >> key.RotationQuat.w;
				if (i == 0)
					vecStartPos.push_back(key.Translation);
				else
				{
					XMStoreFloat3(&key.Translation, XMLoadFloat3(&key.Translation) - XMLoadFloat3(&vecStartPos[j]));
				}
				boneAnim.Keyframes.push_back(key);
			}
			animationclip.BoneAnimations.push_back(boneAnim);
		}
		m_SkinnedModelInsts[m_meshName]->SkinnedInfo->SetAnimation(animationclip, eAnimation);
		return true;
	}

	return false;
}

