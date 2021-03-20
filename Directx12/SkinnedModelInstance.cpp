
#include "SkinnedModelInstance.h"



void SkinnedModelInstance::UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos)
{
	// 해당 Animation에 대해 갱신을 한다.
	SkinnedInfo->GetFinalTransforms(_eAnimation, timePos, FinalTransforms);
}
