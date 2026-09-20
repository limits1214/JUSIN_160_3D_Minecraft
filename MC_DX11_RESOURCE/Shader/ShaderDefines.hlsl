
struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 pos;
    float range;
    
    float3 att;
    float _pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 pos;
    float range;
    
    float3 direction;
    float spot;
    
    float3 att;
    float _pad;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};





// 2. 오브젝트당 n회 갱신 (슬롯 b0)
cbuffer CB_PER_OBJECT : register(b0)
{
    matrix g_matWorld;
    matrix g_matWVP;
    float4 g_objectColor;
    uint g_objectLight;
    float3 _g_object_pad;
};

// 1. 프레임당 1회 갱신 (슬롯 b1)
cbuffer CB_PER_FRAME : register(b1)
{
    //DirectionalLight gDirLights;
    
    matrix g_matView; // _float4x4와 1:1 대응
    matrix g_matProj;
    matrix g_matViewProj;
    matrix g_matInvView;
    matrix g_matInvViewProj;
    float3 g_vCamPos;
    float g_fDayFactor; //
    matrix g_matSkyRotation;
    matrix g_matStarRotation;
    matrix g_matShadowLightViewProj;
    float3 g_vShadowLightDir;

};


cbuffer CB_PER_ENTITY_BONE : register(b4)
{
    matrix gBoneMatrices[64];
};

cbuffer CB_PER_QUADITEM_ANIM : register(b5)
{
    float2 gUVOffset; // (0,0) 이면 그대로
    float2 gUVScale; // (1,1) 이면 그대로
};

cbuffer CB_PER_DESTROYSTAGE : register(b6)
{
    uint g_destroyStage; // 0~9
    uint g_destroyStageLight;
    //float g_crackTiling; // 블록 크기에 맞게 조절 (보통 1.0)
    float2 _pad;
};

cbuffer CB_PER_UI : register(b7)
{
    float2 g_ui_texCoord;
    float2 g_ui_uvSize; 
    float4 g_ui_color; 
    uint g_ui_texIndex; 
    float2 g_ui_borderUV; 
    float _pad_perui; 
    float2 g_ui_borderPx; 
    float2 g_ui_rectSizePx;
};

cbuffer CB_PER_VOXEL_WATER : register(b8)
{
    uint g_stillFrameIndex;
    uint g_flowFrameIndex;
    float2 _g_voxel_water_pddding;
}

cbuffer CB_BlockOutline : register(b10)
{
    float3 g_vBlockPos;
    float g_fThickness;
    float4 g_vColor;
    float3 g_vExtents;
    uint g_blockOutlineLight;
}


uint GetTexArrayGroup(uint texId)
{
    return texId >> 24;
}

uint GetTexSliceIndex(uint texId)
{
    return texId & 0x00FFFFFF;
}


Texture2D gShadowMap : register(t4);
SamplerComparisonState gShadowSampler : register(s4);