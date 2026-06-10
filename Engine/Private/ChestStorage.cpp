#include "pch.h"

#include "ChestStorage.h"

NS_USING(Engine)

CChestStorage::CChestStorage()
{
}

CChestStorage::~CChestStorage()
{
}

HRESULT CChestStorage::Initialize(void* pArg)
{

    if (FAILED(CVoxelObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    return S_OK;
}

void CChestStorage::PriorityUpdate(E::_float fTimeDelta)
{
}

void CChestStorage::Update(E::_float fTimeDelta)
{
}

void CChestStorage::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CChestStorage::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

UPtr<CChestStorage> CChestStorage::Create()
{
    auto pInstance = ToUPtr(new CChestStorage{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CChestStorage");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CChestStorage::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CChestStorage{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CChestStorage");
        return nullptr;
    }

    return pInstance;
}