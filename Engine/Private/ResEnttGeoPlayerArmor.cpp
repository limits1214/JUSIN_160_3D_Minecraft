#include "ResEnttGeoPlayerArmor.h"

NS_USING(Engine)

HRESULT CResEnttGeoPlayerArmor::Load(const std::any& arg)
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
                .name = "rightLeg",
                .parent = "body",
                .pivot = {-1.9f, 12.0f, 0.0f},
                .cubes = {
                    {.origin = {-3.9f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {0, 16}, .annotation = "armor2"}
                }
            },
            {
                .name = "leftLeg",
                .parent = "body",
                .pivot = {1.9f, 12.0f, 0.0f},
                
                .cubes = {
                    {.origin = {-0.1f, 0.0f, -2.0f}, .size = {4, 12, 4}, .uv = {0, 16}, .annotation = "armor2"}
                },
                .mirror = true,
            }
          };


        RightToLeft();

        Scaling();
    }
    m_eState = STATE::LOADED;
    return S_OK;
}

HRESULT CResEnttGeoPlayerArmor::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoPlayerArmor::CResEnttGeoPlayerArmor(const _string& sPath)
    : CResEnttGeo{ sPath }
{
}

CResEnttGeoPlayerArmor::~CResEnttGeoPlayerArmor()
{
}

SPtr<CResEnttGeoPlayerArmor> CResEnttGeoPlayerArmor::Create()
{
    return ToSPtr(new CResEnttGeoPlayerArmor{ "" });
}
