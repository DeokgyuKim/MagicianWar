#pragma once

struct Indices
{
	unsigned int i1, i2, i3;
};
struct MeshVtxInfo
{
	unsigned long		ulVertexCnt;
	vector<XMFLOAT3>	vecVtxPos;
	vector<Indices>		vecIdx;
};

struct TransformStruct
{
	XMFLOAT3 xmfPosition;
	XMFLOAT3 xmfRotate;
	XMFLOAT3 xmfScale;
};

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexC;
	XMFLOAT3 Tangent;
	XMFLOAT3 Binormal;
};

struct Material
{
	std::string Name;

	// Material constant buffer data used for shading.
	XMFLOAT3 Ambient = { 0.f,0.f,0.f };
	XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 0.01f, 0.01f, 0.01f };

	int MaterialIndex = -1;

	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};