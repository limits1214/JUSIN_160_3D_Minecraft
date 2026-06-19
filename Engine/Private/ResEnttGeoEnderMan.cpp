#include "ResEnttGeoEnderMan.h"

NS_USING(Engine)

HRESULT CResEnttGeoEnderMan::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {
        // 이거 v01이 아니라 그냥지오메트리파일

        m_entityGeometry.texWidth = 64;
        m_entityGeometry.texHeight = 32;

        // 이거 오른팔이 몸통 파고드는 원본 정점인데
        // 이건 백업용이고 아래는 몸통 파고드는거를 모델단에서 맞춘거
        if constexpr (false)
        {
            m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = { 0.0f, 0.0f, 0.0f }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = { 0.0f, 38.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 26.0f, -2.0f }, .size = { 8, 12, 4 }, .uv = { 32, 16 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 24.0f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 0 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "hat",
                .parent = "head",
                .pivot = { 0.0f, 38.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 37.5f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 7), .inflate = -0.5f }
                }
            },
            {
                .name = "rightArm",
                .parent = "body",
                .pivot = { -3.0f, 36.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 8.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "rightItem", // 아이템 장착용 더미 본 (렌더링 큐브 없음)
                .parent = "rightArm",
                .pivot = { -6.0f, 15.0f, 1.0f }
            },
            {
                .name = "leftArm",
                .parent = "body",
                .pivot = { 5.0f, 36.0f, 0.0f },
                .cubes = {
                    {.origin = { 4.0f, 8.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                },
                .mirror = true
            },
            {
                .name = "rightLeg",
                .parent = "body",
                .pivot = { -2.0f, 26.0f, 0.0f },
                .cubes = {
                    {.origin = { -3.0f, -4.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "leftLeg",
                .parent = "body",
                .pivot = { 2.0f, 26.0f, 0.0f },
                .cubes = {
                    {.origin = { 1.0f, -4.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                },
                .mirror = true
            }
            };
        }


        m_entityGeometry.bones = {
            {
                .name = "root",
                .pivot = { 0.0f, 0.0f, 0.0f }
            },
            {
                .name = "body",
                .parent = "root",
                .pivot = { 0.0f, 38.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 26.0f, -2.0f }, .size = { 8, 12, 4 }, .uv = { 32, 16 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "head",
                .parent = "body",
                .pivot = { 0.0f, 24.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 24.0f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 0 }, .texIndex = PackTexId(7, 7) }
                }
            },
            {
                .name = "hat",
                .parent = "head",
                .pivot = { 0.0f, 38.0f, 0.0f },
                .cubes = {
                    {.origin = { -4.0f, 37.5f, -4.0f }, .size = { 8, 8, 8 }, .uv = { 0, 16 }, .texIndex = PackTexId(7, 7), .inflate = -0.5f }
                }
            },
            {
                .name = "rightArm",
                .parent = "body",
                // [수정] leftArm의 pivot.x가 5.0f이므로 대칭점인 -5.0f로 교정 (-3.0f -> -5.0f)
                .pivot = { -5.0f, 36.0f, 0.0f },
                .cubes = {
                        // [수정] leftArm의 origin.x가 4.0f이므로 대칭점인 -6.0f로 교정 (-4.0f -> -6.0f)
                        // (설명: pivot -5.0f에서 바깥쪽으로 크기 2짜리 큐브가 생성되려면 origin.x는 -6.0f이어야 함)
                        {.origin = { -6.0f, 8.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                    }
                },
                {
                    .name = "rightItem",
                    .parent = "rightArm",
                    // [수정] 오른팔 피벗이 -2.0f 밀려났으므로 로케이터 피벗도 동일하게 X축으로 -2.0f 이동 (-6.0f -> -8.0f)
                    .pivot = { -8.0f, 15.0f, 1.0f }
                },
                {
                    .name = "leftArm",
                    .parent = "body",
                    .pivot = { 5.0f, 36.0f, 0.0f },
                    .cubes = {
                        {.origin = { 4.0f, 8.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                    },
                    .mirror = true
                },
                {
                    .name = "rightLeg",
                    .parent = "body",
                    // [참고] leftLeg의 pivot.x가 2.0f이므로 대칭점인 -2.0f는 원래 정확히 잘 맞아 있었습니다.
                    .pivot = { -2.0f, 26.0f, 0.0f },
                    .cubes = {
                        // [수정] leftLeg의 origin.x가 1.0f이므로 대칭점인 -3.0f가 맞습니다. 원래 코드가 이미 대칭형태였습니다.
                        {.origin = { -3.0f, -4.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
                    }
                },
                {
                    .name = "leftLeg",
                    .parent = "body",
                    .pivot = { 2.0f, 26.0f, 0.0f },
                    .cubes = {
                        {.origin = { 1.0f, -4.0f, -1.0f }, .size = { 2, 30, 2 }, .uv = { 56, 0 }, .texIndex = PackTexId(7, 7) }
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

HRESULT CResEnttGeoEnderMan::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoEnderMan::CResEnttGeoEnderMan(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoEnderMan::~CResEnttGeoEnderMan()
{
}

SPtr<CResEnttGeoEnderMan> CResEnttGeoEnderMan::Create()
{
    return ToSPtr(new CResEnttGeoEnderMan{ "" });
}
