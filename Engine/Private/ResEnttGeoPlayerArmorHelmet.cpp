#include "pch.h"

#include "ResEnttGeoPlayerArmorHelmet.h"

NS_USING(Engine)

HRESULT CResEnttGeoPlayerArmorHelmet::Load(const std::any& arg)
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
                    .pivot = {0.0f, 24.0f, 0.0f}
                    // reset: true 이므로 cubes 데이터 생략 (렌더링 안 함)
                },
                {
                    .name = "head",
                    .parent = "body",
                    .pivot = {0.0f, 24.0f, 0.0f},
                    .cubes = {
                        {.origin = {-4.0f, 24.0f, -4.0f}, .size = {8.0f, 8.0f, 8.0f}, .uv = {0.0f, 0.0f}, .texIndex = PackTexId(7, 2), .inflate = 1.0f } // armor1에서 inflate 1.0 덮어쓰기
                    }
                },
                {
                    .name = "hat",
                    .parent = "head",
                    .pivot = {0.0f, 24.0f, 0.0f},
                    .cubes = {
                        {.origin = {-4.0f, 24.0f, -4.0f}, .size = {8.0f, 8.0f, 8.0f}, .uv = {32.0f, 0.0f}, .texIndex = PackTexId(7, 2), .inflate = 1.5f } // armor1에서 inflate 1.5 덮어쓰기
                    }
                // armor1에서 neverRender: false 가 되었으므로 기본값(false)으로 두기 위해 생략
            },
            {
                .name = "rightArm",
                .parent = "body",
                .pivot = {-5.0f, 22.0f, 0.0f}
                // reset: true 이므로 cubes 생략
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
                // reset: true 이므로 cubes 생략
            },
            {
                .name = "rightLeg",
                .parent = "body",
                .pivot = {-1.9f, 12.0f, 0.0f}
                // reset: true 이므로 cubes 생략
            },
            {
                .name = "leftLeg",
                .parent = "body",
                .pivot = {1.9f, 12.0f, 0.0f},
                .mirror = true
                // reset: true 이므로 cubes 생략
            }
        };

        RightToLeft();

        Scaling();

        BoneSorting();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoPlayerArmorHelmet::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoPlayerArmorHelmet::CResEnttGeoPlayerArmorHelmet(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoPlayerArmorHelmet::~CResEnttGeoPlayerArmorHelmet()
{
}

SPtr<CResEnttGeoPlayerArmorHelmet> CResEnttGeoPlayerArmorHelmet::Create()
{
    return ToSPtr(new CResEnttGeoPlayerArmorHelmet{ "" });
}
