#include "HumanoidEntityObject.h"

NS_USING(Engine)

CHumanoidEntityObject::CHumanoidEntityObject()
{
}

CHumanoidEntityObject::~CHumanoidEntityObject()
{
}

HRESULT CHumanoidEntityObject::Initialize(void* pArg)
{
    if (FAILED(CEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
