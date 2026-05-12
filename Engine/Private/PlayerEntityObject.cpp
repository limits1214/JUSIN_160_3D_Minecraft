#include "PlayerEntityObject.h"

NS_USING(Engine)

CPlayerEntityObject::CPlayerEntityObject()
{
}

CPlayerEntityObject::~CPlayerEntityObject()
{
}

HRESULT CPlayerEntityObject::Initialize(void* pArg)
{
    if (FAILED(CEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
