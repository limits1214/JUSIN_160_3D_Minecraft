#include "AnimalEntityObject.h"

NS_USING(Engine)

CAnimalEntityObject::CAnimalEntityObject()
{
}

CAnimalEntityObject::~CAnimalEntityObject()
{
}

HRESULT CAnimalEntityObject::Initialize(void* pArg)
{
    if (FAILED(CEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

	return S_OK;
}
