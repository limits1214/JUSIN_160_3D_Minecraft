#include "HandHeldItemObject.h"
#include "GameInstance.h"
#include "Resources.h"
NS_USING(Engine)

CHandHeldItemObject::CHandHeldItemObject()
{
}

CHandHeldItemObject::~CHandHeldItemObject()
{
}

HRESULT CHandHeldItemObject::Initialize(void* pArg)
{
    auto pDesc = static_cast<DESC*>(pArg);
    m_viBufferID = pDesc->viBufferId;
    if (FAILED(CItemObject::Initialize(pArg)))
    {
        return E_FAIL;
    }


	return S_OK;
}

void CHandHeldItemObject::PriorityUpdate(E::_float fTimeDelta)
{
}

void CHandHeldItemObject::Update(E::_float fTimeDelta)
{
}

void CHandHeldItemObject::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CHandHeldItemObject::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
	return S_OK;
}

