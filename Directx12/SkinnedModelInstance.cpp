
#include "SkinnedModelInstance.h"



SkinnedModelInstance::SkinnedModelInstance(const SkinnedModelInstance& rhs)
{
	SkinnedInfo = make_unique<SkinnedData>(*rhs.SkinnedInfo.get());
	FinalTransforms.resize((int)rhs.FinalTransforms.size());
	copy(rhs.FinalTransforms.begin(), rhs.FinalTransforms.end(), FinalTransforms.begin());
	
}

void SkinnedModelInstance::UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos)
{
	// 해당 Animation에 대해 갱신을 한다.
	SkinnedInfo->GetFinalTransforms(_eAnimation, timePos, FinalTransforms);
}

void SkinnedModelInstance::UpdateBlendAnimation(ANIMATION_TYPE _curAnimate, float _curtimePos, ANIMATION_TYPE _nextAnimate, float _nexttimePos, float blendWeight)
{
	SkinnedInfo->GetBlendedFinalTransforms(
		_curAnimate, _curtimePos,
		_nextAnimate, _nexttimePos,
		blendWeight, FinalTransforms
	);
}
