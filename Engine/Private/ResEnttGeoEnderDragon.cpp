#include "ResEnttGeoEnderDragon.h"

NS_USING(Engine)

HRESULT CResEnttGeoEnderDragon::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {
        // 이거 v01이 아니라 그냥지오메트리파일

        m_entityGeometry.texWidth = 256;
        m_entityGeometry.texHeight = 256;

        // 텍스처 식별자 (12, 8) 조립
        uint32_t dragonTexId = PackTexId(12, 8);

        m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = { 0.0f, 24.0f, 0.0f }
            },
            {
                .name = "head",
                .parent = "root",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -6.0f, 20.0f, -24.0f }, .size = { 12.0f, 5.0f, 16.0f }, .uv = { 176.0f, 44.0f }, .texIndex = dragonTexId },
                    {.origin = { -8.0f, 16.0f, -10.0f }, .size = { 16.0f, 16.0f, 16.0f }, .uv = { 112.0f, 30.0f }, .texIndex = dragonTexId },
                    {.origin = { -5.0f, 32.0f, -4.0f }, .size = { 2.0f, 4.0f, 6.0f }, .uv = { 0.0f, 0.0f }, .texIndex = dragonTexId },
                    {.origin = { -5.0f, 25.0f, -22.0f }, .size = { 2.0f, 2.0f, 4.0f }, .uv = { 112.0f, 0.0f }, .texIndex = dragonTexId },
                    {.origin = { 3.0f, 32.0f, -4.0f }, .size = { 2.0f, 4.0f, 6.0f }, .uv = { 0.0f, 0.0f }, .texIndex = dragonTexId },
                    {.origin = { 3.0f, 25.0f, -22.0f }, .size = { 2.0f, 2.0f, 4.0f }, .uv = { 112.0f, 0.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "jaw",
                .parent = "head",
                .pivot = { 0.0f, 20.0f, -8.0f },
                .cubes = {
                    {.origin = { -6.0f, 16.0f, -24.0f }, .size = { 12.0f, 4.0f, 16.0f }, .uv = { 176.0f, 65.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "neck",
                .parent = "root",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -5.0f, 19.0f, -5.0f }, .size = { 10.0f, 10.0f, 10.0f }, .uv = { 192.0f, 104.0f }, .texIndex = dragonTexId },
                    {.origin = { -1.0f, 29.0f, -3.0f }, .size = { 2.0f, 4.0f, 6.0f }, .uv = { 48.0f, 0.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = { 0.0f, 20.0f, 8.0f },
                .cubes = {
                    {.origin = { -12.0f, -4.0f, -8.0f }, .size = { 24.0f, 24.0f, 64.0f }, .uv = { 0.0f, 0.0f }, .texIndex = dragonTexId },
                    {.origin = { -1.0f, 20.0f, -2.0f }, .size = { 2.0f, 6.0f, 12.0f }, .uv = { 220.0f, 53.0f }, .texIndex = dragonTexId },
                    {.origin = { -1.0f, 20.0f, 18.0f }, .size = { 2.0f, 6.0f, 12.0f }, .uv = { 220.0f, 53.0f }, .texIndex = dragonTexId },
                    {.origin = { -1.0f, 20.0f, 38.0f }, .size = { 2.0f, 6.0f, 12.0f }, .uv = { 220.0f, 53.0f }, .texIndex = dragonTexId }
                }
            },

            // ------------------------------------------------------------------------
            // WINGS (날개 본 및 내부 큐브 피벗 매칭)
            // ------------------------------------------------------------------------
            {
                .name = "wing",
                .parent = "body",
                .pivot = { -12.0f, 19.0f, 2.0f },
                .cubes = {
                    {.origin = { -68.0f, 15.0f, -2.0f }, .size = { 56.0f, 8.0f, 8.0f }, .uv = { 112.0f, 88.0f }, .texIndex = dragonTexId, .pivot = { -12.0f, 19.0f, 2.0f } },
                    // 두께가 0인 날개 평면막 메시는 uvSize를 명시적으로 세팅해 줍니다.
                    {.origin = { -68.0f, 19.0f, 4.0f }, .size = { 56.0f, 0.0f, 56.0f }, .uv = { -56.0f, 88.0f }, .texIndex = dragonTexId, .uvSize = { 56.0f, 0.0f, 56.0f }, .pivot = { -12.0f, 19.0f, 2.0f } }
                }
            },
            {
                .name = "wingtip",
                .parent = "wing",
                .pivot = { -56.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -112.0f, 22.0f, -2.0f }, .size = { 56.0f, 4.0f, 4.0f }, .uv = { 112.0f, 136.0f }, .texIndex = dragonTexId, .pivot = { -56.0f, 24.0f, 0.0f } },
                    {.origin = { -112.0f, 24.0f, 2.0f }, .size = { 56.0f, 0.0f, 56.0f }, .uv = { -56.0f, 144.0f }, .texIndex = dragonTexId, .uvSize = { 56.0f, 0.0f, 56.0f }, .pivot = { -56.0f, 24.0f, 0.0f } }
                }
            },
            {
                .name = "wing1",
                .parent = "body",
                .pivot = { 12.0f, 19.0f, 2.0f },
                .cubes = {
                    {.origin = { 12.0f, 15.0f, -2.0f }, .size = { 56.0f, 8.0f, 8.0f }, .uv = { 112.0f, 88.0f }, .texIndex = dragonTexId, .pivot = { 12.0f, 19.0f, 2.0f } },
                    {.origin = { 12.0f, 19.0f, 4.0f }, .size = { 56.0f, 0.0f, 56.0f }, .uv = { -56.0f, 88.0f }, .texIndex = dragonTexId, .uvSize = { 56.0f, 0.0f, 56.0f }, .pivot = { 12.0f, 19.0f, 2.0f } }
                },
                .mirror = true
            },
            {
                .name = "wingtip1",
                .parent = "wing1",
                .pivot = { 56.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { 56.0f, 22.0f, -2.0f }, .size = { 56.0f, 4.0f, 4.0f }, .uv = { 112.0f, 136.0f }, .texIndex = dragonTexId, .pivot = { 56.0f, 24.0f, 0.0f } },
                    {.origin = { 56.0f, 24.0f, 2.0f }, .size = { 56.0f, 0.0f, 56.0f }, .uv = { -56.0f, 144.0f }, .texIndex = dragonTexId, .uvSize = { 56.0f, 0.0f, 56.0f }, .pivot = { 56.0f, 24.0f, 0.0f } }
                },
                .mirror = true
            },

            // ------------------------------------------------------------------------
            // LEGS (다리 파트)
            // ------------------------------------------------------------------------
            {
                .name = "rearleg",
                .parent = "body",
                .pivot = { -16.0f, 8.0f, 42.0f },
                .cubes = {
                    {.origin = { -24.0f, -20.0f, 34.0f }, .size = { 16.0f, 32.0f, 16.0f }, .uv = { 0.0f, 0.0f }, .texIndex = dragonTexId, .pivot = { -16.0f, 8.0f, 42.0f } }
                }
            },
            {
                .name = "rearleg1",
                .parent = "body",
                .pivot = { 16.0f, 8.0f, 42.0f },
                .cubes = {
                    {.origin = { 8.0f, -20.0f, 34.0f }, .size = { 16.0f, 32.0f, 16.0f }, .uv = { 0.0f, 0.0f }, .texIndex = dragonTexId, .pivot = { 16.0f, 8.0f, 42.0f } }
                },
                .mirror = true
            },
            {
                .name = "frontleg",
                .parent = "body",
                .pivot = { -12.0f, 4.0f, 2.0f },
                .cubes = {
                    {.origin = { -16.0f, -16.0f, -2.0f }, .size = { 8.0f, 24.0f, 8.0f }, .uv = { 112.0f, 104.0f }, .texIndex = dragonTexId, .pivot = { -12.0f, 4.0f, 2.0f } }
                }
            },
            {
                .name = "frontleg1",
                .parent = "body",
                .pivot = { 12.0f, 4.0f, 2.0f },
                .cubes = {
                    {.origin = { 8.0f, -16.0f, -2.0f }, .size = { 8.0f, 24.0f, 8.0f }, .uv = { 112.0f, 104.0f }, .texIndex = dragonTexId, .pivot = { 12.0f, 4.0f, 2.0f } }
                },
                .mirror = true
            },

            // ------------------------------------------------------------------------
            // TIPS & FEET (하위 컴포넌트 계층)
            // ------------------------------------------------------------------------
            {
                .name = "rearlegtip",
                .parent = "rearleg",
                .pivot = { 0.0f, -8.0f, -4.0f },
                .cubes = {
                    {.origin = { -6.0f, -38.0f, -4.0f }, .size = { 12.0f, 32.0f, 12.0f }, .uv = { 196.0f, 0.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "rearlegtip1",
                .parent = "rearleg1",
                .pivot = { 0.0f, -8.0f, -4.0f },
                .cubes = {
                    {.origin = { -6.0f, -38.0f, -4.0f }, .size = { 12.0f, 32.0f, 12.0f }, .uv = { 196.0f, 0.0f }, .texIndex = dragonTexId }
                },
                .mirror = true
            },
            {
                .name = "frontlegtip",
                .parent = "frontleg",
                .pivot = { 0.0f, 4.0f, -1.0f },
                .cubes = {
                    {.origin = { -3.0f, -19.0f, -4.0f }, .size = { 6.0f, 24.0f, 6.0f }, .uv = { 226.0f, 138.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "frontlegtip1",
                .parent = "frontleg1",
                .pivot = { 0.0f, 4.0f, -1.0f },
                .cubes = {
                    {.origin = { -3.0f, -19.0f, -4.0f }, .size = { 6.0f, 24.0f, 6.0f }, .uv = { 226.0f, 138.0f }, .texIndex = dragonTexId }
                },
                .mirror = true
            },
            {
                .name = "rearfoot",
                .parent = "rearlegtip",
                .pivot = { 0.0f, -7.0f, 4.0f },
                .cubes = {
                    {.origin = { -9.0f, -13.0f, -16.0f }, .size = { 18.0f, 6.0f, 24.0f }, .uv = { 112.0f, 0.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "rearfoot1",
                .parent = "rearlegtip1",
                .pivot = { 0.0f, -7.0f, 4.0f },
                .cubes = {
                    {.origin = { -9.0f, -13.0f, -16.0f }, .size = { 18.0f, 6.0f, 24.0f }, .uv = { 112.0f, 0.0f }, .texIndex = dragonTexId }
                },
                .mirror = true
            },
            {
                .name = "frontfoot",
                .parent = "frontlegtip",
                .pivot = { 0.0f, 1.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, -3.0f, -12.0f }, .size = { 8.0f, 4.0f, 16.0f }, .uv = { 144.0f, 104.0f }, .texIndex = dragonTexId }
                }
            },
            {
                .name = "frontfoot1",
                .parent = "frontlegtip1",
                .pivot = { 0.0f, 1.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, -3.0f, -12.0f }, .size = { 8.0f, 4.0f, 16.0f }, .uv = { 144.0f, 104.0f }, .texIndex = dragonTexId }
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

HRESULT CResEnttGeoEnderDragon::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoEnderDragon::CResEnttGeoEnderDragon(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoEnderDragon::~CResEnttGeoEnderDragon()
{
}

SPtr<CResEnttGeoEnderDragon> CResEnttGeoEnderDragon::Create()
{
    return ToSPtr(new CResEnttGeoEnderDragon{ "" });
}
