
cbuffer cbCamera : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gInvView;
	float4x4 gInvProj;
	float4 gCamPosition;
	float gTime;
};

cbuffer cbLight : register(b3)
{
	float4 gLightDiffuse;
	float4 gLightAmbient;
	float4 gLightSpecular;
	float4 gLightPosition;
	float4 gLightDirection;
};

cbuffer cbSkinned : register(b4)
{
	float4x4 gBoneTransforms[96];
}


Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);


Texture2D DiffTex : register(t1);
Texture2D AmbiTex : register(t2);
Texture2D SpecTex : register(t3);
Texture2D NormalTex : register(t4);
Texture2D DepthTex : register(t5);

TextureCube CubeMapTex : register(t6);
Texture2D NoiseTex : register(t7);

//// �ν��Ͻ� �ϴ� �ֵ��� ���
struct InstanceObject
{
	float4x4	gWorld;
	uint		MaterialIndex;
};

struct MaterialData
{
	float4 gDiffuse;
	float4 gAmbient;
	float4 gSpecular;
};

StructuredBuffer<InstanceObject> gInstanceData : register(t0, space1);
StructuredBuffer<MaterialData>	 gMaterialData : register(t1, space1);