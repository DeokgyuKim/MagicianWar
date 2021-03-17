#pragma once


struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
};

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

struct SkinnedVertex// : Vertex
{ // Movable Model
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
	XMFLOAT3 BoneWeights;
	BYTE BoneIndices[4];

	UINT MaterialIndex;
};

struct Material
{
	std::string Name;

	// Material constant buffer data used for shading.
	XMFLOAT3 Ambient = { 0.f,0.f,0.f };
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 0.01f, 0.01f, 0.01f };

	
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct ObjectCB
{
	XMFLOAT4X4 World = MathHelper::Identity4x4();
	XMFLOAT4X4 WorldNoScaling = MathHelper::Identity4x4();
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

struct CameraCB
{
	XMFLOAT4X4	View = MathHelper::Identity4x4();
	XMFLOAT4X4	Proj = MathHelper::Identity4x4();
	XMFLOAT4X4	InvView = MathHelper::Identity4x4();
	XMFLOAT4X4	InvProj = MathHelper::Identity4x4();
	XMFLOAT4	Position = XMFLOAT4(0.f, 0.f, 0.f, 0.f);
};

struct MaterialCB
{
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Specular;
};

struct LightCB
{
	XMFLOAT4	Diffuse;
	XMFLOAT4	Ambient;
	XMFLOAT4	Specular;
	XMFLOAT4	Position;
	XMFLOAT4	Direction;
};

struct SkinnedCB
{
	// 최대 뼈대 Character - 33 
	XMFLOAT4X4 BoneTransforms[96];
};