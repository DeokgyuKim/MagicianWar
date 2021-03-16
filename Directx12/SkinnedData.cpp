
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

	// �� Ŭ���� ��� ���븦 �־��� �ð��� �°� �����Ѵ�.
	auto clip = mAnimations.find(clipName);
	clip->second.Interpolate(timePos, toParentTransforms);

	//
	// ��� ���뱸���� �y���鼭 ��� ���븦 ��Ʈ�������� ��ȯ�Ѵ�.
	//
	// ��Ʈ Ʈ������ �ʱ�ȭ
	m_ToRootTransforms.resize(numBones);

	// �Ѹ� ������ �ε����� 0�̴�. �Ѹ� ���뿡�� �θ� �����Ƿ�,
	// �Ѹ� ������ �Ѹ� ��ȯ�� �׳� �ڽ��� ���� ���� ��ȯ�̴�.
	m_ToRootTransforms[0] = toParentTransforms[0];

	// ���� �ڽ� ������� ��Ʈ ��ȯ���� ���Ѵ�.
	// ������� �y���鼭 �θ� ������ ��Ʈ��ȯ�� ������ ���� ��ȸ�Ѵ�.
	// ! �׻� �θ� ���밡 �ڽ� ���뺸�� �տ� ���� �ؾ��Ѵ�.
	for (UINT i = 1; i < numBones; ++i)
	{
		XMMATRIX toParent = XMLoadFloat4x4(&toParentTransforms[i]);

		// i�� ������ �θ��� ��ȣ
		int parentIndex = mBoneHierarchy[i];
		// i�� ������ �θ��� ��ȣ�� ��ȯ���
		XMMATRIX parentToRoot = XMLoadFloat4x4(&m_ToRootTransforms[parentIndex]);

		XMMATRIX toRoot = XMMatrixMultiply(toParent, parentToRoot);

		XMStoreFloat4x4(&m_ToRootTransforms[i], toRoot);
	}


	// ���� ������ ��ȯ�� ���� ���ؼ� ���� ��ȯ�� ���Ѵ�.
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
