//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************


cbuffer cbPerObjectWorld : register(b0)
{
	float4x4 gWorld; 
};

cbuffer cbCamera : register(b1)
{
	float4x4 gView;
	float4x4 gProj;
	float4x4 gInvView;
	float4x4 gInvProj;
	float4 gCamPosition;
};

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuse;
	float4 gAmbient;
	float4 gSpecular;
};

cbuffer cbLight : register(b3)
{
	float4 gLightDiffuse;
	float4 gLightAmbient;
	float4 gLightSpecular;
	float4 gLightPosition;
	float4 gLightDirection;
};


Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);


Texture2D DiffTex : register(t1);
Texture2D AmbiTex : register(t2);
Texture2D SpecTex : register(t3);
Texture2D NormalTex : register(t4);
Texture2D DepthTex : register(t5);

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

	pOut.Shade = float4((diffuse.xyz * diffuseValue) + (ambient.xyz) + (specular.xyz * specularValue), 1.f);


	return pOut;
}