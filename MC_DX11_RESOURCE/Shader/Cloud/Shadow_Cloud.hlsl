#include "../ShaderDefines.hlsl"


Texture2DArray gTexture32_32Array : register(t5);
SamplerState samp : register(s0);

struct VS_IN
{
    float3 vPosition : POSITION; 
    
    float4 matWorld0 : INSTANCE_WORLD0;
    float4 matWorld1 : INSTANCE_WORLD1;
    float4 matWorld2 : INSTANCE_WORLD2;
    float4 matWorld3 : INSTANCE_WORLD3;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 vLocalPos : TEXCOORD0;
};

struct PS_OUT
{
    float4 target0 : SV_Target;
};

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT output;

    float4x4 matInstanceWorld = float4x4(
        vin.matWorld0,
        vin.matWorld1,
        vin.matWorld2,
        vin.matWorld3
    );

    float4 worldPos = mul(float4(vin.vPosition, 1.0f), matInstanceWorld);
    
    output.position = mul(worldPos, g_matViewProj);
    
    output.vLocalPos = vin.vPosition;

    return output;
}
void PSMain(VS_OUT pin)
{
/*
        PS_OUT pout;
    
    // 기본적으로 하얗고 투명한 마크 구름 색상 세팅
    float3 cloudBaseColor = float3(1.0f, 1.0f, 1.0f);
    
    //  [디테일 팁] 큐브 구름에 입체감 주기
    // 아랫면(Y가 -0.5에 가까운 영역)은 햇빛을 못 받으므로 살짝 어둡게 음영(Shadow)을 줍니다.
    if (pin.vLocalPos.y < -0.4f)
    {
        cloudBaseColor *= 0.75f; // 아랫면은 25% 어둡게 그레이 톤으로 변경
    }
    
    //  낮/밤 보정 추가
    // 밤이 되면 구름도 밤하늘에 묻히도록 밤 그라데이션 컬러와 슥 섞어주거나 가중치를 곱해줍니다.
    // 새벽/노을에는 g_fDayFactor에 의해 자연스럽게 붉어지거나 어두워집니다.
    float3 nightCloudColor = float3(0.1f, 0.12f, 0.2f); // 밤 구름 색상
    float3 finalCloudColor = lerp(nightCloudColor, cloudBaseColor, g_fDayFactor);
    
    // 불투명하게 꽉 채우거나 마크처럼 살짝 반투명하게 하고 싶다면 알파를 0.8f 내외로 조절
    pout.target0 = float4(finalCloudColor, 0.8f);
    
    return pout;
    */
}