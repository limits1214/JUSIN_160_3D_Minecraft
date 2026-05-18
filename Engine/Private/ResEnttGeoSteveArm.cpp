#include "ResEnttGeoSteveArm.h"

NS_USING(Engine)

HRESULT CResEnttGeoSteveArm::Load(const std::any& arg)
{
    if (FAILED(CResEnttGeoSteve::Load(arg)))
    {
        return E_FAIL;
    }

    for (auto iter = m_entityGeometry.bones.begin(); iter != m_entityGeometry.bones.end();)
    {
        if (iter->name == "rightArm")
        {
            iter->parent = "";
            iter->pivot = {};
            iter->cubes[0].origin = { -2 * 0.0625f, -12 * 0.0625f, -2 * 0.0625f };
            ++iter;
        }
        else
        {
            iter = m_entityGeometry.bones.erase(iter);
        }
    }
    
    return S_OK;
}

HRESULT CResEnttGeoSteveArm::Unload(const std::any& arg)
{
    return S_OK;
}

CResEnttGeoSteveArm::CResEnttGeoSteveArm(const _string& sPath)
    : CResEnttGeoSteve{ sPath }
{
}

CResEnttGeoSteveArm::~CResEnttGeoSteveArm()
{
}

SPtr<CResEnttGeoSteveArm> CResEnttGeoSteveArm::Create()
{
    return ToSPtr(new CResEnttGeoSteveArm{ "" });
}
