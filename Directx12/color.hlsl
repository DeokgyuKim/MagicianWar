//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************


cbuffer cbPerObjectWorld : register(b0)
{
	float4x4 gWorld; 
};


cbuffer cbPerObjectView : register(b1)
{
	float4x4 gView;
};

cbuffer cbPerObjectProj : register(b2)
{
	float4x4 gProj;
};

Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);



static const float3 DirectionalLight = float3(1.f, -1.f, 1.f);

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

float4 PS(VertexOut pin) : SV_Target
{
	float4 LightDir = float4(normalize(DirectionalLight), 0.f);
	float value = dot(pin.Normal, -LightDir);
	return pin.Color * value;
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

float4 PS_Main(Out pin) : SV_Target
{
	//return float4(1.f, 1.f, 1.f, 1.f);
	float4 LightDir = float4(normalize(DirectionalLight), 0.f);
	float value = dot(pin.Normal, -LightDir);
	return Texture.Sample(gsamLinear, pin.UV) * value;
}