
#include "Common.hlsl"


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

    MaterialData matData = gMaterialData[gMaterialIndex];

    // 월드 & 카메라 변환
    float4 posH = mul(mul(mul(float4(vin.PosL, 1.0f), gWorld), gView), gProj);
    vout.PosH = posH;

    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
    vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);
    vout.BinormalW = mul(vin.BinormalL, (float3x3)gWorld);

    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;
    
    return vout;
}

float4 PS_Static(VertexOut_Default pin) : SV_Target
{
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float  roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;

    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    return color;
}

VertexOut_Default VS_Movable(VertexIn_Movable vin)
{
    VertexOut_Default vout = (VertexOut_Default)0.0f;

    MaterialData matData = gMaterialData[gMaterialIndex];

    //float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    //weights[0] = vin.BoneWeights.x;
    //weights[1] = vin.BoneWeights.y;
    //weights[2] = vin.BoneWeights.z;
    //weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

    //float3 posL = float3(0.0f, 0.0f, 0.0f);
    //float3 normalL = float3(0.0f, 0.0f, 0.0f);
    //float3 tangentL = float3(0.0f, 0.0f, 0.0f);
    //float3 binormalL = float3(0.0f, 0.0f, 0.0f);

    //for (int i = 0; i < 4; ++i)
    //{
    //    // 변환들에 비균등 비례가 전혀 없다고 가정한다(따라서
    //    // 법선 변환 시 역전치 행렬을 사용할 필요가 없다.)

    //    posL += weights[i] * mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
    //    normalL += weights[i] * mul(vin.NormalL, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
    //    tangentL += weights[i] * mul(vin.TangentL.xyz, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
    //    binormalL += weights[i] * mul(vin.BinormalL, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);

    //}

    //vin.PosL = posL;
    //vin.NormalL = normalL;
    //vin.TangentL.xyz = tangentL;
    //vin.BinormalL = binormalL;

    // 월드 & 카메라 변환
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
    //float4 posH = mul(mul(posW, gView), gProj);
    //vout.PosH = posH;

    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
    vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);
    vout.BinormalW = mul(vin.BinormalL, (float3x3)gWorld);

    vout.PosH = mul(mul(posW, gView), gProj);

    float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
    vout.TexC = mul(texC, matData.MatTransform).xy;

    return vout;
}

float4 PS_Movable(VertexOut_Default pin) : SV_Target
{
    MaterialData matData = gMaterialData[gMaterialIndex];
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float  roughness = matData.Roughness;
    uint diffuseMapIndex = matData.DiffuseMapIndex;
    uint normalMapIndex = matData.NormalMapIndex;

    //float4 color = float4(1.0f, 0.0f, 1.0f, 1.0f);
    return diffuseAlbedo;
}