#pragma once

#include "SkinnedData.h"

class SkinnedModelInstance
{
public:
	void UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos);

public:
	unique_ptr<SkinnedData> SkinnedInfo = nullptr;
	vector<XMFLOAT4X4> FinalTransforms;
	float TimePos = 0.0f;
	ANIMATION_TYPE eAnimation;

};

