#include "pch.h"

#include "FurnaceStorage.h"

NS_USING(Engine)

CFurnaceStorage::CFurnaceStorage()
{
}

CFurnaceStorage::~CFurnaceStorage()
{
}

HRESULT CFurnaceStorage::Initialize(void* pArg)
{

    if (FAILED(CVoxelObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CFurnaceStorage::PriorityUpdate(E::_float fTimeDelta)
{
}

void CFurnaceStorage::Update(E::_float fTimeDelta)
{
}

void CFurnaceStorage::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CFurnaceStorage::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

UPtr<CFurnaceStorage> CFurnaceStorage::Create()
{
    auto pInstance = ToUPtr(new CFurnaceStorage{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CFurnaceStorage");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CFurnaceStorage::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CFurnaceStorage{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CFurnaceStorage");
        return nullptr;
    }

    return pInstance;
}