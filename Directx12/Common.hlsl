
struct MaterialData
{
	float4   DiffuseAlbedo;
	float    Roughness;
	float4x4 MatTransform;
	uint     DiffuseMapIndex;
	uint     NormalMapIndex;

};


cbuffer cbPerObjectWorld : register(b0)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
	uint gMaterialIndex;
};


cbuffer cbPerObjectView : register(b1)
{
	float4x4 gView;
};

cbuffer cbPerObjectProj : register(b2)
{
	float4x4 gProj;
};

cbuffer cbSkinned : register(b3)
{
	float4x4 gBoneTransforms[96];
};

Texture2D Texture : register(t0);

// ���� �ڷḦ space1�� �����Ѵ�. ���� ���� �ؽ�ó �迭���� ��ġ�� �ʴ´�.
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

SamplerState gsamLinear  : register(s0);