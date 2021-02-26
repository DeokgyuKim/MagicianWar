#pragma once

struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct UvVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Uv;
};

struct ObjectCB
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};