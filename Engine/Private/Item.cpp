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
    return E_NOTIMPL;
}

UPtr<CPrototype> CItem::Clone(void* pArg)
{
    return UPtr<CPrototype>();
}
