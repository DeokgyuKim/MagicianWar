#pragma once

struct ColorVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT3 Normal;
};


struct UvVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Uv;
	XMFLOAT3 Normal;
};

struct LightVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Uv;
};

struct ObjectCB
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};
struct MatCB
{
	XMFLOAT4 Material = XMFLOAT4(0.f, 0.f, 0.f, 1.f);
};

struct MeshInfo
{
	int VtxCnt;
	int IdxCnt;
	//버텍스 정보, 인덱스 정보
};