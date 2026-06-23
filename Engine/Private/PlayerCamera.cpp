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

    if (ImGui::Button("Shake"))
    {
        TriggerCameraShake(1.f, 1.f);
    }
}

HRESULT CPlayerCamera::Initialize(void* pArg)
{
    if (FAILED(CCameraObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    m_pCollider = CCollFrustum::Create(XMLoadFloat4x4(&m_matProj));
    m_pCollider->SetOriginalColor({ 1.f, 0.f, 1.f, 1.f });
    return S_OK;
}

void CPlayerCamera::PriorityUpdate(E::_float fTimeDelta)
{
    

    
}

void CPlayerCamera::Update(E::_float fTimeDelta)
{
    if (m_fShakeTimer > 0.f)
    {
        m_fShakeTimer -= fTimeDelta;
        if (m_fShakeTimer < 0.f)
            m_fShakeTimer = 0.f;

        // 시간이 지날수록 1.0에서 0.0으로 줄어드는 감쇠 비율
        _float fDecay = m_fShakeTimer / m_fShakeDuration;

        // 현재 프레임의 실제 흔들림 강도
        _float fCurrentIntensity = m_fShakeIntensity * fDecay;

        // -1.0 ~ 1.0 사이의 난수 생성 후 강도 곱하기
        _float fOffsetX = ((rand() % 200) / 100.f - 1.f) * fCurrentIntensity;
        _float fOffsetY = ((rand() % 200) / 100.f - 1.f) * fCurrentIntensity;
        _float fOffsetZ = ((rand() % 200) / 100.f - 1.f) * fCurrentIntensity;

        auto vFinalPos = GetTransform().GetLoadedPostion();
        // 최종 위치에 흔들림 오프셋 추가
        vFinalPos += XMVectorSet(fOffsetX, fOffsetY, fOffsetZ, 0.f);
        GetTransform().SetPosition(vFinalPos);
    }
}

void CPlayerCamera::LateUpdate(E::_float fTimeDelta)
{
    m_pComTransform->Update();

    E::CGameInstance::Get().AddColliderGroup("Coll_PlayerCamera", m_pCollider.get());

    m_pCollider->Transform(m_pComTransform->GetLoadedWorldMatrix());

    CCameraObject::UpdateViewMatrix();
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
