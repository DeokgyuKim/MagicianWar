
#include "SkinnedModelInstance.h"



void SkinnedModelInstance::UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos)
{
	// �ش� Animation�� ���� ������ �Ѵ�.
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
