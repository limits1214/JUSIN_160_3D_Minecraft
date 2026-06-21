#include "ResEnttGeoArrow.h"

NS_USING(Engine)

HRESULT CResEnttGeoArrow::Load(const std::any& arg)
{
    if (m_eState == STATE::LOADED)
    {
        return S_OK;
    }
    m_eState = STATE::LOADING;
    {
        m_entityGeometry.texWidth = 32;
        m_entityGeometry.texHeight = 32;

        m_entityGeometry.bones = {
             {
                 .name = "body",
                 .pivot = {0.0f, 1.0f, 0.0f},
                 .cubes = {
                     {
                         .origin = {0.0f, -2.5f, -3.0f},
                         .size = {0.0f, 5.0f, 16.0f},
                         .rotation = {0.0f, 0.0f, 45.0f},
                         .uv {0.0f, 0.0f},
                         .texIndex = PackTexId(5, 2)
                     },
                     {
                         .origin = {0.0f, -2.5f, -3.0f},
                         .size = {0.0f, 5.0f, 16.0f},
                         .rotation = {0.0f, 0.0f, -45.0f},
                         .uv {0.0f, 0.0f},
                         .texIndex = PackTexId(5, 2)
                     },
                     {
                         .origin = {-2.5f, -2.5f, 12.0f},
                         .size = {5.0f, 5.0f, 0.0f},
                         .rotation = {0.0f, 0.0f, 45.0f},
                         .uv {0.0f, 5.0f},
                         .texIndex = PackTexId(5, 2)
                     }
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

HRESULT CResEnttGeoArrow::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoArrow::CResEnttGeoArrow(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoArrow::~CResEnttGeoArrow()
{
}

SPtr<CResEnttGeoArrow> CResEnttGeoArrow::Create()
{
    return ToSPtr(new CResEnttGeoArrow{ "" });
}
