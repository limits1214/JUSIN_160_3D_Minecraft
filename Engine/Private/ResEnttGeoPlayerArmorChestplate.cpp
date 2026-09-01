#include "pch.h"
#include "ResEnttGeoPlayerArmorChestplate.h"

NS_USING(Engine)

HRESULT CResEnttGeoPlayerArmorChestplate::Load(const std::any& arg)
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
                // armor1에서 덮어씌운 inflate 1.01f 적용
                {.origin = {-4.0f, 12.0f, -2.0f}, .size = {8.0f, 12.0f, 4.0f}, .uv = {16.0f, 16.0f}, .texIndex = PackTexId(7, 2), .inflate = 1.01f }
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
            .pivot = {-5.0f, 22.0f, 0.0f},
            .cubes = {
                // armor1에서 덮어씌운 inflate 1.0f 적용
                {.origin = {-8.0f, 12.0f, -2.0f}, .size = {4.0f, 12.0f, 4.0f}, .uv = {40.0f, 16.0f}, .texIndex = PackTexId(7, 2), .inflate = 1.0f }
            }
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
            .cubes = {
                // armor1에서 덮어씌운 inflate 1.0f 적용
                {.origin = {4.0f, 12.0f, -2.0f}, .size = {4.0f, 12.0f, 4.0f}, .uv = {40.0f, 16.0f}, .texIndex = PackTexId(7, 2), .inflate = 1.0f }
            },
            .mirror = true
        },
        {
            .name = "rightLeg",
            .parent = "body",
            .pivot = {-1.9f, 12.0f, 0.0f}
            // reset: true 이므로 cubes 데이터 생략
        },
        {
            .name = "leftLeg",
            .parent = "body",
            .pivot = {1.9f, 12.0f, 0.0f},
            .mirror = true
            // reset: true 이므로 cubes 데이터 생략
        }
        };

        RightToLeft();

        Scaling();

        BoneSorting();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoPlayerArmorChestplate::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoPlayerArmorChestplate::CResEnttGeoPlayerArmorChestplate(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoPlayerArmorChestplate::~CResEnttGeoPlayerArmorChestplate()
{
}

SPtr<CResEnttGeoPlayerArmorChestplate> CResEnttGeoPlayerArmorChestplate::Create()
{
    return ToSPtr(new CResEnttGeoPlayerArmorChestplate{ "" });
}
