#include "Common.hlsl"

struct In_Skill
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

struct Out_Skill
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
	float4 Normal : NORMAL;
};

struct PSOut_Skill
{
	float4 Diffuse : SV_TARGET0;
	float4 Ambient : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
	float4 Depth : SV_TARGET4;
};

Out_Skill VS_Flames_FireEff(In_Skill vin)
{
	Out_Skill vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);

	// Just pass vertex color into the pixel shader.
	vout.UV = vin.UV;
	vout.Normal = normalize(mul(float4(vin.Normal, 0.f), gWorldNoInstanced));

	return vout;
}

PSOut_Skill PS_Flames_FireEff(Out_Skill pin)
{
	PSOut_Skill vout;

	MaterialData materialData = gMaterialData[MaterialIndexNoInstanced];

	float2 uv1 = pin.UV;
	float2 uv2 = pin.UV * 2.f;
	float2 uv3 = pin.UV * 3.f;

	float fOffset = gSkillIdx * 0.3f + 0.7f;
	

	uv1.y += gSkillTime * 0.5f * fOffset;
	uv2.y += gSkillTime * 1.2f * fOffset;
	uv3.y += gSkillTime * 1.8f * fOffset;

	float4 noise1 = SkillEffTex5.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex5.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex5.Sample(gsamLinear, uv3);

	noise1 = (noise1 - 0.5f) * 2.f;
	noise2 = (noise2 - 0.5f) * 2.f;
	noise3 = (noise3 - 0.5f) * 2.f;

	float2 distortion1 = float2(0.1f, 0.2f);
	float2 distortion2 = float2(0.1f, 0.3f);
	float2 distortion3 = float2(0.1f, 0.1f);

	noise1.xy = noise1.xy * distortion1.xy;
	noise2.xy = noise2.xy * distortion2.xy;
	noise3.xy = noise3.xy * distortion3.xy;

	float4 noise = noise1 + noise2 + noise3;

	float perturb = ((1.f - pin.UV.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.UV.xy;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord);

	vout.Diffuse = fire;
	//vout.Diffuse = SkillEffTex1.Sample(gsamLinear, pin.UV) * materialData.gDiffuse;
	//vout.Ambient = SkillEffTex1.Sample(gsamLinear, pin.UV) * materialData.gAmbient;
	//vout.Specular = SkillEffTex1.Sample(gsamLinear, pin.UV) * materialData.gSpecular;
	vout.Normal = float4((pin.Normal * 0.5f + 0.5f).xyz, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}