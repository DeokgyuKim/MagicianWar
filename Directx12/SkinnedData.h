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
/// 본애니메이션은 키프레임의 리스트로 이루어져 있다.  
/// 두개의 시간 값이 두 키 프레임 사이에 있다면, 
/// 두 키프레임 사이의 시간을 보간한다.
///
/// 애니메이션은 항상 최소 2개의 키프레임을 가진다고 가정한다.
///</summary>
struct BoneAnimation
{
	float GetStartTime()const;
	float GetEndTime()const;

	void Interpolate(float t, DirectX::XMFLOAT4X4& M)const;

	// 이전 키프레임과 현재 키프레임이 필요함
	std::vector<Keyframe> Keyframes;
};

///<summary>
/// AnimationClip 클래스는 '걷기', '뛰기', '공격', '방어' 같은 
/// 개별 애니메이션 클립을 대표한다.
/// 하나의 AnimationClip 객체는 
/// 애니메이션 클립을 구성하는 BoneAnimation 인스턴스들(뼈대당 하나씩)을 담는다.
///</summary>
struct AnimationClip
{
	// 이 클립의 모든 뼈대 중 가장 이른 시작 시간을 돌려준다.
	float GetClipStartTime()const;

	// 이 클립의 모든 뼈대 중 가장 늦은 종료 시간을 돌려준다.
	float GetClipEndTime()const;

	// 이 클립의 각 BoneAnimation을 흝으면서 애니메이션을 보간한다.
	void Interpolate(float t, std::vector<DirectX::XMFLOAT4X4>& boneTransforms)const;

	// 이 클립을 구성하는 뼈대별 애니메이션들.
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

	// 실제 프로젝트에서는, 만일 같은 clipName과 timePos로
	// 이 메서드를 여러 번 호출할 가능성이 크다면
	// 일종의 캐싱 시스템을 도입하는 것이 바람직할 것이다.
	// i번 뼈대의 부모의 부모의 오프셋 변환을 얻는 코드
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
