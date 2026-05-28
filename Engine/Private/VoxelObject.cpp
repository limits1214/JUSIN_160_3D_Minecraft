#include "VoxelObject.h"

NS_USING(Engine)

CVoxelObject::CVoxelObject()
{
}

CVoxelObject::~CVoxelObject()
{
}

HRESULT CVoxelObject::Initialize(void* pArg)
{
    if (FAILED(CGameObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}
