
cbuffer cbCamera : register(b1)
{
	row_major matrix gView;
	row_major matrix gProj;
	row_major matrix gInvView;
	row_major matrix gInvProj;
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
	row_major matrix gLightView;
	row_major matrix gLightProj;
};

//cbuffer cbSkinned : register(b4)
//{
//	float4x4 gBoneTransforms[96];
//}

cbuffer cbSkill : register(b5)
{
	float	gSkillTime;
	uint	gSkillIdx;
	float	gDissolveC;
}

cbuffer cbObject : register(b6)
{
	row_major matrix	gWorldNoInstanced;
	uint		MaterialIndexNoInstanced;
};

cbuffer cbUi : register(b7)
{
	float2	ratio;
}
cbuffer cbText : register(b8)
{
	float2	Textidx;
	float3	TextColor;
}


Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);
SamplerState gsamClamp  : register(s1);


Texture2D DiffTex : register(t1);
Texture2D AmbiTex : register(t2);
Texture2D SpecTex : register(t3);
Texture2D NormalTex : register(t4);
Texture2D DepthTex : register(t5);
Texture2D LightDepthTex : register(t13);
Texture2D ShadeTex : register(t14);

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
	row_major matrix	gWorld;
	uint		MaterialIndex;
	uint		gboolBone;
	uint		Attribute;
};

struct MaterialData
{
	float4 gDiffuse;
	float4 gAmbient;
	float4 gSpecular;
};

struct SkinnedData
{
	float3 gRight[33];
	float3 gUp[33];
	float3 gLook[33];
	float3 gPos[33];
};

row_major matrix makeFloat4x4ForFloat3x4(float3 r, float3 u, float3 l, float3 p)
{
	row_major matrix result;
	//result._11 = r.x; result._21 = r.y; result._31 = r.z; result._41 = 0;
	//result._12 = u.x; result._22 = u.y; result._32 = u.z; result._42 = 0;
	//result._13 = l.x; result._23 = l.y; result._33 = l.z; result._43 = 0;
	//result._14 = p.x; result._24 = p.y; result._34 = p.z; result._44 = 1;


	result._11 = r.x; result._12 = r.y; result._13 = r.z; result._14 = 0;
	result._21 = u.x; result._22 = u.y; result._23 = u.z; result._24 = 0;
	result._31 = l.x; result._32 = l.y; result._33 = l.z; result._34 = 0;
	result._41 = p.x; result._42 = p.y; result._43 = p.z; result._44 = 1;

	return result;
}

StructuredBuffer<InstanceObject> gInstanceData : register(t0, space1);
StructuredBuffer<MaterialData>	 gMaterialData : register(t1, space1);
StructuredBuffer<SkinnedData>	 gSkinnedData  : register(t2, space2);