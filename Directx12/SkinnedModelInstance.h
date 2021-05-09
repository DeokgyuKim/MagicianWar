#pragma once

#include "SkinnedData.h"

class SkinnedModelInstance
{
public:
	SkinnedModelInstance() {}
	SkinnedModelInstance(const SkinnedModelInstance& rhs);
public:
	void UpdateAnimation(ANIMATION_TYPE _eAnimation, float timePos);
	void UpdateBlendAnimation(
		ANIMATION_TYPE _curAnimate, float _curtimePos,
		ANIMATION_TYPE _nextAnimate, float _nexttimePos, float blendWeight);
public:
	unique_ptr<SkinnedData> SkinnedInfo = nullptr;
	vector<XMFLOAT4X4> FinalTransforms;


};

