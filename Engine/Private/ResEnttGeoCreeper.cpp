#include "ResEnttGeoCreeper.h"

NS_USING(Engine)

HRESULT CResEnttGeoCreeper::Load(const std::any& arg)
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
                .name = "root",
                .pivot = { 0.0f, 0.0f, 0.0f }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = { 0.0f, 6.0f, 0.0f }, // 다리가 시작되는 높이(6.0)를 몸통 회전 피벗으로 설정
                .cubes = {
                    {.origin = { -4.0f, 6.0f, -2.0f }, .size = { 8, 12, 4 }, .uv = { 16, 16 }, .texIndex = PackTexId(7, 5) }
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = { 0.0f, 18.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 18.0f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 0 }, .texIndex = PackTexId(7, 5) }
                }
            },
            {
                .name = "leg0", // 오른쪽 뒷다리
                .parent = "body",
                .pivot = { -2.0f, 6.0f, 4.0f },
                .cubes = {
                    {.origin = { -4.0f, 0.0f, 2.0f }, .size = { 4, 6, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 5) }
                }
            },
            {
                .name = "leg1", // 왼쪽 뒷다리
                .parent = "body",
                .pivot = { 2.0f, 6.0f, 4.0f },
                .cubes = {
                    {.origin = { 0.0f, 0.0f, 2.0f }, .size = { 4, 6, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 5) }
                },
                .mirror = true
            },
            {
                .name = "leg2", // 오른쪽 앞다리
                .parent = "body",
                .pivot = { -2.0f, 6.0f, -4.0f },
                .cubes = {
                    {.origin = { -4.0f, 0.0f, -6.0f }, .size = { 4, 6, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 5) }
                }
            },
            {
                .name = "leg3", // 왼쪽 앞다리
                .parent = "body",
                .pivot = { 2.0f, 6.0f, -4.0f },
                .cubes = {
                    {.origin = { 0.0f, 0.0f, -6.0f }, .size = { 4, 6, 4 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 5) }
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

HRESULT CResEnttGeoCreeper::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoCreeper::CResEnttGeoCreeper(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoCreeper::~CResEnttGeoCreeper()
{
}

SPtr<CResEnttGeoCreeper> CResEnttGeoCreeper::Create()
{
    return ToSPtr(new CResEnttGeoCreeper{ "" });
}
