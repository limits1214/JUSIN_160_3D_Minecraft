#include "pch.h"

#include "VoxelManager2.h"

#include "Resources.h"
NS_USING(Engine)

CVoxelManager2::CVoxelManager2(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: m_pDevice{pDevice}
	, m_pContext{pContext}
{
}

CVoxelManager2::~CVoxelManager2()
{
}

HRESULT CVoxelManager2::Render(ID3D11DeviceContext* pContext, const RENDER_CTX& ctx)
{
	return S_OK;
}

void CVoxelManager2::Update(_float fTimeDelta)
{
}

void CVoxelManager2::UpdateGUI()
{
}

HRESULT CVoxelManager2::Initialize()
{
    m_NoiseHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_NoiseHeight.SetFrequency(0.03f); // 전체적인 지형의 크기 (낮을수록 거대함)

    // Fractal 설정 (핵심!)
    m_NoiseHeight.SetFractalType(FastNoiseLite::FractalType_FBm);
    m_NoiseHeight.SetFractalOctaves(5);     // 층을 얼마나 쌓을지 (4~6 추천)
    m_NoiseHeight.SetFractalLacunarity(2.0f); // 층 사이의 주파수 배율
    m_NoiseHeight.SetFractalGain(0.3f);       // 층 사이의 영향력 배율


    //
    {
        if (auto res = CGameInstance::Get().AddResourceT<E::CResVertexShader>(TAG_RES_GRP_PERMANENT_SHADER, "VS_Block", "./Resources/Shader/Block/Block.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
            m_pResVertexShader = res;
        }
        if (auto res = CGameInstance::Get().AddResourceT<E::CResPixelShader>(TAG_RES_GRP_PERMANENT_SHADER, "PS_Block", "./Resources/Shader/Block/Block.hlsl"))
        {
            if (FAILED(res->Load()))
            {
                return E_FAIL;
            }
            m_pResPixelShader = res;
        }
    }
    {
        {
            //0
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/dirt.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }

        {
            //1
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/stone.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }

        {
            //2
            auto pTexture = CResTexture2D::Create("./Resources/Texture/Blocks/sand.png");
            if (FAILED(pTexture->Load()))
            {
                return E_FAIL;
            }
            CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURES", pTexture);
        }
    }

    {
        CResTexture2DArray::DESC desc{};
        desc.textureId = { "VOXEL_MANAGER_TEX", "TEXTURES" };
        auto pTextureArray = CResTexture2DArray::Create();
        if (FAILED(pTextureArray->Load(desc)))
        {
            return E_FAIL;
        }
        m_pResBlocksTexutreArray = pTextureArray;
        CGameInstance::Get().AddResource("VOXEL_MANAGER_TEX", "TEXTURE_ARRAY", pTextureArray);
    }
    {
        m_pResSamplerPointWrap = CGameInstance::GetConst().GetResourceFirst<E::CResSamplerState>(TAG_RES_GRP_PERMANENT_STATE, TAG_RES_STATE_SS_POINT_WRAP);
    }



    // test light
    DIRECTIONAL_LIGHT light{};

    light.direction = _float3(0.3f, -1.0f, 0.2f); // 아래로 비추는 방향
    // 필요하면 normalize

    light.ambient = _float4(0.2f, 0.2f, 0.25f, 1.0f); // 약간 푸른 톤
    light.diffuse = _float4(1.f, 1.f, 1.f, 1.0f); // 메인 밝기
    light.specular = _float4(0.5f, 0.5f, 0.5f, 1.0f);  // 적당한 하이라이트
    CGameInstance::Get().SetDirectionalLight("0_Test", light);
	return S_OK;
}

UPtr<CVoxelManager2> CVoxelManager2::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    auto pInstance = ToUPtr(new CVoxelManager2{ pDevice , pContext });
    if (FAILED(pInstance->Initialize()))
    {
        return nullptr;
    }
    return pInstance;
}
