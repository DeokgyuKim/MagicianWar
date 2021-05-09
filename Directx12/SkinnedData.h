///<summary>
/// A Keyframe defines the bone transformation at an instant in time.
///</summary>

#include "framework.h"

struct Keyframe
{
	Keyframe();
	~Keyframe();

	float TimePos;
	DirectX::XMFLOAT3 Translation;
	DirectX::XMFLOAT3 Scale;
	DirectX::XMFLOAT4 RotationQuat;

};

///<summary>
/// ���ִϸ��̼��� Ű�������� ����Ʈ�� �̷���� �ִ�.  
/// �ΰ��� �ð� ���� �� Ű ������ ���̿� �ִٸ�, 
/// �� Ű������ ������ �ð��� �����Ѵ�.
///
/// �ִϸ��̼��� �׻� �ּ� 2���� Ű�������� �����ٰ� �����Ѵ�.
///</summary>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	// ���� Ű�����Ӱ� ���� Ű�������� �ʿ���
	std::vector<Keyframe> Keyframes;
};

///<summary>
/// AnimationClip Ŭ������ '�ȱ�', '�ٱ�', '����', '���' ���� 
/// ���� �ִϸ��̼� Ŭ���� ��ǥ�Ѵ�.
/// �ϳ��� AnimationClip ��ü�� 
/// �ִϸ��̼� Ŭ���� �����ϴ� BoneAnimation �ν��Ͻ���(����� �ϳ���)�� ��´�.
///</summary>
struct AnimationClip
{
	// �� Ŭ���� ��� ���� �� ���� �̸� ���� �ð��� �����ش�.
	float GetClipStartTime()const;

	// �� Ŭ���� ��� ���� �� ���� ���� ���� �ð��� �����ش�.
	float GetClipEndTime()const;

	// �� Ŭ���� �� BoneAnimation�� �y���鼭 �ִϸ��̼��� �����Ѵ�.
	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

	// �� Ŭ���� �����ϴ� ���뺰 �ִϸ��̼ǵ�.
	std::vector<BoneAnimation> BoneAnimations;
};

class SkinnedData
{
public:
	SkinnedData() {}
	SkinnedData(const SkinnedData& rhs) {
		for (auto o = rhs.mAnimations.begin(); o != rhs.mAnimations.end(); ++o) {
			mAnimations[o->first] = o->second;
		}
		//copy(rhs.mAnimations.begin(), rhs.mAnimations.end(), mAnimations.begin());
		mBoneHierarchy.resize(rhs.mBoneHierarchy.size());
		copy(rhs.mBoneHierarchy.begin(), rhs.mBoneHierarchy.end(), mBoneHierarchy.begin());
		mBoneName.resize(rhs.mBoneName.size());
		copy(rhs.mBoneName.begin(), rhs.mBoneName.end(), mBoneName.begin());
		mBoneOffsets.resize(rhs.mBoneOffsets.size());
		copy(rhs.mBoneOffsets.begin(), rhs.mBoneOffsets.end(), mBoneOffsets.begin());
		mSubmeshOffset.resize(rhs.mSubmeshOffset.size());
		copy(rhs.mSubmeshOffset.begin(), rhs.mSubmeshOffset.end(), mSubmeshOffset.begin());
		m_ToRootTransforms.resize(rhs.m_ToRootTransforms.size());
		copy(rhs.m_ToRootTransforms.begin(), rhs.m_ToRootTransforms.end(), m_ToRootTransforms.begin());
		//mAnimations = rhs.mAnimations;
		//mBoneHierarchy = rhs.mBoneHierarchy;
		//mBoneName = rhs.mBoneName;
		//mBoneOffsets = rhs.mBoneOffsets;
		//mSubmeshOffset = rhs.mSubmeshOffset;
		//m_ToRootTransforms = rhs.m_ToRootTransforms;
	}
public:
	// Get
	UINT GetBoneCount()const;

	float GetClipStartTime(const ANIMATION_TYPE eType)const;
	float GetClipEndTime(const ANIMATION_TYPE eType)const;

	// ���� ������Ʈ������, ���� ���� clipName�� timePos��
	// �� �޼��带 ���� �� ȣ���� ���ɼ��� ũ�ٸ�
	// ������ ĳ�� �ý����� �����ϴ� ���� �ٶ����� ���̴�.
	// i�� ������ �θ��� �θ��� ������ ��ȯ�� ��� �ڵ�
	void GetFinalTransforms(const ANIMATION_TYPE eType, float timePos, std::vector<DirectX::XMFLOAT4X4>& finalTransforms);
	void GetBlendedFinalTransforms(
		const ANIMATION_TYPE _curAnimation, float _curtimePos,
		const ANIMATION_TYPE _nextAnimation, float _nexttimePos,
		float _blendWeight, std::vector<DirectX::XMFLOAT4X4>& finalTransform);
	std::vector<std::string> GetBoneName() const;

public:
	// Set
	void SetBoneName(std::string boneName);

	void SetAnimation(AnimationClip inAnimation, ANIMATION_TYPE eType);
	void SetSubmeshOffset(int num);
	std::vector<int> GetSubmeshOffset() const;

	void Set(
		std::vector<int>& boneHierarchy,
		std::vector<DirectX::XMFLOAT4X4>& boneOffsets,
		std::unordered_map<ANIMATION_TYPE, AnimationClip>* animations = nullptr);

public:
	std::vector<std::string> mBoneName;
	std::vector<int> mBoneHierarchy;
	std::vector<DirectX::XMFLOAT4X4> mBoneOffsets;
	std::vector<XMFLOAT4X4> m_ToRootTransforms;

	std::vector<std::string> mAnimationName;
	std::unordered_map<ANIMATION_TYPE, AnimationClip> mAnimations;

	std::vector<int> mSubmeshOffset;
};
