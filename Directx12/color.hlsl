//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

#include "Common.hlsl"

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
	float3 Normal : NORMAL;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
	float4 Normal : NORMAL;

	nointerpolation uint MaterialIndex : MATINDEX;
};

struct PSOut
{
	float4 Diffuse : SV_TARGET0;
	float4 Ambient : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
	float4 Depth : SV_TARGET4;
};

VertexOut VS(VertexIn vin, uint instanceID : SV_InstanceID)
{
	VertexOut vout;
	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;
	uint boolBone = instObjData.gboolBone;
	vout.MaterialIndex = matIndex;

	//SkinnedData instSkinned = gSkinnedData[instanceID];

	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gView), gProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
	vout.Normal = normalize(mul(float4(vin.Normal, 0.f), world));
    
    return vout;
}

PSOut PS(VertexOut pin)
{
	PSOut vout;
	MaterialData materialData = gMaterialData[pin.MaterialIndex];
	vout.Diffuse = pin.Color * materialData.gDiffuse;
	vout.Ambient = pin.Color * materialData.gAmbient;
	vout.Specular = pin.Color * materialData.gSpecular;
	vout.Normal = float4(pin.Normal.xyz * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}


struct In
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
	float3 Normal : NORMAL;
};

struct Out
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
	float4 Normal : NORMAL;

	nointerpolation uint MaterialIndex : MATINDEX;
};

Out VS_Main(In vin, uint instanceID : SV_InstanceID)
{
	Out vout;

	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;
	vout.MaterialIndex = matIndex;

	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gView), gProj);

	// Just pass vertex color into the pixel shader.
	vout.UV = vin.UV;
	vout.Normal = normalize(mul(float4(vin.Normal, 0.f), world));

	return vout;
}

PSOut PS_Main(Out pin)
{
	PSOut vout;

	float2 terrainsize = float2(50.f, 100.f);
	MaterialData materialData = gMaterialData[pin.MaterialIndex];

	vout.Diffuse = Texture.Sample(gsamLinear, pin.UV * terrainsize) * materialData.gDiffuse;
	vout.Ambient = Texture.Sample(gsamLinear, pin.UV * terrainsize) * materialData.gAmbient;
	vout.Specular = Texture.Sample(gsamLinear, pin.UV * terrainsize) * materialData.gSpecular;
	vout.Normal = float4((pin.Normal * 0.5f + 0.5f).xyz, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

struct Shade_In
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
};

struct Shade_Out
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
};

Shade_Out VS_Shade(Shade_In pin)
{
	Shade_Out vOut;
	//vOut.PosH = mul(mul(float4(pin.PosL, 1.0f), gView), gProj);
	float4 depth = DepthTex.SampleLevel(gsamLinear, pin.UV, 0);
	vOut.PosH = float4(pin.PosL.x, pin.PosL.y, depth.x, 1.f);
	vOut.UV = pin.UV;

	return vOut;
}

struct PS_SHADE_OUT
{
	float4 Shade : SV_TARGET0;
};

PS_SHADE_OUT PS_Shade(Shade_Out pin)
{
	PS_SHADE_OUT pOut;
	//pOut.Shade = float4(1.f, 1.f, 1.f, 1.f);

	float4 Normal = NormalTex.Sample(gsamLinear, pin.UV);
	Normal = Normal * 2.f - 1.f;
	Normal.w = 0.f;

	float4 depth = DepthTex.Sample(gsamLinear, pin.UV);
	float ViewZ = depth.y * 1000.f;

	float4 position;
	position.x = (pin.UV.x * 2.f - 1.f) * ViewZ;
	position.y = (pin.UV.y * -2.f + 1.f) * ViewZ;
	position.z = depth.x * ViewZ;
	position.w = ViewZ;

	position = mul(position, gInvView);
	position = mul(position, gInvProj);

	float3 camdir = normalize(position.xyz - gCamPosition.xyz);

	float diffuseValue = saturate(dot(normalize(gLightDirection.xyz) * -1.f, Normal.xyz));
	diffuseValue = (ceil(diffuseValue * 3) / 3.f);

	//reflect(vector(normalize(g_vLightDir.xyz), 0.f), vNormal);
	float3 reflection = normalize(reflect(normalize(gLightDirection.xyz), Normal.xyz));
	//float3 reflection = normalize(reflect(gLightDirection.xyz, Normal.xyz));
	float3 specularValue = 0;

	if (diffuseValue.x > 0)
	{
		specularValue = saturate(dot(reflection, -camdir));
		specularValue = pow(specularValue, 20.f);
	}

	float4 diffuse = DiffTex.Sample(gsamLinear, pin.UV);
	float4 ambient = AmbiTex.Sample(gsamLinear, pin.UV);
	float4 specular = SpecTex.Sample(gsamLinear, pin.UV);

	float3 color = (diffuse.xyz * diffuseValue) + (ambient.xyz) + (specular.xyz * specularValue);
	color = pow(color, 1.f / 2.2f);

	pOut.Shade = float4(color.xyz, 1.f);
	


	return pOut;
}

Shade_Out VS_UI(Shade_In pin)
{
	Shade_Out vOut;
	//vOut.PosH = mul(mul(float4(pin.PosL, 1.0f), gView), gProj);
	float4 depth = DepthTex.SampleLevel(gsamLinear, pin.UV, 0);
	vOut.PosH = float4(pin.PosL.x, pin.PosL.y, depth.x, 1.f);
	vOut.UV = pin.UV;

	return vOut;
}
PS_SHADE_OUT PS_UI(Shade_Out pin)
{
	PS_SHADE_OUT pOut;
	
	pOut.Shade = Texture.Sample(gsamLinear, pin.UV);

	return pOut;
}

struct VertexIn_Static
{   // 구조물
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
};


struct VertexIn_Movable
{   // 움직이는 객체
	float3 PosL    : POSITION;
	float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentL : TANGENT;
	float3 BinormalL : BINORMAL;
	float3 BoneWeights : WEIGHTS;
	uint4 BoneIndices  : BONEINDICES;

};

struct VertexOut_Default
{   // 기본 
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION2;
	float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentW : TANGENT;
	float3 BinormalW : BINORMAL;

	// 인덱스가 삼각형을 따라 보간되지 않도록 하기위함
	// 왜 필요한지는 잘 모르겟음
	nointerpolation uint MaterialIndex : MATINDEX;
};

VertexOut_Default VS_FireBall(VertexIn_Static vin, uint instanceID : SV_InstanceID)
{
	VertexOut_Default vout = (VertexOut_Default)0.0f;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;

	vout.MaterialIndex = matIndex;

	vin.NormalL.x *= -1.f;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)world));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)world));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)world));

	// 월드 & 카메라 변환
	
	float2 tex = vin.TexC;
	tex.x += gTime * 0.1f;
	tex.y -= gTime * 0.1f;
	tex *= 3.f;

	float Interval = NoiseTex.SampleLevel(gsamLinear, tex, 0).x;
	Interval = Interval * 2.f - 1.f;
	Interval *= 0.2f;

	vin.PosL += vin.NormalL * Interval;

	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gView), gProj);
	vout.PosH = posH;


	vout.TexC = vin.TexC;

	return vout;
}
PSOut PS_FireBall(VertexOut_Default pin)
{
	PSOut vout;
	MaterialData materialData = gMaterialData[pin.MaterialIndex];
	float2 tex = pin.TexC;
	tex.x += gTime * 0.1f;
	tex.y -= gTime * 0.1f;
	tex *= 3.f;

	float4 color = Texture.Sample(gsamLinear, pin.TexC);
	color *= saturate(pow(NoiseTex.Sample(gsamLinear, tex).x * 1.5f, 3.f));

	vout.Diffuse = color * materialData.gDiffuse;
	vout.Ambient = color * materialData.gAmbient;
	vout.Specular = color * materialData.gSpecular;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

VertexOut_Default VS_Static(VertexIn_Static vin, uint instanceID : SV_InstanceID)
{
	VertexOut_Default vout = (VertexOut_Default)0.0f;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;

	vout.MaterialIndex = matIndex;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), world), gView), gProj);
	vout.PosH = posH;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)world));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)world));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)world));

	vout.TexC = vin.TexC;

	return vout;
}

PSOut PS_Static(VertexOut_Default pin)
{
	MaterialData materialData = gMaterialData[pin.MaterialIndex];

	PSOut vout;
	float3 color = Texture.Sample(gsamLinear, pin.TexC).xyz;
	color = pow(color, 2.2f);
	float4 outcolor = float4(color.xyz, 1.f);
	vout.Diffuse = outcolor * materialData.gDiffuse;
	vout.Ambient = outcolor * materialData.gAmbient;
	vout.Specular = outcolor * materialData.gSpecular;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

VertexOut_Default VS_Movable(VertexIn_Movable vin, uint instanceID : SV_InstanceID)
{
	VertexOut_Default vout = (VertexOut_Default)0.0f;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;
	uint matIndex = instObjData.MaterialIndex;

	SkinnedData instSkinned = gSkinnedData[instanceID];
	//float4x4 SkinnedBoneTransforms[33];
	//for (int i = 0; i < 33; ++i)
	//{
	//	SkinnedBoneTransforms[i] = makeFloat4x4ForFloat3x4(instSkinned.gRight[i], instSkinned.gUp[i], instSkinned.gLook[i], instSkinned.gPos[i]);
	//}

	vout.MaterialIndex = matIndex;

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

		//posL += weights[i] * mul(float4(vin.PosL, 1.0f), SkinnedBoneTransforms[vin.BoneIndices[i]]).xyz;
		//normalL += weights[i] * mul(vin.NormalL, (float3x3)SkinnedBoneTransforms[vin.BoneIndices[i]]);
		//tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)instSkinned.gStructedBoneTransforms[vin.BoneIndices[i]]);
		//binormalL += weights[i] * mul(vin.BinormalL, (float3x3)instSkinned.gStructedBoneTransforms[vin.BoneIndices[i]]);

		
		posL += weights[i] * mul(float4(vin.PosL, 1.0f), makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]])).xyz;
		normalL += weights[i] * mul(vin.NormalL, (float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
		tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
		binormalL += weights[i] * mul(vin.BinormalL, (float3x3)makeFloat4x4ForFloat3x4(instSkinned.gRight[vin.BoneIndices[i]], instSkinned.gUp[vin.BoneIndices[i]], instSkinned.gLook[vin.BoneIndices[i]], instSkinned.gPos[vin.BoneIndices[i]]));
	}

	vin.PosL = posL;
	vin.NormalL = normalL;
	vin.TangentL.xyz = tangentL;
	vin.BinormalL = binormalL;

	// 월드 & 카메라 변환
	float4 posW = mul(float4(vin.PosL, 1.0f), world);
	vout.PosW = posW.xyz;
	//float4 posH = mul(mul(posW, gView), gProj);
	//vout.PosH = posH;

	vout.NormalW = normalize(mul(vin.NormalL, (float3x3)world));
	vout.TangentW = normalize(mul(vin.TangentL, (float3x3)world));
	vout.BinormalW = normalize(mul(vin.BinormalL, (float3x3)world));

	vout.PosH = mul(mul(posW, gView), gProj);

	vout.TexC = vin.TexC;

	return vout;
}

PSOut PS_Movable(VertexOut_Default pin)
{
	PSOut vout;
	MaterialData materialData = gMaterialData[pin.MaterialIndex];
	
	float3 color = Texture.Sample(gsamLinear, pin.TexC).xyz;
	color = pow(color, 2.2f);
	float4 outcolor = float4(color.xyz, 1.f);
	vout.Diffuse = outcolor * materialData.gDiffuse;
	vout.Ambient = outcolor * materialData.gAmbient;
	vout.Specular = outcolor * materialData.gSpecular;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);

	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

struct SkyboxIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float3 TexC : TEXCOORD;
};

struct SkyboxOut
{
	float4 PosH : SV_POSITION;
	float3 TexC : TEXCOORD;
};

SkyboxOut VS_Skybox(SkyboxIn vin, uint instanceID : SV_InstanceID)
{
	SkyboxOut vout;

	// 인스턴싱
	InstanceObject instObjData = gInstanceData[instanceID];
	float4x4 world = instObjData.gWorld;


	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.f), world), gView), gProj);
	vout.TexC = vin.TexC;

	return vout;
}

float4 PS_Skybox(SkyboxOut pin) : SV_Target
{
	return CubeMapTex.Sample(gsamLinear, pin.TexC);
}