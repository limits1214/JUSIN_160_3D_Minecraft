#include "ResEnttGeoCow.h"

NS_USING(Engine)

HRESULT CResEnttGeoCow::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {


        ;

        m_entityGeometry.texHeight = 64;
        m_entityGeometry.texWidth = 64;
        m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = {0,0,-1}
            },
            {
                .name = "head",
                .parent = "root",
                .pivot = { 0, 20, -9 },
                .cubes = {
                    {.origin = { -4, 16, -15 }, .size = { 8, 8, 6 }, .uv { 0,0 }, .texIndex = PackTexId(8, 2) },
                    {.origin = { -3, 16, -16 }, .size = { 6, 3, 1 }, .uv { 1, 33 }, .texIndex = PackTexId(8, 2)},
                    {.origin = { -5, 22, -14 }, .size = { 1, 3, 1 }, .uv { 22, 0 }, .texIndex = PackTexId(8, 2) },
                    {.origin = { 4, 22, -14 }, .size = { 1, 3, 1 }, .uv { 22, 0 }, .texIndex = PackTexId(8, 2) },
                },
                .locators = {
                    {"lead", { 0, 20, -9 }}
                }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = { 0, 19, 1 },
                .cubes = {
                     {.origin = { -6, 29, 14 }, .size = { 12, 18, 10 }, .rotation = {90, 0, 0},.uv {18, 4}, .texIndex = PackTexId(8, 2), .pivot = { 0, 18, 20}  },
                     {.origin = { -2, 29, 13 }, .size = { 4, 6, 1 }, .rotation = {90, 0, 0},.uv {52, 0 }, .texIndex = PackTexId(8, 2) , .pivot = {0, 18, 20} },
                },
                .locators = {
                    {"lead_hold", {0, 34, 0 }}
                }
            },
            {
                .name = "leg0",
                .parent = "root",
                .pivot = { -4, 12, 6  },
                .cubes = {
                     {.origin = { -6, 0, 4 }, .size = { 4, 12, 4 }, .uv {0, 16}, .texIndex = PackTexId(8, 2)  },
                }
            },

            {
                .name = "leg1",
                .parent = "root",
                .pivot = { 4, 12, 6 },
                .cubes = {
                     {.origin = { 2, 0, 4 }, .size = { 4, 12, 4 }, .uv {0, 16}, .texIndex = PackTexId(8, 2)  },
                },
                .mirror = true,
            },

            {
               .name = "leg2",
               .parent = "root",
               .pivot = {-4, 12, -7},
               .cubes = {
                    {.origin = { -6, 0, -8}, .size = {4, 12, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 2)  },
               }
            },
             {
                .name = "leg3",
                .parent = "root",
                .pivot = {4, 12, -7},
                .cubes = {
                     {.origin = { 2, 0, -8 }, .size = {4, 12, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 2)  },
                },
                .mirror = true,
            },
        };


        ZReverse();


    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoCow::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoCow::CResEnttGeoCow(const _string& sPath)
    : CResEnttGeo{sPath}
{
}

CResEnttGeoCow::~CResEnttGeoCow()
{
}

SPtr<CResEnttGeoCow> CResEnttGeoCow::Create()
{
    return ToSPtr(new CResEnttGeoCow{ "" });
}
