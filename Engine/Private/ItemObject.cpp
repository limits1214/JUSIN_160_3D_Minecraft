#include "ItemObject.h"

NS_USING(Engine)

CItemObject::CItemObject()
{
}

CItemObject::~CItemObject()
{
}

HRESULT CItemObject::Initialize(void* pArg)
{
    auto* pDesc = static_cast<DESC*>(pArg);
    //m_ItemInfo = pDesc->info;

    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
