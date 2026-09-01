#include "ResEnttGeoSpider.h"

NS_USING(Engine)

HRESULT CResEnttGeoSpider::Load(const std::any& arg)
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
                .name = "body0", // 거미의 중심축 (가슴 부위)
                .parent = "root",
                .pivot = { 0.0f, 9.0f, 0.0f },
                .cubes = {
                    {.origin = { -3.0f, 6.0f, -3.0f }, .size = { 6, 6, 6 }, .uv = { 0, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "head",
                .parent = "body0",
                .pivot = { 0.0f, 9.0f, -3.0f },
                .cubes = {
                    {.origin = { -4.0f, 5.0f, -11.0f }, .size = { 8, 8, 8 }, .uv = { 32, 4 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "body1", // 뒷몸통 (배 부위)
                .parent = "body0",
                .pivot = { 0.0f, 9.0f, 9.0f },
                .cubes = {
                    {.origin = { -5.0f, 5.0f, 3.0f }, .size = { 10, 8, 12 }, .uv = { 0, 12 }, .texIndex = PackTexId(7, 6) }
                }
            },
            // --- 다리 (Legs) 구간 ---
            {
                .name = "leg0",
                .parent = "body0",
                .pivot = { -4.0f, 9.0f, 2.0f },
                .cubes = {
                    {.origin = { -19.0f, 8.0f, 1.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg1",
                .parent = "body0",
                .pivot = { 4.0f, 9.0f, 2.0f },
                .cubes = {
                    {.origin = { 3.0f, 8.0f, 1.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg2",
                .parent = "body0",
                .pivot = { -4.0f, 9.0f, 1.0f },
                .cubes = {
                    {.origin = { -19.0f, 8.0f, 0.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg3",
                .parent = "body0",
                .pivot = { 4.0f, 9.0f, 1.0f },
                .cubes = {
                    {.origin = { 3.0f, 8.0f, 0.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg4",
                .parent = "body0",
                .pivot = { -4.0f, 9.0f, 0.0f },
                .cubes = {
                    {.origin = { -19.0f, 8.0f, -1.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg5",
                .parent = "body0",
                .pivot = { 4.0f, 9.0f, 0.0f },
                .cubes = {
                    {.origin = { 3.0f, 8.0f, -1.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg6",
                .parent = "body0",
                .pivot = { -4.0f, 9.0f, -1.0f },
                .cubes = {
                    {.origin = { -19.0f, 8.0f, -2.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            },
            {
                .name = "leg7",
                .parent = "body0",
                .pivot = { 4.0f, 9.0f, -1.0f },
                .cubes = {
                    {.origin = { 3.0f, 8.0f, -2.0f }, .size = { 16, 2, 2 }, .uv = { 18, 0 }, .texIndex = PackTexId(7, 6) }
                }
            }
        };


        RightToLeft();

        Scaling();

        BoneSorting();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoSpider::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoSpider::CResEnttGeoSpider(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoSpider::~CResEnttGeoSpider()
{
}

SPtr<CResEnttGeoSpider> CResEnttGeoSpider::Create()
{
    return ToSPtr(new CResEnttGeoSpider{ "" });
}
