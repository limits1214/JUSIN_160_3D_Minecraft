#include "pch.h"
#include "ResEnttGeoPlayerArmorLeggings.h"

NS_USING(Engine)

HRESULT CResEnttGeoPlayerArmorLeggings::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {
        m_entityGeometry.texWidth = 64;
        m_entityGeometry.texHeight = 32;

        m_entityGeometry.bones = {
            {
                .name = "waist",
                .pivot = {0.0f, 12.0f, 0.0f},
                .neverRender = true
            },
            {
                .name = "body",
                .parent = "waist",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                // armor2에서 덮어씌운 inflate 0.5f 적용 (골반을 덮는 부분)
                {.origin = {-4.0f, 12.0f, -2.0f}, .size = {8.0f, 12.0f, 4.0f}, .uv = {16.0f, 16.0f}, .texIndex = PackTexId(7, 3), .inflate = 0.5f }
            }
        },
        {
            .name = "head",
            .parent = "body",
            .pivot = {0.0f, 24.0f, 0.0f}
            // reset: true 이므로 cubes 데이터 생략
        },
        {
            .name = "hat",
            .parent = "head",
            .pivot = {0.0f, 24.0f, 0.0f}
            // reset: true 이므로 cubes 데이터 생략
        },
        {
            .name = "rightArm",
            .parent = "body",
            .pivot = {-5.0f, 22.0f, 0.0f}
            // reset: true 이므로 cubes 데이터 생략
        },
        {
            .name = "rightItem",
            .parent = "rightArm",
            .pivot = {-6.0f, 15.0f, 1.0f},
            .neverRender = true
        },
        {
            .name = "leftArm",
            .parent = "body",
            .pivot = {5.0f, 22.0f, 0.0f},
            .mirror = true
            // reset: true 이므로 cubes 데이터 생략
        },
        {
            .name = "rightLeg",
            .parent = "body",
            .pivot = {-1.9f, 12.0f, 0.0f},
            .cubes = {
                // armor2에서 덮어씌운 inflate 0.49f 적용
                {.origin = {-3.9f, 0.0f, -2.0f}, .size = {4.0f, 12.0f, 4.0f}, .uv = {0.0f, 16.0f}, .texIndex = PackTexId(7, 3), .inflate = 0.49f }
            }
        },
        {
            .name = "leftLeg",
            .parent = "body",
            .pivot = {1.9f, 12.0f, 0.0f},
            .cubes = {
                // armor2에서 덮어씌운 inflate 0.49f 적용
                {.origin = {-0.1f, 0.0f, -2.0f}, .size = {4.0f, 12.0f, 4.0f}, .uv = {0.0f, 16.0f}, .texIndex = PackTexId(7, 3), .inflate = 0.49f }
            },
            .mirror = true
        }
        };

        RightToLeft();

        Scaling();

        BoneSorting();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoPlayerArmorLeggings::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoPlayerArmorLeggings::CResEnttGeoPlayerArmorLeggings(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoPlayerArmorLeggings::~CResEnttGeoPlayerArmorLeggings()
{
}

SPtr<CResEnttGeoPlayerArmorLeggings> CResEnttGeoPlayerArmorLeggings::Create()
{
    return ToSPtr(new CResEnttGeoPlayerArmorLeggings{ "" });
}
