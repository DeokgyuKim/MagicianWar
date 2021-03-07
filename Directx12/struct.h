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

struct ObjectCB
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
};

struct MeshInfo
{
	int VtxCnt;
	int IdxCnt;
	//버텍스 정보, 인덱스 정보
};