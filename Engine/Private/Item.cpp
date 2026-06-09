#include "Item.h"

NS_USING(Engine)

CItem::CItem()
{
}

CItem::~CItem()
{
}

HRESULT CItem::Initialize(void* pArg)
{
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }
    return S_OK;
}

void CItem::PriorityUpdate(E::_float fTimeDelta)
{
}

void CItem::Update(E::_float fTimeDelta)
{
}

void CItem::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CItem::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

UPtr<CItem> CItem::Create()
{
    auto pInstance = ToUPtr(new CItem{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CDropItem");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CItem::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CItem{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CItem");
        return nullptr;
    }

    return pInstance;
}