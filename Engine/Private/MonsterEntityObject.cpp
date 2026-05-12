#include "MonsterEntityObject.h"

NS_USING(Engine)

CMonsterEntityObject::CMonsterEntityObject()
{
}

CMonsterEntityObject::~CMonsterEntityObject()
{
}

HRESULT CMonsterEntityObject::Initialize(void* pArg)
{
    if (FAILED(CEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
