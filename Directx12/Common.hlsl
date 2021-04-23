
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

cbuffer cbSkill : register(b5)
{
	float4x4 gSkillTime;
}

cbuffer cbObject : register(b6)
{
	float4x4	gWorld;
	uint		MaterialIndex;
};


Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);


Texture2D DiffTex : register(t1);
Texture2D AmbiTex : register(t2);
Texture2D SpecTex : register(t3);
Texture2D NormalTex : register(t4);
Texture2D DepthTex : register(t5);

TextureCube CubeMapTex : register(t6);
Texture2D NoiseTex : register(t7);

Texture2D SkillEffTex1 : register(t8);
Texture2D SkillEffTex2 : register(t9);
Texture2D SkillEffTex3 : register(t10);
Texture2D SkillEffTex4 : register(t11);
Texture2D SkillEffTex5 : register(t12);

//// 인스턴싱 하는 애들이 사용
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