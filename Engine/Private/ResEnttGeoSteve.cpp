#include "ResEnttGeoSteve.h"

NS_USING(Engine)

HRESULT CResEnttGeoSteve::Load(const std::any& arg)
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
                .pivot = {0.0f, 0.0f, 0.0f}
            },
            {
                .name = "waist",
                .parent = "root",
                .pivot = {0.0f, 12.0f, 0.0f}
            },
            {
                .name = "body",
                .parent = "waist",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 12.0f, -2.0f}, .size = {8, 12, 4}, .uv = {16, 16}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "jacket",
                .parent = "body",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 12.0f, -2.0f}, .size = {8, 12, 4}, .uv = {16, 32}, .texIndex = PackTexId(8, 3), .inflate = 0.25f}
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 24.0f, -4.0f}, .size = {8, 8, 8}, .uv = {0, 0}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "hat",
                .parent = "head",
                .pivot = {0.0f, 24.0f, 0.0f},
                .cubes = {
                    {.origin = {-4.0f, 24.0f, -4.0f}, .size = {8, 8, 8}, .uv = {32, 0}, .texIndex = PackTexId(8, 3), .inflate = 0.5f}
                }
            },
            {
                .name = "cape",
                .parent = "body",
                .pivot = {0.0f, 24.0f, 3.0f}
            },
            {
                .name = "rightArm",
                .parent = "body",
                .pivot = {-5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {-8.0f, 12.0f, -2.0f}, .size = {4, 12, 4}, .uv = {40, 16}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "rightSleeve",
                .parent = "rightArm",
                .pivot = {-5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {-8.0f, 12.0f, -2.0f}, .size = {4, 12, 4}, .uv = {40, 32}, .texIndex = PackTexId(8, 3), .inflate = 0.25f}
                }
            },
            {
                .name = "rightItem",
                .parent = "rightArm",
                .pivot = {-6.0f, 15.0f, 1.0f},
                .locators = {
                    {"lead_hold", {-6.0f, 15.0f, 1.0f}}
                }
            },
            {
                .name = "leftArm",
                .parent = "body",
                .pivot = {5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {4.0f, 12.0f, -2.0f}, .size = {4, 12, 4}, .uv = {32, 48}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "leftSleeve",
                .parent = "leftArm",
                .pivot = {5.0f, 22.0f, 0.0f},
                .cubes = {
                    {.origin = {4.0f, 12.0f, -2.0f}, .size = {4, 12, 4}, .uv = {48, 48}, .texIndex = PackTexId(8, 3), .inflate = 0.25f}
                }
            },
            {
                .name = "leftItem",
                .parent = "leftArm",
                .pivot = {6.0f, 15.0f, 1.0f}
            },
            {
                .name = "rightLeg",
                .parent = "root",
                .pivot = {-1.9f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-3.9f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {0, 16}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "rightPants",
                .parent = "rightLeg",
                .pivot = {-1.9f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-3.9f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {0, 32}, .texIndex = PackTexId(8, 3), .inflate = 0.25f}
                }
            },
            {
                .name = "leftLeg",
                .parent = "root",
                .pivot = {1.9f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-0.1f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {16, 48}, .texIndex = PackTexId(8, 3)}
                }
            },
            {
                .name = "leftPants",
                .parent = "leftLeg",
                .pivot = {1.9f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-0.1f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {0, 48}, .texIndex = PackTexId(8, 3), .inflate = 0.25f}
                }
            }
        };


        RightToLeft();

        Scaling();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoSteve::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoSteve::CResEnttGeoSteve(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoSteve::~CResEnttGeoSteve()
{
}

SPtr<CResEnttGeoSteve> CResEnttGeoSteve::Create()
{
    return ToSPtr(new CResEnttGeoSteve{ "" });
}
