
#include "SkinnedModelInstance.h"



void SkinnedModelInstance::UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos)
{
	// �ش� Animation�� ���� ������ �Ѵ�.
	SkinnedInfo->GetFinalTransforms(_eAnimation, timePos, FinalTransforms);
}
