#include "pch.h"

#include "PlayerInvenUICamera.h"
#include "GameInstance.h"
#include "CollFrustum.h"
NS_USING(Engine)

CPlayerInvenUICamera::CPlayerInvenUICamera()
{
}

CPlayerInvenUICamera::CPlayerInvenUICamera(const CPlayerInvenUICamera& Prototype)
    : CCameraObject{ Prototype }
{
}

CPlayerInvenUICamera::~CPlayerInvenUICamera()
{
}
void CPlayerInvenUICamera::UpdateGUI()
{
    CCameraObject::UpdateGUI();
}

HRESULT CPlayerInvenUICamera::Initialize(void* pArg)
{
    if (FAILED(CCameraObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    m_pCollider = CCollFrustum::Create(XMLoadFloat4x4(&m_matProj));
    m_pCollider->SetOriginalColor({ 1.0f, 0.5f, 0.5f, 1.f });
    return S_OK;
}

void CPlayerInvenUICamera::PriorityUpdate(E::_float fTimeDelta)
{



}

void CPlayerInvenUICamera::Update(E::_float fTimeDelta)
{

}

void CPlayerInvenUICamera::LateUpdate(E::_float fTimeDelta)
{
    m_pComTransform->Update();

    E::CGameInstance::Get().AddColliderGroup("Coll_PlayerInvenUICamera", m_pCollider.get());

    m_pCollider->Transform(m_pComTransform->GetLoadedWorldMatrix());

    CCameraObject::UpdateViewMatrix();
}


Engine::UPtr<CPlayerInvenUICamera> CPlayerInvenUICamera::Create()
{
    auto pInstance = ToUPtr(new CPlayerInvenUICamera{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPlayerInvenUICamera");
        return nullptr;
    }

    return pInstance;
}

Engine::UPtr<CPrototype> CPlayerInvenUICamera::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPlayerInvenUICamera{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPlayerInvenUICamera");
        return nullptr;
    }

    return pInstance;
}
