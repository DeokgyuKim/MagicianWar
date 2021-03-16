
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

// 재질 자료를 space1에 배정한다. 따라서 위의 텍스처 배열과는 겹치지 않는다.
StructuredBuffer<MaterialData> gMaterialData : register(t0, space1);

SamplerState gsamLinear  : register(s0);