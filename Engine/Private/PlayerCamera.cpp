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



    if (CGameInstance::Get().KeyDown(DIK_TAB))
    {
        m_bFix = !m_bFix;
        if (!m_bFix)
        {
            ShowCursor(TRUE);
        }
        else
        {
            ShowCursor(FALSE);
        }
    }
    if (m_bFix)
    {
        MouseFix();


        //if (CGameInstance::Get().KeyPressing(DIK_J))
        //{
        //    GetTransform().AddRotation(vUp, fTimeDelta * -90.f);
        //}
        //if (CGameInstance::Get().KeyPressing(DIK_L))
        //{
        //    GetTransform().AddRotation(vUp, fTimeDelta * 90.f);
        //}
        if (auto a = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::X))
        {
            _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
            GetTransform().AddRotation(vUp, fTimeDelta * 10.f * a);
        }


        //if (CGameInstance::Get().KeyPressing(DIK_I))
        //{
        //    GetTransform().AddRotation(vRight, fTimeDelta * -90.f);
        //}
        //if (CGameInstance::Get().KeyPressing(DIK_K))
        //{
        //    GetTransform().AddRotation(vRight, fTimeDelta * 90.f);
        //}



        //if (auto a = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::Y))
        //{
        //    _float3 look;
        //    XMStoreFloat3(&look, GetTransform().GetState(STATE::LOOK));

        //   
        //    // 위쪽 제한
        //    if (look.y > 0.59f && a > 0)
        //        return;

        //    // 아래쪽 제한
        //    if (look.y < -0.59f && a < 0)
        //        return;

        //    _vector vRight = GetTransform().GetState(STATE::RIGHT);
        //    GetTransform().AddRotation(vRight, fTimeDelta * 10.f * a);
        //    
        //}

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
    else
    {

    }

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
    if (auto colliders = E::CGameInstance::Get().GetColliders())
    {
        for (const auto& [key, value] : *colliders)
        {
            for (const auto& p : value)
            {
                if (m_pCollider.get() == p)
                {
                    continue;
                }

                if (m_pCollider->Intersect(*p))
                {
                    ++m_iColliderIntersect;
                }
            }
        }
    }
}

void CPlayerCamera::MouseFix() const
{
    RECT rect;
    GetClientRect(CGameInstance::Get().GetHwnd(), &rect);
    //POINT ul = { rect.left, rect.top };
    //POINT lr = { rect.right, rect.bottom };
    //ClientToScreen(CGameInstance::Get().GetHwnd(), &ul);
    //ClientToScreen(CGameInstance::Get().GetHwnd(), &lr);

    //RECT clipRect = { ul.x, ul.y, lr.x, lr.y };

    //ClipCursor(&clipRect); 

    POINT center;
    center.x = (rect.right - rect.left) / 2;
    center.y = (rect.bottom - rect.top) / 2;

    ClientToScreen(CGameInstance::Get().GetHwnd(), &center);
    SetCursorPos(center.x, center.y);

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
