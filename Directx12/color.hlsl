//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
};


Texture2D Texture : register(t0);
SamplerState gsamLinear  : register(s0);

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;
}


struct In
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
};

struct Out
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
};

Out VS_Main(In vin)
{
	Out vout;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
	vout.UV = vin.UV;

	return vout;
}

float4 PS_Main(Out pin) : SV_Target
{
	return Texture.Sample(gsamLinear, pin.UV);
}