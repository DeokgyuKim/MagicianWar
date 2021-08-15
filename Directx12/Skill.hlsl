#include "Common.hlsl"

struct In_Skill
{
	float3 PosL  : POSITION;
	float2 TexC : TEXCOORD;
	float3 Normal : NORMAL;
};

struct Out_Skill
{
	float4 PosH  : SV_POSITION;
	float3 WorldPos : POSITION;
	float2 TexC : TEXCOORD;
	float4 Normal : NORMAL;
};

struct PSOut_Skill
{
	float4 Diffuse		: SV_TARGET0;
	float4 Distortion	: SV_TARGET1;
};

struct PSOut_Skill_DEFF
{
	float4 Diffuse		: SV_TARGET0;
	float4 Ambient		: SV_TARGET1;
	float4 Specular		: SV_TARGET2;
	float4 Normal		: SV_TARGET3;
	float4 Depth		: SV_TARGET4;
	float4 Positiion	: SV_TARGET5;
	float4 Emmissive	: SV_TARGET6;
};

Out_Skill VS_Flames_FireEff(In_Skill vin)
{
	Out_Skill vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.WorldPos = mul(float4(vin.PosL, 1.0f), gWorldNoInstanced);
	// Just pass vertex color into the pixel shader.
	vout.TexC = vin.TexC;
	vout.Normal = normalize(mul(float4(vin.Normal, 0.f), gWorldNoInstanced));


	return vout;
}
PSOut_Skill PS_Flames_FireEff(Out_Skill pin)
{
	PSOut_Skill vout;

	MaterialData materialData = gMaterialData[MaterialIndexNoInstanced];

	pin.TexC.x *= 2.f;

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord) + SkillEffTex1.Sample(gsamClamp, noiseCoord + float2(0.f, 0.2f));

	vout.Diffuse = fire;
	vout.Diffuse.a = saturate(vout.Diffuse.r + 0.1f);
	vout.Distortion = float4(1 - fire.r, 1 - fire.r, 0.f, 0.f);

	return vout;
}
PSOut_Skill PS_Magic_Circle(Out_Skill pin)
{
	PSOut_Skill vout;

	float4 color = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	float alpha = SkillEffTex2.Sample(gsamLinear, pin.TexC).r + gSkillTime;
	alpha = floor(alpha);
	color.a = min(alpha, color.r);
	//color.a = color.r;
	if (gSkillIdx == 1)
		color.rgb *= float3(1.f, 0.5f, 0.2f);
	else if (gSkillIdx == 2)
		color.rgb *= float3(0.2f, 0.5f, 1.f);
	else if (gSkillIdx == 3)
		color.rgb *= float3(0.5f, 0.2f, 1.f);

	vout.Diffuse = color;
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);

	return vout;
}


struct In_Skill_Static
{   // 구조물
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
};

struct Out_Skill_Static
{   // 기본 
	float4 PosH    : SV_POSITION;
	float3 ViewPos    : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;
};

Out_Skill_Static VS_FireShock_FireCylinder(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	//vout.TexC = vin.TexC;

	vout.TexC = vin.TexC + float2(gSkillTime * pow(4, gSkillTime * 0.1f), gSkillTime * 1.f);
	return vout;
}
PSOut_Skill PS_FireShock_FireCylinder(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	float3 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC).rgb;
	float3 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC).rgb;


	float multi2_time = gDissolveC % 1.f;
	clamp(multi2_time, 0.5f, 1.f);

	float multi1 = ((diff2.r * sin(gDissolveC)) * 2.8);
	float multi2 = diff2.r * sin(gDissolveC);

	float b = saturate(pow(multi1 + multi2, 20));

	float c = pow(multi1 + multi2, 20);

	float3 Ke;

	if (0.9f >= b)
		Ke = float3(1.f, 100.f, 1.f);
	else
		Ke = float3(0.f, 0.f, 0.f);



	float3 diffuse = (Ke * float3(0.f, 1.f, 0.f) + diff1);
	diffuse.r = 0.5 + diffuse.r * sin(gDissolveC);
	diffuse.g = 0.5 + diffuse.g * sin(gDissolveC);


	vout.Diffuse = float4(diffuse, c);
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);

	return vout;
}

Out_Skill_Static VS_FireShock_FireParticle(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;


	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	// 월드 & 카메라 변환

	//float cosTime = cos(gSkillTime * 10.f + vin.TexC.x * 1.f) * 0.1f;

	//vin.PosL.x += cosTime;



	vout.TexC = vin.TexC/* + float2(gSkillTime * 5.f, 0)*/;
	//vout.TexC = vin.TexC;


	return vout;
}
PSOut_Skill PS_FireShock_FireParticle(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	float3 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC).rgb;
	float3 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC).rgb;

	float multi2_time = gSkillTime % 1.f;
	clamp(multi2_time, 0.5f, 1.f);
	float multi1 = ((diff2.r * sin(1.5f)) * 2.8);


	float multi2 = diff2.r * sin(0.50001f);


	float b = saturate(pow(multi1 + multi2, 20));

	float c = pow(multi1 + multi2, 20);

	float3 Ke;

	if (0.9f >= b)
		Ke = float3(100.f, 1.f, 1.f);
	else
		Ke = float3(0.f, 0.f, 0.f);



	float3 diffuse = (Ke * float3(0.f, 1.f, 0.f) + diff1);
	//diffuse.r = diffuse.r * sin(gSkillTime);
	vout.Diffuse = float4(diffuse, c);
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);

	return vout;
}

Out_Skill_Static VS_FireRing_FireCylinder(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;


	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	// 월드 & 카메라 변환

	//float cosTime = cos(gSkillTime * 10.f + vin.TexC.x * 1.f) * 0.1f;

	//vin.PosL.x += cosTime;



	vout.TexC = vin.TexC + float2(gSkillTime * 1.f, 0);
	//vout.TexC = vin.TexC;


	return vout;
}
PSOut_Skill PS_FireRing_FireCylinder(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	float3 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC).rgb;
	float3 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC).rgb;

	float multi2_time = gSkillTime % 1.f;
	clamp(multi2_time, 0.5f, 1.f);

	float multi1 = ((diff2.r * 0.5) * 2.8);
	float multi2 = diff2.r * 0.5;



	//float b = saturate(multi1);
	//float c = pow(multi1, 20);
	float b = saturate(pow(multi1 + multi2, 20));
	float c = pow(multi1 * 1.f + multi2 * gDissolveC, 20);



	float3 Ke;

	if (0.9f >= b)
		Ke = float3(100.f, 1.f, 1.f);
	else
		Ke = float3(0.f, 0.f, 0.f);



	float3 diffuse = (Ke * float3(0.f, 1.f, 0.f) + diff1);
	//diffuse.r = diffuse.r * sin(gSkillTime);
	vout.Diffuse = float4(diffuse, c);
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);



	return vout;
}

Out_Skill_Static VS_DEFFSKILL_MESH(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;
	vout.ViewPos = mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView).xyz;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	vout.TexC = vin.TexC;
	return vout;
}

Out_Skill_Static VS_ICESphere_MESH(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;
	vout.ViewPos = mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView).xyz;

	vin.NormalL.x *= -1.f;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	vout.TexC = vin.TexC;
	return vout;
}

PSOut_Skill_DEFF PS_IceBolt_Ice(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float4 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	float4 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC);

	float4 position = mul(float4(pin.ViewPos, 1.f), gInvView);
	float4 vLook = float4(normalize(gCamPosition.xyz - position.xyz), 0.f);
	float fresnel = 1.f - abs(dot(pin.NormalW, -vLook.xyz));


	vout.Diffuse = diff1 * fresnel + diff2 * (1.f - fresnel);
	vout.Ambient = diff1 * fresnel + diff2 * (1.f - fresnel);
	vout.Specular = diff1 * fresnel + diff2 * (1.f - fresnel);
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	vout.Emmissive = pow(vout.Diffuse, 3.f) * 5.f;
	vout.Emmissive.a = 1.f;

	return vout;
}

PSOut_Skill_DEFF PS_IceBolt_Body(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float4 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	float4 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC)*1.15f;
	diff2.a = 1.f;

	float4 position = mul(float4(pin.ViewPos, 1.f), gInvView);
	float4 vLook = float4(normalize(gCamPosition.xyz - position.xyz), 0.f);
	float fresnel = 1.f - abs(dot(pin.NormalW, -vLook.xyz));

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	uv1.x += gSkillTime * 0.5f * 0.1f;
	uv2.x += gSkillTime * 2.2f * 0.1f;
	uv3.x += gSkillTime * 3.8f * 0.1f;

	float4 noise1 = SkillEffTex3.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex3.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex3.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 Ice = float4(SkillEffTex1.Sample(gsamClamp, noiseCoord).rrra);// *SkillEffTex2.Sample(gsamClamp, noiseCoord);


	vout.Diffuse = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Ambient = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Specular = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	vout.Emmissive = pow(vout.Diffuse, 3.f) * 5.f;
	vout.Emmissive.a = 1.f;

	return vout;
}

PSOut_Skill_DEFF PS_IceBolt_Tornado(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float4 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	float4 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC) * 1.15f;
	diff2.a = 1.f;

	float4 position = mul(float4(pin.ViewPos, 1.f), gInvView);
	float4 vLook = float4(normalize(gCamPosition.xyz - position.xyz), 0.f);
	float fresnel = 1.f - abs(dot(pin.NormalW, -vLook.xyz));

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	uv1.y -= gSkillTime * 0.5f * 0.1f;
	uv2.y -= gSkillTime * 2.2f * 0.1f;
	uv3.y -= gSkillTime * 3.8f * 0.1f;

	uv1.x -= gSkillTime * 0.5f * 0.1f;
	uv2.x -= gSkillTime * 2.2f * 0.1f;
	//uv3.x -= gSkillTime * 3.8f * 0.1f;

	float4 noise1 = SkillEffTex3.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex3.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex3.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 Ice = float4(SkillEffTex1.Sample(gsamClamp, noiseCoord).rrra);// *SkillEffTex2.Sample(gsamClamp, noiseCoord);


	vout.Diffuse = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Ambient = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Specular = Ice * fresnel + diff2 * (1 - fresnel);
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	vout.Emmissive = pow(vout.Diffuse, 3.f) * 5.f;
	vout.Emmissive.a = 1.f;

	return vout;
}

PSOut_Skill_DEFF PS_Meteor_Body(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	uv1.y += gSkillTime * 0.5f * 0.1f;
	uv2.y += gSkillTime * 1.2f * 0.1f;
	uv3.y += gSkillTime * 1.8f * 0.1f;

	float4 noise1 = SkillEffTex2.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex2.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex2.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord);// *SkillEffTex2.Sample(gsamClamp, noiseCoord);

	vout.Diffuse = fire;
	vout.Ambient = fire;
	vout.Specular = fire;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	vout.Emmissive = fire * SkillEffTex2.Sample(gsamClamp, noiseCoord) * 20.f;
	vout.Emmissive.a = 1.f;
	return vout;
}

PSOut_Skill PS_Meteor_FireEff(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	float2 newUV = pin.TexC;
	newUV.y += gSkillTime * 0.01f;

	pin.TexC.y = pow(pin.TexC.y, 1.f / 5.f);

	float4 fire = SkillEffTex1.Sample(gsamClamp, pin.TexC);// *SkillEffTex2.Sample(gsamClamp, noiseCoord);

	vout.Diffuse = float4(fire.rgb, fire.r * (pin.TexC.y));
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);

	return vout;
}
PSOut_Skill PS_Meteor_FireEff2(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	pin.TexC.y = pow(pin.TexC.y, 1.f / 5.f);
	pin.TexC.x /= 5.f;

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	float fOffset = 0.7f;


	uv1.y += gSkillTime * 0.5f * fOffset;
	uv2.y += gSkillTime * 1.2f * fOffset;
	uv3.y += gSkillTime * 1.8f * fOffset;

	float4 noise1 = SkillEffTex2.Sample(gsamLinear, uv1 * 5.f);
	float4 noise2 = SkillEffTex2.Sample(gsamLinear, uv2 * 5.f);
	float4 noise3 = SkillEffTex2.Sample(gsamLinear, uv3 * 5.f);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord);

	vout.Diffuse = float4(fire.rgb, pow(fire.r, 1.3f));
	vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);
	return vout;
}

Out_Skill_Static VS_BlackHole(In_Skill_Static vin)
{
	Out_Skill_Static vout = (Out_Skill_Static)0;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView), gProj);
	vout.PosH = posH;
	vout.ViewPos = mul(mul(float4(vin.PosL, 1.0f), gWorldNoInstanced), gView).xyz;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)gWorldNoInstanced));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)gWorldNoInstanced));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)gWorldNoInstanced));

	vout.TexC = vin.TexC;
	return vout;
}
PSOut_Skill PS_BlackHole(Out_Skill_Static pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	float fOffset = gSkillIdx * 0.3f + 0.7f;


	uv1.y += gSkillTime * 0.5f * fOffset;
	uv2.y += gSkillTime * 1.2f * fOffset;
	uv3.y += gSkillTime * 1.8f * fOffset;
	uv1.x *= 5.f;
	uv2.x *= 5.f;
	uv3.x *= 5.f;

	float4 noise1 = SkillEffTex2.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex2.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex2.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	noiseCoord.x *= 3.f;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord);
	float4 black = float4(fire.r * 0.5f, 0.f, fire.r * 0.5f, fire.r);
	//float4 black = float4(0.f, 0.f, 0.f, 1.f);

	vout.Diffuse = black;
	vout.Distortion = float4(fire.r * 0.05f, fire.r * 0.05f, 0.f, 1.f);

	return vout;
}

PSOut_Skill_DEFF PS_BlackHole_Inner(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float4 diff1 = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	float4 diff2 = SkillEffTex2.Sample(gsamLinear, pin.TexC) * 1.15f;
	diff2.a = 1.f;
	diff2 = float4(0.f, 0.f, 0.f, 1.f);

	float4 position = mul(float4(pin.ViewPos, 1.f), gInvView);
	float4 vLook = float4(normalize(gCamPosition.xyz - position.xyz), 0.f);
	float fresnel = 1.f - abs(dot(pin.NormalW, -vLook.xyz));

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	uv1.y -= gSkillTime * 0.5f * 0.1f;
	uv2.y -= gSkillTime * 2.2f * 0.1f;
	uv3.y -= gSkillTime * 3.8f * 0.1f;

	uv1.x -= gSkillTime * 0.5f * 0.1f;
	uv2.x -= gSkillTime * 2.2f * 0.1f;
	//uv3.x -= gSkillTime * 3.8f * 0.1f;

	float4 noise1 = SkillEffTex3.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex3.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex3.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 Ice = float4(0.f, SkillEffTex1.Sample(gsamClamp, noiseCoord).r, 0.f, 1.f);// *SkillEffTex2.Sample(gsamClamp, noiseCoord);

	float value = SkillEffTex1.Sample(gsamClamp, noiseCoord).r;
	float4 color = float4(0.f, 0.f, 0.f, 1.f) * value + float4(1.f, 0.f, 1.f, 1.f) * (1.f - value);


	vout.Diffuse = color;
	vout.Ambient = color;
	vout.Specular = color;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	//vout.Emmissive = float4(0.f, 0.f, 0.f, 0.f);
	vout.Emmissive = pow(vout.Diffuse, 3.f) * 10.f;
	vout.Emmissive.a = 1.f;

	return vout;
}

PSOut_Skill_DEFF PS_HatredChain_Body(Out_Skill_Static pin)
{
	PSOut_Skill_DEFF vout = (PSOut_Skill_DEFF)0;

	float4 color = SkillEffTex1.Sample(gsamLinear, pin.TexC);

	float4 position = mul(float4(pin.ViewPos, 1.f), gInvView);
	float4 vLook = float4(normalize(gCamPosition.xyz - position.xyz), 0.f);
	float fresnel = 1.f - abs(dot(pin.NormalW, -vLook.xyz));

	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	float fOffset = 0.7f;

	uv1.y += gSkillTime * 0.5f * fOffset;
	uv2.y += gSkillTime * 1.2f * fOffset;
	uv3.y += gSkillTime * 1.8f * fOffset;
	uv1.x *= 5.f;
	uv2.x *= 5.f;
	uv3.x *= 5.f;

	float4 noise1 = SkillEffTex2.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex2.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex2.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	noiseCoord.x *= 3.f;
	float4 fire = SkillEffTex3.Sample(gsamClamp, noiseCoord);
	float4 black = float4(fire.r, 0.f, fire.r, 1.f);

	vout.Diffuse = float4(0.f, 0.f, 0.f, 1.f);
	vout.Ambient = float4(0.f, 0.f, 0.f, 1.f);
	vout.Specular = float4(0.f, 0.f, 0.f, 1.f);
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);
	vout.Positiion = float4(pin.ViewPos, 1.f);
	vout.Emmissive = black * (fresnel * 2.f) * 1.f;
	vout.Emmissive.a = 1.f;
	//vout.Emmissive = pow(vout.Diffuse, 3.f) * 10.f;
	//vout.Emmissive.a = 1.f;

	return vout;
}

PSOut_Skill PS_HatredChain_Outter(Out_Skill pin)
{
	PSOut_Skill vout = (PSOut_Skill)0;
	vout.Diffuse = float4(0.f, 0.f, 0.f, 0.f);
	//if (distance(pin.TexC, float2(0.5f, 0.5f)) > 0.4f && distance(pin.TexC, float2(0.5f, 0.5f)) < 0.45f)
	//{
	//	vout.Diffuse = float4(0.3f, 0.f, 0.3f, 1.f);
	//}
	//vout.Distortion = SkillEffTex1.Sample(gsamLinear, pin.TexC);
	if (distance(pin.TexC, float2(0.5f, 0.5f)) > 0.45f)
	{
		vout.Distortion = float4(0.f, 0.f, 0.f, 0.f);
		return vout;
	}


	float2 Dist = pin.TexC - float2(0.5f, 0.5f);

	//Dist.y *= -1.f;
	
	Dist *= 0.1f;
	
	float2 uv1 = pin.TexC;
	float2 uv2 = pin.TexC * 2.f;
	float2 uv3 = pin.TexC * 3.f;

	float fOffset = 0.7f;

	uv1.y += gSkillTime * 0.5f * fOffset;
	uv2.y += gSkillTime * 1.2f * fOffset;
	uv3.y += gSkillTime * 1.8f * fOffset;
	uv1.x *= 5.f;
	uv2.x *= 5.f;
	uv3.x *= 5.f;

	float4 noise1 = SkillEffTex2.Sample(gsamLinear, uv1);
	float4 noise2 = SkillEffTex2.Sample(gsamLinear, uv2);
	float4 noise3 = SkillEffTex2.Sample(gsamLinear, uv3);

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

	float perturb = ((1.f - pin.TexC.y) * 0.8f) + 0.5f;

	float2 noiseCoord = (noise.xy * perturb) + pin.TexC.xy;
	float4 fire = SkillEffTex1.Sample(gsamClamp, noiseCoord);

	Dist *= fire;

	Dist = Dist * 0.5f + 0.5f;

	vout.Distortion = float4(Dist, 0.f, Dist.r);
	return vout;
}
