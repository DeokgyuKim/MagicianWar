#include "Common.hlsl"


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

struct Shade_Out_UI_Room
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Pos : TEXCOORD1;
};

struct Blend_In
{
	float3 PosL  : POSITION;
	float2 UV : TEXCOORD;
};

struct Blend_Out
{
	float4 PosH  : SV_POSITION;
	float2 UV : TEXCOORD;
};

struct PS_BLEND_OUT
{
	float4 Blend : SV_TARGET0;
};

Shade_Out VS_Shade(Shade_In pin)
{
	Shade_Out vOut;
	//vOut.PosH = mul(mul(float4(pin.PosL, 1.0f), gView), gProj);
	float4 depth = DepthTex.SampleLevel(gsamLinear, pin.UV, 0);
	vOut.PosH = float4(pin.PosL.x, pin.PosL.y, 0.f, 1.f);
	vOut.UV = pin.UV;

	return vOut;
}
struct PS_SHADE_OUT
{
	float4 Shade	: SV_TARGET0;
	float4 Specular : SV_TARGET1;
	float4 RimLight : SV_TARGET2;
	float4 OutLine	: SV_TARGET3;
};
PS_SHADE_OUT PS_Shade(Shade_Out pin)
{
	PS_SHADE_OUT pOut;
	//pOut.Shade = float4(1.f, 1.f, 1.f, 1.f);
	
	float4 diffuse = DiffTex.Sample(gsamLinear, pin.UV);
	float4 ambient = AmbiTex.Sample(gsamLinear, pin.UV);
	float4 specular = SpecTex.Sample(gsamLinear, pin.UV);

	float4 Normal = NormalTex.Sample(gsamLinear, pin.UV);
	Normal = Normal * 2.f - 1.f;
	Normal.w = 0.f;

	float diffuseValue = saturate(dot(normalize(gLightDirection.xyz) * -1.f, Normal.xyz));
	diffuseValue = (ceil(diffuseValue * 3) / 3.f);


	float3 color = (diffuse.xyz * diffuseValue) + (ambient.xyz);// +(specular.xyz * specularValue);
	//color = pow(color, 1.f / 2.2f);
	pOut.Shade = float4(color.xyz, 1.f);

	//pOut.Shade = float4(diffuseValue.xxx, 1.f);


	///Specular//////////////////////////////////////////////////////////////////////////////////////
	float4 depth = DepthTex.Sample(gsamLinear, pin.UV);
	float ViewZ = depth.y * 1000.f;

	float4 position;
	position.x = (pin.UV.x * 2.f - 1.f) * ViewZ;
	position.y = (pin.UV.y * -2.f + 1.f) * ViewZ;
	position.z = depth.x * ViewZ;
	position.w = ViewZ;

	float4 InvProjPos = mul(position, gInvProj);
	float4 InvViewPos = mul(InvProjPos, gInvView);
	//position = mul(position, gInvProj); 

	float4 vLook = float4(normalize(gCamPosition.xyz - InvViewPos.xyz), 0.f);
	float4 vReflect = reflect(float4(normalize(-gLightDirection.xyz), 0.f), Normal);

	//pOut.Specular = float4(ceil(InvViewPos.xyz) / 100.f , 1.f);

	pOut.Specular = pow((saturate(dot(vReflect, -vLook))), 20.f);
	//pOut.Specular.a = 0.f;


	///OutLine//////////////////////////////////////////////////////////////////////////////////////
	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			float2 adjacencyUV = pin.UV + float2(j * 0.0005f, i * 0.0009f);
			adjacencyUV = saturate(adjacencyUV);

			float4 adjacencydepth = DepthTex.Sample(gsamLinear, adjacencyUV);
			float adjacencyViewZ = adjacencydepth.y * 1000.f;

			float4 adjacencyposition;


			adjacencyposition.x = (adjacencyUV.x * 2.f - 1.f) * adjacencyViewZ;
			adjacencyposition.y = (adjacencyUV.y * -2.f + 1.f) * adjacencyViewZ;
			adjacencyposition.z = adjacencydepth.x * adjacencyViewZ;
			adjacencyposition.w = adjacencyViewZ;

			adjacencyposition = mul(adjacencyposition, gInvProj);
			adjacencyposition = mul(adjacencyposition, gInvView);

			float4 adjacencyNormal = NormalTex.Sample(gsamLinear, adjacencyUV);
			adjacencyNormal = adjacencyNormal * 2.f - 1.f;
			adjacencyNormal.w = 0.f;

			if(distance(InvViewPos.xyz, adjacencyposition.xyz) > 150.f || dot(adjacencyNormal, Normal) <= 0.3f)
				pOut.OutLine = float4(1.f, 1.f, 1.f, 1.f);

			//if (abs(depth.y - adjacencydepth.y) >= 0.001f || dot(adjacencyNormal, Normal) <= 0.3f)
			//	//if (dot(adjacencyNormal, Normal) <= 0.3f)
			//	pOut.OutLine = float4(1.f, 1.f, 1.f, 1.f);
		}
	}

	/////RimLight//////////////////////////////////////////////////////////////////////////////////////
	float fresnel = 1.f - abs(dot(Normal, -vLook));
	float RimLightValue = max(0.f, -dot(-gLightDirection.xyz, -vLook.xyz));
	pOut.RimLight = float4(1.f, 1.f, 1.f, 1.f) * RimLightValue * fresnel;

	return pOut;
}





Blend_Out VS_BLEND(Blend_In pin)
{
	Blend_Out vOut;
	//vOut.PosH = mul(mul(float4(pin.PosL, 1.0f), gView), gProj);
	float4 depth = DepthTex.SampleLevel(gsamLinear, pin.UV, 0);
	vOut.PosH = float4(pin.PosL.x, pin.PosL.y, 0.f, 1.f);
	vOut.UV = pin.UV;

	return vOut;
}

PS_BLEND_OUT PS_BLEND(Blend_Out pin)
{
	PS_BLEND_OUT pOut;
	//pOut.Shade = float4(1.f, 1.f, 1.f, 1.f);

	float4 shade = DiffTex.Sample(gsamLinear, pin.UV);
	float4 specular = AmbiTex.Sample(gsamLinear, pin.UV);
	float4 rimlight = SpecTex.Sample(gsamLinear, pin.UV);
	float4 outline = NormalTex.Sample(gsamLinear, pin.UV);
	float4 depth = DepthTex.Sample(gsamLinear, pin.UV);
	

	float4 color = shade + specular + rimlight;
	color = color * (1 - outline);
	color = pow(color, 1.f / 2.2f);
	color.a = 1.f;

	//Find Position
	float ViewZ = depth.y * 1000.f;
	float4 position;
	position.x = (pin.UV.x * 2.f - 1.f) * ViewZ;
	position.y = (pin.UV.y * -2.f + 1.f) * ViewZ;
	position.z = depth.x * ViewZ;
	position.w = ViewZ;
	position = mul(position, gInvProj);
	position = mul(position, gInvView);

	float4 shadowpos = mul(position, gLightView);
	float4 uvpos = mul(shadowpos, gLightProj);
	float2 newuv;

	newuv.x = (uvpos.x / uvpos.w) * 0.5f + 0.5f;
	newuv.y = (uvpos.y / uvpos.w) * -0.5f + 0.5f;
	//color = LightDepthTex.Sample(gsamLinear, saturate(newuv));
	if (saturate(newuv.x) == newuv.x && saturate(newuv.y) == newuv.y)
	{
		float4 lightdepth = LightDepthTex.Sample(gsamLinear, newuv);
	
		if (shadowpos.z - 0.1f > lightdepth.r * 1000.f)
		{
			color *= 0.5f;
			color.a = 1.f;
		}
	}



	pOut.Blend = color;
	pOut.Blend.a = 1.f;

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
PS_BLEND_OUT PS_UI(Shade_Out pin)
{
	PS_BLEND_OUT pOut;

	float4 text = DiffTex.Sample(gsamLinear, pin.UV);
	float4 ui = Texture.Sample(gsamLinear, pin.UV);
	float value = text.a;

	float4 color = text * value + ui * (1.f - value);
	float2 alpha;
	alpha.x = floor((1 - pin.UV.x) + ratio.x);
	alpha.y = floor((1 - pin.UV.y) + ratio.y);
	float resultalpha = min(alpha.x, alpha.y);

	pOut.Blend = float4(color.xyz, resultalpha * color.w);

	return pOut;
}
Shade_Out_UI_Room VS_UI_ROOMS(Shade_In pin)
{
	Shade_Out_UI_Room vOut;
	//vOut.PosH = mul(mul(float4(pin.PosL, 1.0f), gView), gProj);
	float4 depth = DepthTex.SampleLevel(gsamLinear, pin.UV, 0);
	vOut.PosH = float4(pin.PosL.x, pin.PosL.y + ratio.y, depth.x, 1.f);
	vOut.UV = pin.UV;
	vOut.Pos = vOut.PosH;

	return vOut;
}
PS_BLEND_OUT PS_UI_ROOMS(Shade_Out_UI_Room pin)
{
	PS_BLEND_OUT pOut;

	float4 text = DiffTex.Sample(gsamLinear, pin.UV);
	float4 ui = Texture.Sample(gsamLinear, pin.UV);
	float value = text.a;

	float4 color = text * value + ui * (1.f - value);
	pOut.Blend = color;

	//((250.f / (1080.f / 2.f)) - 1.f) * -1.f;
	//(800.f / 1080.f) * 2.f

	pOut.Blend.a = 0.f;
	if (pin.Pos.y > ((270.f / (1080.f / 2.f)) - 1.f) * -1.f - (760.f / 1080.f) * 2.f
		&& pin.Pos.y < ((270.f / (1080.f / 2.f)) - 1.f) * -1.f)
		pOut.Blend.a = 1.f;

	return pOut;
}