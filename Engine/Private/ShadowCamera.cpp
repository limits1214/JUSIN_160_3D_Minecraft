#include "pch.h"

#include "ShadowCamera.h"
#include "GameInstance.h"
#include "CollFrustum.h"
#include "CollBox.h"
#include "CollOrientedBox.h"
NS_USING(Engine)

CShadowCamera::CShadowCamera()
{
}

CShadowCamera::CShadowCamera(const CShadowCamera& Prototype)
    : CCameraObject{ Prototype }
{
}

CShadowCamera::~CShadowCamera()
{
}

HRESULT CShadowCamera::Initialize(void* pArg)
{
    if (FAILED(CCameraObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    if (m_cameraDesc.eProj == PROJ::ORTHOGRAPHIC)
    {
         _float3 vCenter = { 0.f, 0.f, m_cameraDesc.fFar * 0.5f };
        _float3 vExtent = {
            m_cameraDesc.fWidth * 0.5f,
            m_cameraDesc.fHeight * 0.5f,
            m_cameraDesc.fFar * 0.5f
        };

        m_pCollider = CCollOrientedBox::Create(vCenter, vExtent, {0.f, 0.f, 0.f, 1.f});
    }
    else
    {
        m_pCollider = CCollFrustum::Create(XMLoadFloat4x4(&m_matProj));
    }
    m_pCollider->SetOriginalColor({ 0.f, 0.f, 1.f, 1.f });
    return S_OK;
}

void CShadowCamera::PriorityUpdate(E::_float fTimeDelta)
{

}

void CShadowCamera::Update(E::_float fTimeDelta)
{
    if(false)
    if (CGameInstance::Get().GetActiveGameCamera() == this )
    {
        if (CGameInstance::Get().KeyPressing(DIK_W))
        {
            GetTransform().GoStraight(fTimeDelta * 10.f);
        }

        if (CGameInstance::Get().KeyPressing(DIK_A))
        {
            GetTransform().GoLeft(fTimeDelta * 10.f);
        }

        if (CGameInstance::Get().KeyPressing(DIK_S))
        {
            GetTransform().GoBackward(fTimeDelta * 10.f);
        }

        if (CGameInstance::Get().KeyPressing(DIK_D))
        {
            GetTransform().GoRight(fTimeDelta * 10.f);
        }

        if (CGameInstance::Get().GetMouseFix())
        {
            if (auto a = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::X))
            {
                _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
                GetTransform().AddRotation(vUp, fTimeDelta * 10.f * a);
            }

            if (auto a = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::Y))
            {
                _float3 euler = GetTransform().GetRotationEuler();

                float delta = fTimeDelta * 10.f * a;
                float next = euler.x + delta;

                if (next <= 89.f && next >= -89.f)
                {
                    _vector vRight = GetTransform().GetState(STATE::RIGHT);
                    GetTransform().AddRotation(vRight, delta);
                }
            }
        }

    }

}

void CShadowCamera::LateUpdate(E::_float fTimeDelta)
{

    m_pComTransform->Update();

    E::CGameInstance::Get().AddColliderGroup("Coll_ShadowCamera", m_pCollider.get());

    m_pCollider->Transform(m_pComTransform->GetLoadedWorldMatrix());

    CCameraObject::UpdateViewMatrix();

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


Engine::UPtr<CShadowCamera> CShadowCamera::Create()
{
    auto pInstance = ToUPtr(new CShadowCamera{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CShadowCamera");
        return nullptr;
    }

    return pInstance;
}

Engine::UPtr<CPrototype> CShadowCamera::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CShadowCamera{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CShadowCamera");
        return nullptr;
    }

    return pInstance;
}
