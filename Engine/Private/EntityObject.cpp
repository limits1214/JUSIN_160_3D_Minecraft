#include "EntityObject.h"

NS_USING(Engine)

CEntityObject::CEntityObject()
{
}

CEntityObject::~CEntityObject()
{
}

HRESULT CEntityObject::Initialize(void* pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
