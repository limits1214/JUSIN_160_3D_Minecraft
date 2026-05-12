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
    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
