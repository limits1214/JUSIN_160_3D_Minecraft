#include "HandHeldBlock.h"
#include "GameInstance.h"
#include "Resources.h"

NS_USING(Engine)

CHandHeldBlock::CHandHeldBlock()
{
}

CHandHeldBlock::~CHandHeldBlock()
{
}

HRESULT CHandHeldBlock::Initialize(void* pArg)
{
    if (FAILED(CHandHeldItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }
    return S_OK;
}

void CHandHeldBlock::PriorityUpdate(E::_float fTimeDelta)
{
}

void CHandHeldBlock::Update(E::_float fTimeDelta)
{
}

void CHandHeldBlock::LateUpdate(E::_float fTimeDelta)
{
    E::CGameInstance::Get().AddRenderObject(E::RENDERGROUP::NONBLEND, this);
}

HRESULT CHandHeldBlock::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

UPtr<CHandHeldBlock> CHandHeldBlock::Create()
{
    auto pInstance = ToUPtr(new CHandHeldBlock{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CHandHeldBlock");
        return nullptr;
    }

    return pInstance;
}


UPtr<CPrototype> CHandHeldBlock::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CHandHeldBlock{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CHandHeldBlock");
        return nullptr;
    }

    return pInstance;
}