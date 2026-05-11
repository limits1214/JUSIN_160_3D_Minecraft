#include "ResEnttGeoChicken.h"

NS_USING(Engine)

HRESULT CResEnttGeoChicken::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {

        m_entityGeometry.texHeight = 32;
        m_entityGeometry.texWidth = 64;

        m_entityGeometry.bones = {
             {
                 .name = "body",
                 .pivot = {0.0f, 8.0f, 0.0f},
                 .cubes = {
                     {.origin = {-3.0f, 4.0f, -3.0f}, .size = {6.0f, 8.0f, 6.0f}, .rotation = {90.0f, 0.0f, 0.0f}, .uv {0.0f, 9.0f}, .texIndex = PackTexId(0, 0) },
                 },
                 .locators = {
                     {"lead_hold", {0.0f, 28.0f, 0.0f}}
                 }
             },
             {
                 .name = "head",
                 .pivot = {0.0f, 9.0f, -4.0f},
                 .cubes = {
                     {.origin = {-2.0f, 9.0f, -6.0f}, .size = {4.0f, 6.0f, 3.0f}, .uv {0.0f, 0.0f}, .texIndex = PackTexId(0, 0)},
                 },
                 .locators = {
                     {"lead", {0.0f, 9.0f, -4.0f}}
                 }
             },
             {
                 .name = "comb",
                 .parent = "head",
                 .pivot = {0.0f, 9.0f, -4.0f},
                 .cubes = {
                     {.origin = {-1.0f, 9.0f, -7.0f}, .size = {2.0f, 2.0f, 2.0f}, .uv {14.0f, 4.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
             {
                 .name = "beak",
                 .parent = "head",
                 .pivot = {0.0f, 9.0f, -4.0f},
                 .cubes = {
                     {.origin = {-2.0f, 11.0f, -8.0f}, .size = {4.0f, 2.0f, 2.0f}, .uv {14.0f, 0.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
             {
                 .name = "leg0",
                 .pivot = {-2.0f, 5.0f, 1.0f},
                 .cubes = {
                     {.origin = {-3.0f, 0.0f, -2.0f}, .size = {3.0f, 5.0f, 3.0f}, .uv {26.0f, 0.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
             {
                 .name = "leg1",
                 .pivot = {1.0f, 5.0f, 1.0f},
                 .cubes = {
                     {.origin = {0.0f, 0.0f, -2.0f}, .size = {3.0f, 5.0f, 3.0f}, .uv {26.0f, 0.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
             {
                 .name = "wing0",
                 .pivot = {-3.0f, 11.0f, 0.0f},
                 .cubes = {
                     {.origin = {-4.0f, 7.0f, -3.0f}, .size = {1.0f, 4.0f, 6.0f}, .uv {24.0f, 13.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
             {
                 .name = "wing1",
                 .pivot = {3.0f, 11.0f, 0.0f},
                 .cubes = {
                     {.origin = {3.0f, 7.0f, -3.0f}, .size = {1.0f, 4.0f, 6.0f}, .uv {24.0f, 13.0f}, .texIndex = PackTexId(0, 0)},
                 },
             },
        };


        ZReverse();

    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoChicken::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoChicken::CResEnttGeoChicken(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoChicken::~CResEnttGeoChicken()
{
}

SPtr<CResEnttGeoChicken> CResEnttGeoChicken::Create()
{
    return ToSPtr(new CResEnttGeoChicken{ "" });
}
