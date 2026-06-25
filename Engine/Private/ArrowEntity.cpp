#include "ArrowEntity.h"
#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"
#include "CollBox.h"

#include "PigEntity.h"
#include "PlayerEntity.h"
#include "SkeletonEntity.h"
#include "CreeperEntity.h"
#include "ChickenEntity.h"
#include "ZombieEntity.h"
#include "CowEntity.h"

#include "ActivatedTNT.h"

NS_USING(Engine)

CArrowEntity::CArrowEntity()
{
}

CArrowEntity::CArrowEntity(const CArrowEntity& rhs)
    : CEntityObject{rhs}
{

}
CArrowEntity::~CArrowEntity()
{
}

HRESULT CArrowEntity::Initialize(void* pArg)
{
    //m_RenderPassFlags = ETOUI(RENDERPASS::DEFAULT) | ETOUI(RENDERPASS::PLAYER_INVEN_UI);

    if (FAILED(CEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Arrow"};
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Arrow" };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_EntityModel", "Com_EntityModel", &Desc, &m_pComEntityModel)))
        {
            return E_FAIL;
        };
    }

    {
        CComAnimator::DESC Desc{};
        Desc.pComEntityModel = m_pComEntityModel;
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_Animator", "Com_Animator", &Desc, &m_pComAnimator)))
        {
            return E_FAIL;
        };
    }

    {
        CComConstantBuffer::DESC Desc{};
        Desc.cBufferId = { TAG_RES_GRP_PERMANENT_BUFFER, TAG_RES_CBUFFER_OBJECT };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_ConstantBuffer", "ComCBufferPerObject", &Desc, &m_pComCBufferPerObject)))
        {
            return E_FAIL;
        };
    }

    m_pHeadCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.05f, 0.05f, 0.05f });
    m_pHeadCollider->SetInnerPointer(this);
    return S_OK;
}

void CArrowEntity::PriorityUpdate(E::_float fTimeDelta)
{
    m_fElapsed += fTimeDelta;
}

void CArrowEntity::Update(E::_float fTimeDelta)
{

    E::CGameInstance::Get().AddColliderGroup("Coll_ArrowHead", m_pHeadCollider.get());
    auto matWorld = GetTransform().GetWorldMatrix();

    auto asisScaleX = XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&matWorld->m[0][0])));
    auto asisScaleY = XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&matWorld->m[1][0])));
    auto asisScaleZ = XMVectorGetX(XMVector3Length(XMLoadFloat3((_float3*)&matWorld->m[2][0])));
    auto asisTranslation = XMLoadFloat3((_float3*)&matWorld->m[3][0]);

    _float4x4 todo{};
    XMStoreFloat4((_float4*)&todo.m[0][0], XMVectorSet(1.f, 0.f, 0.f, 0.f) * asisScaleX);
    XMStoreFloat4((_float4*)&todo.m[1][0], XMVectorSet(0.f, 1.f, 0.f, 0.f) * asisScaleY);
    XMStoreFloat4((_float4*)&todo.m[2][0], XMVectorSet(0.f, 0.f, 1.f, 0.f) * asisScaleZ);
    XMStoreFloat4((_float4*)&todo.m[3][0], XMVectorSetW(asisTranslation, 1.f));

    m_pHeadCollider->Transform(XMLoadFloat4x4(&todo));

}

void CArrowEntity::LateUpdate(E::_float fTimeDelta)
{
    ProcessArrowDamage(fTimeDelta);
    

    if (m_bArrowBomb)
    {
        if (m_bOnGround)
        {
            auto pos = GetTransform().GetPosition();
            CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
            SetPendingDestroyCascade();
        }
    }

    UpdateArrowLiftTime(fTimeDelta);
    UpdateArrowVelocity2(fTimeDelta);

    if (m_bRender)
    {
        m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
        CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    }

    GetTransform().Update();
}

HRESULT CArrowEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {

            E::CB_PER_OBJECT cbPerObject{};
            cbPerObject.matWorld = *GetTransform().GetWorldMatrix();

            // 복셀 라이팅 값 바인딩 로직
            auto pos = GetTransform().GetPosition();
            int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
            int32_t blockY = static_cast<int32_t>(std::floor(pos.y));
            int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
            if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
            {
                cbPerObject.light = optCurrBlock->GetLight();
            }

            XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

            memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
            pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
        }
        pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
    }

    m_pComEntityModel->BindBoneMatrix(pContext);

    m_pComEntityModel->Render(pContext, ctx);
    return S_OK;
}

void CArrowEntity::ProcessArrowDamage(_float fTimeDelta)
{
    uint32_t iDamage = 1;
    // Coll_PigCenter
    // Coll_CowCenter
    // Coll_ChickenCenter
    // Coll_ZombieCenter
    // Coll_CreeperCenter
    // Coll_SkeletonCenter

    //auto vPos = GetTransform().GetPosition();

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_PlayerCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {
                if (auto pPigEntity = Cast<CPlayerEntity>(pigColl->GetInnerPointer()))
                {
                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                //auto pos = GetTransform().GetPosition();
                                //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                //SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage);
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage);
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_PigCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CPigEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_ChickenCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CChickenEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_CowCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CCowEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_ZombieCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CZombieEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_CreeperCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CCreeperEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }

    if (auto pColls = CGameInstance::Get().GetColliderGroup("Coll_SkeletonCenter"))
    {
        for (auto& pigColl : *pColls)
        {
            if (CGameInstance::Get().IntersectColl(pigColl, m_pHeadCollider.get()))
            {

                if (auto pPigEntity = Cast<CSkeletonEntity>(pigColl->GetInnerPointer()))
                {

                    if (!m_bOnGround)
                    {
                        if (m_bArrowBomb)
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    //auto pos = GetTransform().GetPosition();
                                    //CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                    //SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                auto pos = GetTransform().GetPosition();
                                CActivatedTNT::ExplodeAndDamageColliding(pos, 5.f, m_iExplodeDamage, 15.f);
                                SetPendingDestroyCascade();
                            }
                        }
                        else
                        {
                            if (m_hSootBy == pPigEntity->GetHandle())
                            {
                                if (m_fElapsed > 1.f)
                                {
                                    pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                    SetPendingDestroyCascade();
                                }
                            }
                            else
                            {
                                pPigEntity->TakeDamage(iDamage, XMLoadFloat3(&m_vStartPos));
                                SetPendingDestroyCascade();
                            }
                        }
                    }
                }
            };
        }
    }
}
//
//void CArrowEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
//{
//    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);
//    //XMVECTOR vWishDir = XMVectorZero();
//
//    // 가속
//    float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
//    float fAddSpeed = m_fSpeed - fCurrSpeed;
//    if (fAddSpeed > 0.f)
//    {
//        float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
//        vVel += vWishDir * fAccelSpeed;
//    }
//
//    // 수평 속도 제한
//    XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
//    float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
//    if (fHorizSpeed > m_fSpeed)
//    {
//        vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
//        vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
//    }
//
//    // 마찰
//    if (m_bOnGround)
//    {
//        float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
//        if (fSpeed > 0.f)
//        {
//            // 의지 방향이 없거나(IDLE 등), 현재 속도가 가고 싶은 속도보다 빠르면 마찰 적용
//            if (XMVectorGetX(XMVector3Length(vWishDir)) < 0.01f || fSpeed > m_fSpeed)
//            {
//                float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
//                float vy = XMVectorGetY(vVel);
//                vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
//            }
//        }
//    }
//
//    // 중력
//    if (!m_bOnGround)
//        vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);
//
//    // AABB 충돌
//    const XMFLOAT3 halfExtents = { 0.49f, 0.49f, 0.49f };
//    XMFLOAT3 pos = GetTransform().GetPosition();
//
//    _float fAddY = 0.49f;
//    XMFLOAT3 c = { pos.x, pos.y + fAddY, pos.z };
//
//
//    // Y
//    float velY = XMVectorGetY(vVel);
//    float prevY = c.y;
//    c.y += velY * fTimeDelta;
//    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//    {
//        if (velY < 0.f)
//        {
//            c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
//            m_bOnGround = true;
//        }
//        else c.y = prevY;
//        vVel = XMVectorSetY(vVel, 0.f);
//    }
//    else m_bOnGround = false;
//
//    _bool bHitWall = false; // 이번 프레임에 벽에 막혔는가?
//
//    // [X 축 충돌 검사]
//    float px = c.x;
//    c.x += XMVectorGetX(vVel) * fTimeDelta;
//    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//    {
//        c.x = px;
//        vVel = XMVectorSetX(vVel, 0.f);
//
//        //  움직이려는 의지(vWishDir)가 있는데 X축이 막혔다면 벽에 박은 것!
//        if (fabsf(XMVectorGetX(vWishDir)) > 0.01f)
//            bHitWall = true;
//    }
//
//    // [Z 축 충돌 검사]
//    float pz = c.z;
//    c.z += XMVectorGetZ(vVel) * fTimeDelta;
//    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
//    {
//        c.z = pz;
//        vVel = XMVectorSetZ(vVel, 0.f);
//
//        //  움직이려는 의지(vWishDir)가 있는데 Z축이 막혔다면 벽에 박은 것!
//        if (fabsf(XMVectorGetZ(vWishDir)) > 0.01f)
//            bHitWall = true;
//    }
//
//
//    if (m_bOnGround && bHitWall)
//    {
//        // 마인크래프트 고증 점프 속도 (상황에 따라 5.0f ~ 6.0f 사이 조절)
//        vVel = XMVectorSetY(vVel, 6.5f);
//        m_bOnGround = false; // 공중에 떴으므로 상태 변경
//    }
//
//    // 최종 좌표 적용 및 속도 백업
//    GetTransform().SetPosition(_float3{ c.x, c.y - fAddY, c.z });
//    XMStoreFloat3(&m_vVelocity, vVel);
//}

void CArrowEntity::UpdateArrowLiftTime(_float fTimeDelta)
{
    m_fArrowLifeTime -= fTimeDelta;
    if (m_fArrowLifeTime < 0)
    {
        SetPendingDestroyCascade();
    }
}

//void CArrowEntity::UpdateArrowVelocity(_float fTimeDelta)
//{
//    // 1. 중력 적용 (Gravity) - y축 속도를 지속적으로 감소
//    m_vVelocity.y -= m_fGravity * fTimeDelta;
//
//    // 2. 공기 저항 적용 (Drag) - 마인크래프트 화살은 날아갈수록 속도가 약간씩 줍니다.
//    // pow를 사용하여 프레임 레이트와 무관하게 저항을 적용합니다.
//    m_vVelocity.x *= pow(m_fDrag, fTimeDelta * 60.0f);
//    m_vVelocity.z *= pow(m_fDrag, fTimeDelta * 60.0f);
//
//    DirectX::XMVECTOR vVelocity = DirectX::XMLoadFloat3(&m_vVelocity);
//
//    // 3. 위치 업데이트 (Position = Position + Velocity * TimeDelta)
//    DirectX::XMVECTOR vPos = GetTransform().GetLoadedPostion(); // 엔진의 위치 겟 함수
//    vPos += vVelocity * fTimeDelta;
//    GetTransform().SetPosition(vPos);
//
//    // 4. 회전 업데이트 (화살촉(+Z)이 진행 방향을 바라보게)
//    // Left-Handed 좌표계 기준 Yaw와 Pitch 계산
//    float fHorizontalDist = sqrtf(m_vVelocity.x * m_vVelocity.x + m_vVelocity.z * m_vVelocity.z);
//
//    // 좌우 회전 (Yaw - Y축 회전)
//    float fYaw = atan2f(m_vVelocity.x, m_vVelocity.z);
//
//    // 상하 회전 (Pitch - X축 회전)
//    // 떨어질 때(-y) 화살촉이 아래를 향하려면 X축 기준 양수 회전이 필요합니다.
//    float fPitch = atan2f(-m_vVelocity.y, fHorizontalDist);
//
//    GetTransform().SetQuaternion(XMQuaternionRotationRollPitchYaw(fPitch, fYaw, 0.f));
//}

void CArrowEntity::UpdateArrowVelocity2(_float fTimeDelta)
{
    _bool bBeforeOnGround = m_bOnGround;
    //if (m_bOnGround)
    //{
    //    auto pos = GetTransform().GetPosition();
    //    //int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
    //    //int32_t blockY = static_cast<int32_t>(std::floor(pos.y));
    //    //int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
    //    //if (auto block = CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
    //    //{
    //    //    //block->
    //    //}
    //    const XMFLOAT3 halfExtents = { 0.05f, 0.05f, 0.05f };
    //    if (CGameInstance::Get().VoxelAABBOverlap(pos, halfExtents))
    //    {
    //        return;
    //    }
    //    else
    //    {
    //        m_bOnGround = false;    
    //    }


    //    
    //}
    


    //if (m_bOnGround) return;

    //
    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);

    if (!m_bOnGround)
    {
        // 1. 중력 및 공기 저항 적용
    // 화살은 속도가 빠르므로 중력을 먼저 적용하여 궤적을 형성합니다.
        vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - m_fGravity * fTimeDelta);

        // 저항 적용 (pow 방식 유지)
        float fDragFactor = pow(m_fDrag, fTimeDelta * 60.0f);
        vVel *= XMVectorSet(fDragFactor, 1.f, fDragFactor, 1.f); // Y는 저항 제외(중력만)

      

    }
    // 2. 위치 및 AABB 충돌 검사 (기존 플레이어 로직 활용)
    const XMFLOAT3 halfExtents = { 0.05f, 0.05f, 0.05f }; // 화살은 작으므로 히트박스 축소
    XMFLOAT3 pos = GetTransform().GetPosition();
    XMFLOAT3 c = { pos.x, pos.y, pos.z };
    // 이동 예측 위치 계산
    XMVECTOR vNextPos = XMLoadFloat3(&c) + vVel * fTimeDelta;
    XMFLOAT3 nextPos;
    XMStoreFloat3(&nextPos, vNextPos);
    // 충돌 시 화살은 멈추거나 박혀야 함
    if (CGameInstance::Get().VoxelAABBOverlap(nextPos, halfExtents))
    {
        


        // 벽/바닥 충돌 시 화살 속도 초기화 (박힌 상태)
        //vVel *= fTimeDelta;
        m_bOnGround = true; // 박혔음을 지면 상태로 간주


    }
    else
    {
        if (m_bOnGround)
        {
            vVel *= 0.01f;
        }
        c = nextPos;
        GetTransform().SetPosition(c);
        m_bOnGround = false;
    }

    // 3. 회전 업데이트 (진행 방향 바라보기)
    // 속도가 너무 느리면 회전 업데이트를 멈추어 덜덜거림 방지
    float fSpeedSq = XMVectorGetX(XMVector3LengthSq(vVel));
   if (!m_bOnGround && fSpeedSq > 0.01f)
    {
        float fHorizontalDist = sqrtf(m_vVelocity.x * m_vVelocity.x + m_vVelocity.z * m_vVelocity.z);
        float fYaw = atan2f(m_vVelocity.x, m_vVelocity.z);
        float fPitch = atan2f(-m_vVelocity.y, fHorizontalDist);

        GetTransform().SetQuaternion(XMQuaternionRotationRollPitchYaw(fPitch, fYaw, 0.f));
    }

    XMStoreFloat3(&m_vVelocity, vVel);


    if (!bBeforeOnGround && m_bOnGround)
    {
        if (auto pCam = CGameInstance::Get().GetActiveGameCamera())
        {
            _vector vCurrPos = GetTransform().GetLoadedPostion();
            _vector vDistVec = vCurrPos - pCam->GetTransform().GetLoadedPostion();
            float fDistSq = XMVectorGetX(XMVector3LengthSq(vDistVec));
            constexpr float MaxDistance = 32.f;
            constexpr float MaxDistanceSq = MaxDistance * MaxDistance;
            float ratioSq = std::clamp(fDistSq / MaxDistanceSq, 0.f, 1.f);
            float fMaxVol = 0.1f;
            float fVol = (1.f - ratioSq) * fMaxVol;

            // 랜덤 재생
            const char* explodeSounds[] = { "BOW_HIT_1", "BOW_HIT_2", "BOW_HIT_3", "BOW_HIT_4" };
            CGameInstance::Get().SoundPlay(explodeSounds[RandInt(0, 3)], fVol);
        }
    }
}

void CArrowEntity::Shoot(CHandle hShootBy, const DirectX::XMVECTOR& vStartPos, const DirectX::XMVECTOR& vDirection, E::_float fSpeed, _bool bArrowBomb)
{
    m_hSootBy = hShootBy;

    auto safeStartPos = vStartPos + XMVector3Normalize(vDirection) * 0.3f;
    GetTransform().SetPosition(safeStartPos);
    XMStoreFloat3(&m_vStartPos, safeStartPos);

    DirectX::XMVECTOR vVelocity = DirectX::XMVector3Normalize(vDirection) * fSpeed;
    DirectX::XMStoreFloat3(&m_vVelocity, vVelocity);
    m_fSpeed = fSpeed;
    m_bArrowBomb = bArrowBomb;

    if (auto pCam = CGameInstance::Get().GetActiveGameCamera())
    {
        _vector vCamPos = XMLoadFloat3(&pCam->GetTransform().GetPosition());
        _vector vCurrPos = GetTransform().GetLoadedPostion();

        // 거리 계산
        float fDist = XMVectorGetX(XMVector3Length(vCurrPos - vCamPos));
        constexpr float MaxDistance = 32.f * 1.f; // 32 * 5

        // 볼륨 감쇄 (0.0 ~ 1.0)
        float ratio = std::clamp(fDist / MaxDistance, 0.f, 1.f);
        //float fVol = (1.f - (ratio * ratio)) * 0.5f;

        float fMaxVol = 0.1f;
        float fVol = (1.f - (ratio * ratio)) * fMaxVol;

        CGameInstance::Get().SoundPlay("BOW", fVol);
    }
}

UPtr<CArrowEntity> CArrowEntity::Create()
{
    auto pInstance = ToUPtr(new CArrowEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CArrowEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CArrowEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CArrowEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CArrowEntity");
        return nullptr;
    }

    return pInstance;
}

