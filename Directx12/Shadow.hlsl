
#include "Common.hlsl"


struct VS_Shadow
{
	float4 PosH		: SV_POSITION;
	float4 ProjPos	: TEXCOORD;
};
struct PS_Shadow
{
	float4 LightDepth : SV_TARGET0;
};

PS_Shadow PS_LIGHTDEPTH(VS_Shadow pin)
{
	PS_Shadow vout;

	vout.LightDepth.r = pin.ProjPos.z / pin.ProjPos.w;
	vout.LightDepth.g = 0.f;
	vout.LightDepth.b = 0.f;
	vout.LightDepth.a = 1.f;

	return vout;
}

//NOBLEND
struct NoBlendShadow_In
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};
VS_Shadow VS_NOBLEND_SHADOW(NoBlendShadow_In vin, uint instanceID : SV_InstanceID)
{
	VS_Shadow vout;

	InstanceObject instObjData = gInstanceData[instanceID];
	row_major matrix world = instObjData.gWorld;

	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gLightView), gLightProj);
	vout.ProjPos = vout.PosH;

	return vout;
}
//NOBLEND

//STATIC
struct StaticShadow_In
{
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
};
VS_Shadow VS_STATIC_SHADOW(StaticShadow_In vin, uint instanceID : SV_InstanceID)
{
	VS_Shadow vout = (VS_Shadow)0.0f;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	row_major matrix world = instObjData.gWorld;

	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gLightView), gLightProj);
	vout.ProjPos = vout.PosH;

	return vout;
}
//STATIC

//MOVABLE
struct MovableShadow_In
{   // 움직이는 객체
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices  : BONEINDICES;

};
VS_Shadow VS_MOVABLE_SHADOW(MovableShadow_In vin, uint instanceID : SV_InstanceID)
{
	VS_Shadow vout = (VS_Shadow)0.0f;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	row_major matrix world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;

	SkinnedData instSkinned = gSkinnedData[instanceID];

	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.BoneWeights.x;
	weights[1] = vin.BoneWeights.y;
	weights[2] = vin.BoneWeights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	float3 binormalL = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 4; ++i)
	{
		posL += weights[i] * mul(float4(vin.PosL, 1.0f), makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]])).xyz;
		normalL += weights[i] * mul(vin.NormalL, (row_major float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
		tangentL += weights[i] * mul(vin.TangentL.xyz, (row_major float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
		binormalL += weights[i] * mul(vin.BinormalL, (row_major float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
	}

	vin.PosL = posL;
	vin.NormalL = normalL;
	vin.TangentL.xyz = tangentL;
	vin.BinormalL = binormalL;

	// 월드 & 카메라 변환

	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gLightView), gLightProj);
	vout.ProjPos = vout.PosH;


	return vout;
}
//MOVABLE