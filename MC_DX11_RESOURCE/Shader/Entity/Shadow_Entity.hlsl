#include "../ShaderDefines.hlsl"

struct VS_IN
{
    float3 posL : POSITION;
    float3 normal : NORMAL; // 입력 레이아웃 맞추기 위해 유지
    float2 texCoord : TEXCOORD0;
    uint boneIndex : BLENDINDICES0;
    uint texIndex : BLENDINDICES1;
};

struct VS_OUT
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES0;
    // NormalW 등 불필요한 데이터 제거
};

struct PS_IN
{
    float4 posH : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    uint texIndex : BLENDINDICES0;
};

Texture2DArray gEntityTexture64_32Array : register(t7);
Texture2DArray gEntityTexture64_64Array : register(t8);
Texture2DArray gTexture256_256Array : register(t12);
Texture2DArray gTexture32_32Array : register(t5);
SamplerState gSamPointWrap : register(s0);

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT vout;
    // 1. bone 행렬로 로컬 → 월드 변환
    float4 posW = mul(float4(vin.posL, 1.f), gBoneMatrices[vin.boneIndex]);

    // 2. 조명 시점의 WVP 곱함 (C++에서 g_matWVP에 빛의 ViewProj 행렬을 넣어야 함)
    vout.posH = mul(posW, g_matWVP);
    vout.texCoord = vin.texCoord;
    vout.texIndex = vin.texIndex;
    
    return vout;
}

// Pixel Shader: 리턴 타입 void로 변경
void PSMain(PS_IN pin)
{
    uint groupId = GetTexArrayGroup(pin.texIndex);
    uint sliceIndex = GetTexSliceIndex(pin.texIndex);
    float4 albedo = float4(0.f, 0.f, 0.f, 1.f);
    
    if (groupId == 7)
    {
        albedo = gEntityTexture64_32Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 8)
    {
        albedo = gEntityTexture64_64Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 12)
    {
        albedo = gTexture256_256Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    else if (groupId == 5)
    {
        albedo = gTexture32_32Array.Sample(gSamPointWrap, float3(pin.texCoord, sliceIndex));
    }
    
    // 이 부분이 핵심! 
    // 투명도(Alpha)가 0.5 미만인 곳은 픽셀 처리를 버려서 그림자도 뚫리게 만듭니다.
    clip(albedo.a - 0.5f);
    
    // 조명 계산(blockLight, skyLight 등)과 SV_Target 리턴은 전부 삭제합니다.
    // void이므로 여기서 끝! DSV(Depth-Stencil View)에 깊이값이 자동으로 기록됩니다.
}