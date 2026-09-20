struct VS_IN
{
    float3 pos : POSITION; // (-1~1)
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};


Texture2D gTex : register(t0);
SamplerState samLinear : register(s0);

VS_OUT VSMain(VS_IN vin)
{
    VS_OUT o;
    o.pos = float4(vin.pos.xy, 1.0f, 1.0f);
    o.uv = vin.uv;
    return o;
}

float4 PSMain(VS_OUT pin) : SV_Target
{
    float4 color = gTex.Sample(samLinear, pin.uv);

    float intensity = 0.4f; // 빨간 효과 강도

    float3 red = float3(0.7f, 0.0f, 0.0f);

    color.rgb = lerp(color.rgb, red, intensity);


    return color;
    //return gTex.Sample(samLinear, pin.uv);
}