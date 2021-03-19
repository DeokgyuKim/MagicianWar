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
};

struct PSOut
{
	float4 Diffuse : SV_TARGET0;
	float4 Ambient : SV_TARGET1;
	float4 Specular : SV_TARGET2;
	float4 Normal : SV_TARGET3;
	float4 Depth : SV_TARGET4;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorld), gView), gProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
	vout.Normal = mul(float4(vin.Normal, 0.f), gWorld);
    
    return vout;
}

PSOut PS(VertexOut pin)
{
	PSOut vout;

	vout.Diffuse = pin.Color * gDiffuse;
	vout.Ambient = pin.Color * gAmbient;
	vout.Specular = pin.Color * gSpecular;
	vout.Normal = pin.Normal * 0.5f + 0.5f;
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
};

Out VS_Main(In vin)
{
	Out vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorld), gView), gProj);

	// Just pass vertex color into the pixel shader.
	vout.UV = vin.UV;
	vout.Normal = mul(float4(vin.Normal, 0.f), gWorld);

	return vout;
}

PSOut PS_Main(Out pin)
{
	PSOut vout;

	vout.Diffuse = Texture.Sample(gsamLinear, pin.UV) * gDiffuse;
	vout.Ambient = Texture.Sample(gsamLinear, pin.UV) * gAmbient;
	vout.Specular = Texture.Sample(gsamLinear, pin.UV) * gSpecular;
	vout.Normal = pin.Normal * 0.5f + 0.5f;
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
	position.y = (pin.UV.y * 2.f - 1.f) * ViewZ;
	position.z = depth.x * ViewZ;
	position.w = ViewZ;

	position = mul(position, gInvProj);
	position = mul(position, gInvView);

	float3 camdir = normalize(position.xyz - gCamPosition.xyz);

	float diffuseValue = saturate(dot(-normalize(gLightDirection.xyz), Normal.xyz));
	float3 reflection = normalize(reflect(gLightDirection.xyz, Normal.xyz));
	float3 specularValue = 0;

	if (diffuseValue.x > 0)
	{
		specularValue = saturate(dot(reflection.xyz, -camdir));
		specularValue = pow(specularValue, 20.f);
	}

	float4 diffuse = DiffTex.Sample(gsamLinear, pin.UV);
	float4 ambient = AmbiTex.Sample(gsamLinear, pin.UV);
	float4 specular = SpecTex.Sample(gsamLinear, pin.UV);

	pOut.Shade = float4((diffuse.xyz * diffuseValue) + (ambient.xyz), 1.f);
	// + (specular.xyz * specularValue)


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
};

VertexOut_Default VS_Static(VertexIn_Static vin)
{
	VertexOut_Default vout = (VertexOut_Default)0.0f;

	// 월드 & 카메라 변환
	float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorld), gView), gProj);
	vout.PosH = posH;

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldNoScaling);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorldNoScaling);
	vout.BinormalW = mul(vin.BinormalL, (float3x3)gWorldNoScaling);

	vout.TexC = vin.TexC;

	return vout;
}

PSOut PS_Static(VertexOut_Default pin)
{
	PSOut vout;
	vout.Diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f) * gDiffuse;
	vout.Ambient = float4(0.5f, 0.5f, 0.5f, 1.0f) * gAmbient;
	vout.Specular = float4(0.5f, 0.5f, 0.5f, 1.0f) * gSpecular;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);
	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

VertexOut_Default VS_Movable(VertexIn_Movable vin)
{
	VertexOut_Default vout = (VertexOut_Default)0.0f;


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
	//	// Assume no nonuniform scaling when transforming normals, so 
	//	// that we do not have to use the inverse-transpose.

		posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
		normalL += weights[i] * mul(vin.NormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		binormalL += weights[i] * mul(vin.BinormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
	}

	vin.PosL = posL;
	vin.NormalL = normalL;
	vin.TangentL.xyz = tangentL;
	vin.BinormalL = binormalL;

	// 월드 & 카메라 변환
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;
	//float4 posH = mul(mul(posW, gView), gProj);
	//vout.PosH = posH;

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldNoScaling);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorldNoScaling);
	vout.BinormalW = mul(vin.BinormalL, (float3x3)gWorldNoScaling);

	vout.PosH = mul(mul(posW, gView), gProj);

	vout.TexC = vin.TexC;

	return vout;
}

PSOut PS_Movable(VertexOut_Default pin)
{
	PSOut vout;
	vout.Diffuse = float4(0.5f, 0.5f, 0.5f, 1.0f) * gDiffuse;
	vout.Ambient = float4(0.5f, 0.5f, 0.5f, 1.0f) * gAmbient;
	vout.Specular = float4(0.5f, 0.5f, 0.5f, 1.0f) * gSpecular;
	vout.Normal = float4(pin.NormalW * 0.5f + 0.5f, 1.f);

	vout.Depth = float4((pin.PosH.z / pin.PosH.w), pin.PosH.w * 0.001f, 0.f, 1.f);

	return vout;
}

