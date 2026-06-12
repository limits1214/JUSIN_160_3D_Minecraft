#include "ResEnttGeoPig.h"

NS_USING(Engine)

HRESULT CResEnttGeoPig::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {




        m_entityGeometry.texHeight = 64;
        m_entityGeometry.texWidth = 64;



        m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = {0,0,0}
            },
            {
                .name = "head",
                .parent = "root",
                .pivot = {0, 12, -7},
                .cubes = {
                    {.origin = {-4, 8, -15}, .size = {8, 8, 8}, .uv {0,0}, .texIndex = PackTexId(8, 0) },
                    {.origin = {-2, 9, -16}, .size = {4, 3, 1}, .uv {16, 16}, .texIndex = PackTexId(8, 0) }
                },
                .locators = {
                    {"lead", {0, 12, -7}}
                }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = {0,0,0},
                .cubes = {
                     {.origin = {-5, 2, -5}, .size = {10, 16, 8}, .rotation = {90, 0, 0},.uv {28, 32}, .texIndex = PackTexId(8, 1), .inflate = 0.5  },
                     {.origin = {-5, 2, -5}, .size = {10, 16, 8}, .rotation = {90, 0, 0},.uv {28, 8}, .texIndex = PackTexId(8, 0)  },
                },
                .locators = {
                    {"lead_hold", {0, 32, 1}}
                }
            },
            {
                .name = "leg0",
                .parent = "root",
                .pivot = {-3, 6, 6},
                .cubes = {
                     {.origin = {-5, 0, 4}, .size = {4, 6, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 0)  },
                }
            },

            {
                .name = "leg1",
                .parent = "root",
                .pivot = {3, 6, 6},
                .cubes = {
                     {.origin = {1, 0, 4}, .size = {4, 6, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 0)  },
                },
                .mirror = true,
            },

            {
                .name = "leg3",
                .parent = "root",
                .pivot = {3, 6, -6},
                .cubes = {
                     {.origin = { 1, 0, -8}, .size = {4, 6, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 0)  },
                },
                .mirror = true,
            },

            {
               .name = "leg2",
               .parent = "root",
               .pivot = {-3, 6, -6},
               .cubes = {
                    {.origin = { -5, 0, -8}, .size = {4, 6, 4}, .uv {0, 16}, .texIndex = PackTexId(8, 0)  },
               }
            }
        };


        RightToLeft();

        Scaling();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoPig::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoPig::CResEnttGeoPig(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoPig::~CResEnttGeoPig()
{
}

SPtr<CResEnttGeoPig> CResEnttGeoPig::Create()
{
    return ToSPtr(new CResEnttGeoPig{ "" });
}
