#include "ResEnttGeoZombie.h"

NS_USING(Engine)

HRESULT CResEnttGeoZombie::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {

        m_entityGeometry.texWidth = 64;
        m_entityGeometry.texHeight = 32; // JSON에 정의된 원본 텍스처 높이

        m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = { 0.0f, 0.0f, 0.0f }
            },
            {
                .name = "waist",
                .parent = "root",
                .pivot = { 0.0f, 12.0f, 0.0f }
            },
            {
                .name = "body",
                .parent = "waist",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 12.0f, -2.0f }, .size = { 8, 12, 4 }, .uv = { 16, 16 }, .texIndex = PackTexId(7, 4) }
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 24.0f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 0 }, .texIndex = PackTexId(7, 4) }
                }
            },
            {
                .name = "hat",
                .parent = "head",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 24.0f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 32, 0 }, .texIndex = PackTexId(7, 4), .inflate = 0.5f }
                }
            },
            {
                .name = "rightArm",
                .parent = "body",
                .pivot = { -5.0f, 22.0f, 0.0f },
                .cubes = {
                    {.origin = { -8.0f, 12.0f, -2.0f }, .size = { 4, 12, 4 }, .uv = { 40, 16 }, .texIndex = PackTexId(7, 4) }
                }
            },
            {
                .name = "rightItem",
                .parent = "rightArm",
                .pivot = { -6.0f, 15.0f, 1.0f }
            },
            {
                .name = "leftArm",
                .parent = "body",
                .pivot = { 5.0f, 22.0f, 0.0f },
                .cubes = {
                    {.origin = { 4.0f, 12.0f, -2.0f }, .size = { 4, 12, 4 }, .uv = { 40, 16 }, .texIndex = PackTexId(7, 4) }
                },
                .mirror = true
            },
            {
                .name = "leftItem",
                .parent = "leftArm",
                .pivot = { 6.0f, 15.0f, 1.0f }
            },
            {
                .name = "rightLeg",
                .parent = "body", // JSON 상에서 부모가 waist가 아닌 body로 기입되어 있어 그대로 유지했습니다.
                .pivot = { -1.9f, 12.0f, 0.0f },
                .cubes = {
                    {.origin = { -3.9f, 0.0f, -2.0f }, .size = { 4, 12, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 4) }
                }
            },
            {
                .name = "leftLeg",
                .parent = "body",
                .pivot = { 1.9f, 12.0f, 0.0f },
                .cubes = {
                    {.origin = { -0.1f, 0.0f, -2.0f }, .size = { 4, 12, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 4) }
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

HRESULT CResEnttGeoZombie::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoZombie::CResEnttGeoZombie(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoZombie::~CResEnttGeoZombie()
{
}

SPtr<CResEnttGeoZombie> CResEnttGeoZombie::Create()
{
    return ToSPtr(new CResEnttGeoZombie{ "" });
}
