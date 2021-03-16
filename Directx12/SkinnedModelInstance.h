#pragma once

#include "SkinnedData.h"

class SkinnedModelInstance
{
public:
	void UpdateAnimation(float time);

public:
	unique_ptr<SkinnedData> SkinnedInfo = nullptr;
	vector<XMFLOAT4X4> Transforms;

};

