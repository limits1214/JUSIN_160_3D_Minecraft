#include "ResEnttGeoSkeleton.h"

NS_USING(Engine)

HRESULT CResEnttGeoSkeleton::Load(const std::any& arg)
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
                .name = "waist",
                .pivot = {0.0f, 12.0f, 0.0f}
            },
            {
                .name = "body",
                .parent = "waist",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 12.0f, -2.0f}, .size = {8, 12, 4}, .uv = {16, 16}, .texIndex = PackTexId(0, 1)}
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 24.0f, -4.0f}, .size = {8, 8, 8}, .uv = {0, 0}, .texIndex = PackTexId(0, 1)}
                }
            },
             {
                .name = "hat",
                .parent = "head",
                .pivot = {0.0, 24.0, 0.0},
                .cubes = {
                    {.origin = {-4.0, 24.0, -4.0}, .size = {8, 8, 8}, .uv = {32, 0}, .texIndex = PackTexId(0, 1), .inflate = 0.5}
                }
            },
            {
                .name = "rightArm",
                .parent = "body",
                .pivot = {-5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {-6.0f, 12.0f, -1.0f}, .size = {2, 12, 2}, .uv = {40, 16}, .texIndex = PackTexId(0, 1)}
                }
            },
            {
                .name = "rightItem",
                .parent = "rightArm",
                .pivot = {-6, 15, 1},
            },
            {
                .name = "leftArm",
                .parent = "body",
                .pivot = {5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {4.0f, 12.0f, -1.0f}, .size = {2, 12, 2}, .uv = {40, 16}, .texIndex = PackTexId(0, 1)}
                }
                , .mirror = true
            },
            {
                .name = "leftItem",
                .parent = "leftArm",
                .pivot = {6.0, 15.0, 1.0},
            },
            {
                .name = "rightLeg",
                .parent = "body",
                .pivot = {-2.0f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-3.0f, 0.0f, -1.0f}, .size = {2, 12, 2}, .uv = {0, 16}, .texIndex = PackTexId(0, 1)}
                }
            },
            {
                .name = "leftLeg",
                .parent = "body",
                .pivot = {2.0f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {1.0f, 0.0f, -1.0f}, .size = {2, 12, 2}, .uv = {0, 16}, .texIndex = PackTexId(0, 1), }
                },
                .mirror = true
            }
        };


        ZReverse();


    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoSkeleton::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoSkeleton::CResEnttGeoSkeleton(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoSkeleton::~CResEnttGeoSkeleton()
{
}

SPtr<CResEnttGeoSkeleton> CResEnttGeoSkeleton::Create()
{
    return ToSPtr(new CResEnttGeoSkeleton{ "" });
}
