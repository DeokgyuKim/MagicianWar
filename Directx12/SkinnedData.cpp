
#include "SkinnedData.h"

Keyframe::Keyframe()
{
}

Keyframe::~Keyframe()
{
}

float BoneAnimation::GetStartTime() const
{
	return 0.0f;
}

float BoneAnimation::GetEndTime() const
{
	return 0.0f;
}

void BoneAnimation::Interpolate(float t, DirectX::XMFLOAT4X4& M) const
{
}

float AnimationClip::GetClipStartTime() const
{
	// Find smallest start time over all bones in this clip.
	float t = MathHelper::Infinity;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Min(t, BoneAnimations[i].GetStartTime());
	}

	return t;
}

float AnimationClip::GetClipEndTime() const
{
	// Find largest end time over all bones in this clip.
	float t = 0.0f;
	for (UINT i = 0; i < BoneAnimations.size(); ++i)
	{
		t = MathHelper::Max(t, BoneAnimations[i].GetEndTime());
	}

	return t;
}

void AnimationClip::Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms) const
{
}

UINT SkinnedData::BoneCount() const
{
	return mBoneHierarchy.size();
}

float SkinnedData::GetClipStartTime(const std::string& clipName) const
{
	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipStartTime();
}

float SkinnedData::GetClipEndTime(const std::string& clipName) const
{
	if (!mAnimations.count(clipName)) return 0.f;

	auto clip = mAnimations.find(clipName);
	return clip->second.GetClipEndTime();
}

void SkinnedData::GetFinalTransforms(const std::string& clipName, float timePos, std::vector<DirectX::XMFLOAT4X4>& finalTransforms)
{
	if (!mAnimations.count(clipName)) return;
	if (clipName == "") return;

	UINT numBones = mBoneOffsets.size();
	finalTransforms.resize(numBones);

	std::vector<XMFLOAT4X4> toParentTransforms(numBones);

	// 이 클립의 모든 뼈대를 주어진 시간에 맞게 보간한다.
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// 골격 계통구조를 흝으면서 모든 뼈대를 루트공간으로 변환한다.
	//
	// 루트 트랜스폼 초기화
	m_ToRootTransforms.resize(numBones);

	// 뿌리 뼈대의 인덱스는 0이다. 뿌리 뼈대에는 부모가 없으므로,
	// 뿌리 뼈대의 뿌리 변환은 그냥 자신의 로컬 뼈대 변환이다.
	m_ToRootTransforms[0] = toParentTransforms[0];

	// 이제 자식 뼈대들의 루트 변환들을 구한다.
	// 뼈대들을 흝으면서 부모 뼈대의 루트변환을 다음과 같이 조회한다.
	// ! 항상 부모 뼈대가 자식 뼈대보다 앞에 오게 해야한다.
	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		// i번 뼈대의 부모의 번호
		int parentIndex = mBoneHierarchy[i];
		// i번 뼈대의 부모의 번호의 변환행렬
		XMMATRIX parentToRoot = XMLoadFloat4x4(&m_ToRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&m_ToRootTransforms[i], toRoot);
	}


	// 뼈대 오프셋 변환을 먼저 곱해서 최종 변환을 구한다.
	for (UINT i = 0; i < numBones; ++i)
	{
		XMMATRIX offset = XMLoadFloat4x4(&mBoneOffsets[i]);
		XMMATRIX toRoot = XMLoadFloat4x4(&toParentTransforms[i]);
		XMMATRIX finalTransform = XMMatrixMultiply(offset, toRoot);

		XMStoreFloat4x4(&finalTransforms[i], XMMatrixTranspose(finalTransform));
	}
}

void SkinnedData::GetBlendedAnimationData(const std::string& clipName1, float timePos1, const std::string& clipName2, float timePos2, float factor, std::vector<DirectX::XMFLOAT4X4>& finalTransform)
{
}

void SkinnedData::SetBoneName(std::string boneName)
{
	mBoneName.push_back(boneName);
}

std::vector<std::string> SkinnedData::GetBoneName() const
{
	return mBoneName;
}

void SkinnedData::SetAnimation(AnimationClip inAnimation, std::string inClipName)
{
	mAnimations[inClipName] = inAnimation;
}

void SkinnedData::SetSubmeshOffset(int num)
{
	mSubmeshOffset.push_back(num);
}

std::vector<int> SkinnedData::GetSubmeshOffset() const
{
	return mSubmeshOffset;
}

void SkinnedData::Set(std::vector<int>& boneHierarchy, std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
	std::unordered_map<std::string, AnimationClip>* animations)
{
	mBoneHierarchy = boneHierarchy;
	mBoneOffsets = boneOffsets;
	if (animations != nullptr)
	{
		mAnimations = (*animations);
	}
}
