#include "pch.h"

#include "PlayerCamera.h"
#include "GameInstance.h"
#include "CollFrustum.h"
NS_USING(Engine)

CPlayerCamera::CPlayerCamera()
{
}

CPlayerCamera::CPlayerCamera(const CPlayerCamera& Prototype)
    : CCameraObject{ Prototype }
{
}

CPlayerCamera::~CPlayerCamera()
{
}
void CPlayerCamera::UpdateGUI()
{
    CCameraObject::UpdateGUI();

    ImGui::Text("intersect: %i", m_iColliderIntersect);
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
    if (FAILED(CCameraObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    m_pCollider = CCollFrustum::Create(XMLoadFloat4x4(&m_matProj));

    return S_OK;
}

void CPlayerCamera::PriorityUpdate(E::_float fTimeDelta)
{
    

    m_pComTransform->Update();

    E::CGameInstance::Get().AddColliderGroup("Coll_FlyCamera", m_pCollider.get());

    m_pCollider->Transform(m_pComTransform->GetLoadedWorldMatrix());

    CCameraObject::UpdateViewMatrix();
}

void CPlayerCamera::Update(E::_float fTimeDelta)
{

}

void CPlayerCamera::LateUpdate(E::_float fTimeDelta)
{
    m_iColliderIntersect = 0;
    //if (auto colliders = E::CGameInstance::Get().GetColliders())
    //{
    //    for (const auto& [key, value] : *colliders)
    //    {
    //        for (const auto& p : value)
    //        {
    //            if (m_pCollider.get() == p)
    //            {
    //                continue;
    //            }

    //            if (m_pCollider->Intersect(*p))
    //            {
    //                ++m_iColliderIntersect;
    //            }
    //        }
    //    }
    //}
}


Engine::UPtr<CPlayerCamera> CPlayerCamera::Create()
{
    auto pInstance = ToUPtr(new CPlayerCamera{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPlayerCamera");
        return nullptr;
    }

    return pInstance;
}

Engine::UPtr<CPrototype> CPlayerCamera::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPlayerCamera{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPlayerCamera");
        return nullptr;
    }

    return pInstance;
}
