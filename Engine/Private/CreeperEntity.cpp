#include "CreeperEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"

#include "ComConstantBuffer.h"

#include "CollBox.h"

#include "ExperienceOrb.h"
#include "ItemObject.h"

#include "ActivatedTNT.h"
NS_USING(Engine)


CCreeperEntity::CCreeperEntity()
{
}

CCreeperEntity::CCreeperEntity(const CCreeperEntity& rhs)
    : CMonsterEntityObject{rhs}
{

}

CCreeperEntity::~CCreeperEntity()
{
}

HRESULT CCreeperEntity::Initialize(void* pArg)
{
    if (FAILED(CMonsterEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Creeper" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Creeper" };
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

    m_pCenterCollider = CCollBox::Create({ 0.f, 1.f, 0.f }, { 0.25f, 0.9f, 0.25f });
    m_pCenterCollider->SetInnerPointer(this);

    return S_OK;
}

void CCreeperEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CCreeperEntity::Update(E::_float fTimeDelta)
{
    m_fStateTimer -= fTimeDelta;
    XMVECTOR vWishDir = XMVectorZero();
    auto pPlayer = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer);

    _float fPlayerDetectDist = 10.f;

    auto playerpos = pPlayer->GetTransform().GetPosition();
    playerpos.y += 1.5f;

    auto creeperpos = GetTransform().GetPosition();
    creeperpos.y += 1.325f;
    
    XMVECTOR vCreeperPos = XMLoadFloat3(&creeperpos);
    XMVECTOR vPlayerPos = pPlayer ? XMLoadFloat3(&playerpos) : XMVectorZero();

    auto vDir = XMVector3Normalize(vPlayerPos - vCreeperPos);

    auto pLeg0 = m_pComEntityModel->GetBone("leg0"); // 뒤쪽 오른쪽 다리
    auto pLeg1 = m_pComEntityModel->GetBone("leg1"); // 뒤쪽 왼쪽 다리
    auto pLeg2 = m_pComEntityModel->GetBone("leg2"); // 앞쪽 오른쪽 다리
    auto pLeg3 = m_pComEntityModel->GetBone("leg3"); // 앞쪽 왼쪽 다리
    auto pHead = m_pComEntityModel->GetBone("head");
    auto pBody = m_pComEntityModel->GetBone("body");

    

    //m_fSwellingProgress;

    if (m_bIsHit)
    {
        m_fHitTimer -= fTimeDelta;
        if (m_fHitTimer <= 0.f)
        {
            m_bIsHit = false;
            m_fHitTimer = 0.f;
        }

        if (m_iHeart <= 0)
        {
            //m_eCurrentState = PIG_STATE::DIE;
            //return;
        }
    }
    

    _float fDistToPlayer = pPlayer ? XMVectorGetX(XMVector3Length(vPlayerPos - vCreeperPos))  : 999.f;
    switch (m_eCurrentState)
    {
    case CREEPER_STATE::IDLE:

        if (m_bIsHit)
        {
            break;
        }

        m_fSpeed = 0.f; // 정지

        m_fWalkWeight += (0.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
        if (m_fWalkWeight < 0.001f) {
            m_fWalkWeight = 0.0f;
            m_fWalkTime = 0.0f;
        }

        // -------------------------------------------------------------
        // [IDLE 전용 시선 처리] 플레이어가 없을 때 무작위로 고개 돌리기
        // -------------------------------------------------------------
        if (fDistToPlayer >= fPlayerDetectDist)
        {
            m_fIdleLookTimer -= fTimeDelta;
            if (m_fIdleLookTimer <= 0.f)
            {
                // 1~3초마다 한 번씩 고개를 돌림
                m_fIdleLookTimer = 1.0f + (rand() % 20) * 0.1f;

                // 현재 몸통 방향(m_fRootRotRadY)을 기준으로 좌우 35도 이내의 무작위 각도를 타겟으로 설정
                float fRandomOffset = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(45.f);
                m_fTargetYaw = m_fRootRotRadY + fRandomOffset;

                // 가끔 위아래도 살짝 쳐다보게 설정 (-10도 ~ 10도)
                m_fTargetPitch = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(20.f);
            }
        }
        else // 플레이어가 4블록 이내로 접근하면 쳐다봄 (CHASE 전환)
        {
            m_eCurrentState = CREEPER_STATE::CHASE_PLAYER;
        }

        // 타이머 끝나면 무작위로 걷기 시작
        if (m_fStateTimer <= 0.f) {
            m_eCurrentState = CREEPER_STATE::WANDER;
            m_fStateTimer = 2.f + (rand() % 40) * 0.1f;
            m_fTargetYaw = (rand() % 360) * (XM_PI / 180.f); // 걸어갈 새로운 무작위 방향
            m_fTargetPitch = 0.f; // 걸을 때는 정면 응시
        }

        m_fSwellingProgress -= fTimeDelta;
        if (m_fSwellingProgress < 0.0f) m_fSwellingProgress = 0.0f;
        break;
    case CREEPER_STATE::WANDER:

        if (m_bIsHit)
        {
            break;
        }

        m_fSpeed = 1.5f; // 평화롭게 걷는 속도

        m_fWalkTime += fTimeDelta * 7.0f;
        m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;

        // 가고 싶은 방향 벡터 추출 (TargetYaw 기준 정면)
        vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
        m_pComAnimator->QuadrupedWalk(fTimeDelta * 0.1f); // 걷기 애니메이션

        if (fDistToPlayer < fPlayerDetectDist) {
            m_eCurrentState = CREEPER_STATE::CHASE_PLAYER;
        }
        else if (m_fStateTimer <= 0.f) {
            m_eCurrentState = CREEPER_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기
        }

        m_fSwellingProgress -= fTimeDelta;
        if (m_fSwellingProgress < 0.0f) m_fSwellingProgress = 0.0f;
        break;
    case CREEPER_STATE::CHASE_PLAYER:

        if (m_bIsHit)
        {
            break;
        }

        m_fSpeed = 3.5f;

        m_fWalkTime += fTimeDelta * 7.0f;
        m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;

        if (fDistToPlayer >= fPlayerDetectDist) {
            m_eCurrentState = CREEPER_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기

            //m_fIgniteTimer = 0.f;
            m_fColorWhiteTimer = 0.f;
            m_bColorWhite = false;

            m_fSwellingProgress -= fTimeDelta;
            if (m_fSwellingProgress < 0.0f) m_fSwellingProgress = 0.0f;
        }
        else
        {
            {
                XMVECTOR vToPlayer = XMVector3Normalize(vPlayerPos - vCreeperPos);
                m_fTargetYaw = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer));
            }

            {
                XMVECTOR vToPlayer = XMVector3Normalize(vPlayerPos - vCreeperPos);
                //m_fTargetPitch = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer)); 
                
                m_fTargetPitch = -atan2f(XMVectorGetY(vToPlayer), sqrtf(XMVectorGetX(vToPlayer)* XMVectorGetX(vToPlayer) + XMVectorGetZ(vToPlayer)* XMVectorGetZ(vToPlayer)));
            }

            

            //XMVector3Normalize(pPlayer->GetTransform().GetState(STATE::POSITION) - GetTransform().GetState(STATE::POSITION));
            //XMVECTOR dir = XMVector3Normalize(
            //    pPlayer->GetTransform().GetState(STATE::POSITION)
            //    - GetTransform().GetState(STATE::POSITION)
            //);

            //XMFLOAT3 vDir;
            //XMStoreFloat3(&vDir, dir);

            //float yaw = atan2f(vDir.x, vDir.z);
            //m_fTargetYaw = yaw;

            vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);

            if (fDistToPlayer < 3.f)
            {
                if (m_fSwellingProgress == 0.f)
                {
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

                        CGameInstance::Get().SoundPlay("FUSE", fVol);
                    }
                }
                m_fSwellingProgress += fTimeDelta * 0.5f;
                if (m_fSwellingProgress > 1.0f) m_fSwellingProgress = 1.0f;
                
                //m_fIgniteTimer += fTimeDelta;

                m_fColorWhiteTimer += fTimeDelta;
                if (m_fColorWhiteTimer > 0.1f)
                {
                    m_fColorWhiteTimer = 0.f;
                    m_bColorWhite = !m_bColorWhite;
                }

                if (m_fSwellingProgress >= 1.f)
                {
                    ExplodeUpdate(fTimeDelta);
                }
                //if (m_fIgniteTimer > 2.f)
                //{
                //    ExplodeUpdate(fTimeDelta);
                //}
            }
            else
            {
                m_fSwellingProgress -= fTimeDelta;
                if (m_fSwellingProgress < 0.0f) m_fSwellingProgress = 0.0f;
               

                //m_fIgniteTimer = 0.f;
                m_fColorWhiteTimer = 0.f;
                m_bColorWhite = false;
            }
        }
        
        break;
    case CREEPER_STATE::DIE:
        m_fDeathTimer += fTimeDelta;

        m_pComEntityModel->ResetBonesChannel();
        m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

        constexpr float FALL_DURATION = 0.5f;
        constexpr float TOTAL_DURATION = 1.5f;

        float fFallT = std::min(m_fDeathTimer / FALL_DURATION, 1.0f);
        float fEased = 1.f - (1.f - fFallT) * (1.f - fFallT);
        float fRoll = XMConvertToRadians(90.f) * fEased;

        // 1. Yaw 쿼터니언 (바라보는 방향 고정)
        _vector qYaw = XMQuaternionRotationAxis(
            XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRootRotRadY);

        // 2. Yaw 적용 후 실제 Look 방향(월드 Z를 Yaw로 회전)
        _vector vLook = XMVector3Rotate(
            XMVectorSet(0.f, 0.f, 1.f, 0.f), qYaw);

        // 3. 그 Look축을 기준으로 Roll
        _vector qRoll = XMQuaternionRotationAxis(vLook, fRoll);

        // 4. Roll * Yaw 순서로 결합 (Roll이 월드 공간에서 먼저)
        _vector qFinal = XMQuaternionMultiply(qYaw, qRoll);

        GetTransform().SetQuaternion(qFinal);

        VelocityUpdate(fTimeDelta, XMVectorZero());

        E::CGameInstance::Get().AddColliderGroup("Coll_PigCenter", m_pCenterCollider.get());
        m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

        if (m_fDeathTimer >= TOTAL_DURATION)
            ProcessDestroy(fTimeDelta);
        return;
    }

    m_pComEntityModel->ResetBonesChannel();

    //if(false)
    //{
    //    // 몸통(Root) 회전 보간 처리 (탁탁 끊기지 않고 부드럽게 싹 돌게 만듬)
    //    // 현재 각도에서 목표 각도로 매 프레임 조금씩 Lerp 회전
    //    float fAngleDiff = m_fTargetYaw - m_fRootRotRadY;
    //    // 고각 오차 보정 (-PI ~ PI 사이로 각도 범위 래핑)
    //    while (fAngleDiff < -XM_PI) fAngleDiff += XM_2PI;
    //    while (fAngleDiff > XM_PI) fAngleDiff -= XM_2PI;
    //    m_fRootRotRadY += fAngleDiff * 8.f * fTimeDelta;

    //    auto pRoot = m_pComEntityModel->GetBone("root");
    //    pRoot->GetRotation()->y = m_fRootRotRadY;
    //}

    //{
    //    float toRotYRadDiff = m_fTargetYaw - m_fRootRotRadY;

    //    while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
    //    while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

    //    float sign = toRotYRadDiff > 0 ? 1.f : -1.f;

    //    float threshold = XMConvertToRadians(45.f);

    //    if (fabsf(toRotYRadDiff) > threshold)
    //    {
    //        float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
    //        m_fRootRotRadY += delta;
    //        pHead->GetRotation()->y += (threshold * sign);
    //    }
    //    else
    //    {
    //        pHead->GetRotation()->y += toRotYRadDiff;
    //    }

    //    pBody->GetRotation()->y += m_fRootRotRadY;



    //    {
    //        float pitchDiff = m_fTargetPitch - m_fHeadRotRadX;
    //        float delta = pitchDiff * 5.f * fTimeDelta;
    //        m_fHeadRotRadX += delta;
    //        pHead->GetRotation()->x += m_fHeadRotRadX;
    //    }
    //}

    //{
    //    // 1. 현재 타겟 방향과 몸통 방향의 차이 계산
    //    float toRotYRadDiff = m_fTargetYaw - m_fRootRotRadY;

    //    while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
    //    while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

    //    float sign = toRotYRadDiff > 0 ? 1.f : -1.f;
    //    float threshold = XMConvertToRadians(45.f);

    //    // 머리가 향해야 할 '목표 로컬 Y 각도'
    //    float fTargetHeadY = 0.f;

    //    if (fabsf(toRotYRadDiff) > threshold)
    //    {
    //        // 차이가 45도를 넘어가면, 몸통이 쫓아감
    //        float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
    //        m_fRootRotRadY += delta;

    //        // 머리의 목표 로컬 각도는 최대치(45도)로 고정
    //        fTargetHeadY = threshold * sign;
    //    }
    //    else
    //    {
    //        // 차이가 45도 이내면, 몸통은 멈추고 머리만 타겟을 향함
    //        fTargetHeadY = toRotYRadDiff;
    //    }

    //    // 2. Head Y 보간 처리 (목표 각도를 향해 부드럽게 회전)
    //    // 10.f는 머리 회전 속도입니다. 필요에 따라 조절하세요.
    //    float headYDiff = fTargetHeadY - m_fHeadRotRadY;
    //    m_fHeadRotRadY += headYDiff * 10.f * fTimeDelta;

    //    // 3. Pitch (상하 회전) 보간 처리 - 기존 로직 유지
    //    float pitchDiff = m_fTargetPitch - m_fHeadRotRadX;
    //    m_fHeadRotRadX += pitchDiff * 5.f * fTimeDelta;

    //    // 4. 최종 계산된 누적 변수들을 Bone에 절대값으로 대입 (= 사용)
    //    if (pHead)
    //    {
    //        pHead->GetRotation()->y = m_fHeadRotRadY;
    //        pHead->GetRotation()->x = m_fHeadRotRadX;
    //    }
    //    if (pBody)
    //    {
    //        pBody->GetRotation()->y = m_fRootRotRadY;
    //    }
    //}

    {
        // 1. 현재 이동(전진) 중인지 여부 체크
        // WANDER 상태이거나 CHASE_PLAYER 상태일 때를 이동 중으로 판별합니다.
        bool bIsMoving = (m_eCurrentState == CREEPER_STATE::WANDER || m_eCurrentState == CREEPER_STATE::CHASE_PLAYER);

        // 타겟 방향과 몸통 방향의 차이 계산
        float toRotYRadDiff = m_fTargetYaw - m_fRootRotRadY;

        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

        float fTargetHeadY = 0.f;

        if (bIsMoving)
        {
            // -------------------------------------------------------------
            // [규칙 A] 전진(이동) 중일 때: 몸통과 머리를 동일선상으로 정렬
            // -------------------------------------------------------------
            // 몸통(Root)이 타겟 방향(이동 방향)을 향해 딜레이 없이 직접 회전합니다.
            // (이동 중일 때는 회전 속도를 8.f~10.f 정도로 높여주면 더 빠릿하게 돕니다)
            m_fRootRotRadY += toRotYRadDiff * 8.f * fTimeDelta;

            // 머리의 목표 로컬 Y 각도는 0 즉, 몸통과 똑같은 정면(동일선상)을 바라봅니다.
            fTargetHeadY = 0.f;
        }
        else
        {
            // -------------------------------------------------------------
            // [규칙 B] 제자리에 서 있을 때 (IDLE): 기존의 45도 시선 분리 로직
            // -------------------------------------------------------------
            float sign = toRotYRadDiff > 0 ? 1.f : -1.f;
            float threshold = XMConvertToRadians(45.f);

            if (fabsf(toRotYRadDiff) > threshold)
            {
                float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
                m_fRootRotRadY += delta;
                fTargetHeadY = threshold * sign;
            }
            else
            {
                fTargetHeadY = toRotYRadDiff;
            }
        }

        // 2. Head Y 보간 처리 (목표 각도를 향해 부드럽게 회전)
        float headYDiff = fTargetHeadY - m_fHeadRotRadY;
        m_fHeadRotRadY += headYDiff * 10.f * fTimeDelta;

        // 3. Pitch (상하 회전) 보간 처리 - 기존 유지
        float pitchDiff = m_fTargetPitch - m_fHeadRotRadX;
        m_fHeadRotRadX += pitchDiff * 5.f * fTimeDelta;

        // 4. 최종 계산된 누적 변수들을 Bone에 절대값으로 대입
        if (pHead)
        {
            pHead->GetRotation()->y = m_fHeadRotRadY;
            pHead->GetRotation()->x = m_fHeadRotRadX;
        }
        if (pBody)
        {
            pBody->GetRotation()->y = m_fRootRotRadY;
        }
    }

    {
        float fMaxLegAngle = 35.0f; // 최대 앞뒤 회전 각도 (도 단위)
        float fLegRot = std::sin(m_fWalkTime) * fMaxLegAngle * m_fWalkWeight;

        float legRot0 = fLegRot;
        float legRot1 = -fLegRot;
        float legRot2 = -fLegRot;
        float legRot3 = fLegRot;

        if (pLeg0) pLeg0->SetRotation({ XMConvertToRadians(legRot0), 0.f, 0.f });
        if (pLeg1) pLeg1->SetRotation({ XMConvertToRadians(legRot1), 0.f, 0.f });
        if (pLeg2) pLeg2->SetRotation({ XMConvertToRadians(legRot2), 0.f, 0.f });
        if (pLeg3) pLeg3->SetRotation({ XMConvertToRadians(legRot3), 0.f, 0.f });
    }

    
    

   
    float s_fSwellingScale1 = 1.0f + (m_fSwellingProgress * 0.35f); // X, Z축 (가로 늘리기)
    float s_fSwellingScale2 = 1.0f + (m_fSwellingProgress * 0.15f); // Y축 (세로 늘리기)
    XMFLOAT3 vSwellingScale = { s_fSwellingScale1, s_fSwellingScale2, s_fSwellingScale1 };

    if (pHead) pHead->SetScale(vSwellingScale);
    if (pBody) pBody->SetScale(vSwellingScale);
    if (pLeg0) pLeg0->SetScale(vSwellingScale);
    if (pLeg1) pLeg1->SetScale(vSwellingScale);
    if (pLeg2) pLeg2->SetScale(vSwellingScale);
    if (pLeg3) pLeg3->SetScale(vSwellingScale);
    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

    VelocityUpdate(fTimeDelta, vWishDir);

    E::CGameInstance::Get().AddColliderGroup("Coll_CreeperCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());
}

//if (false)
//{
//    // 1. 임시 스태틱 변수 선언 (테스트용)
//    static float s_fWalkTime = 0.0f;          // 걷기 애니메이션 시간 누적 변수
//    static float s_fSwellingProgress = 0.0f;  // 폭발 카운트다운 진행도 (0.0: 평소 ~ 1.0: 폭발직전)
//
//    // [테스트] 이동 중이라면 걷기 시간 누적 (멈추면 0으로 서서히 보간 가능)
//    s_fWalkTime += fTimeDelta * 7.0f; // 곱하는 값이 걸음 속도
//
//    // [테스트] 크리퍼가 플레이어에게 접근해 점화(우클릭 등) 상태일 때 부풀어 오름
//    if (CGameInstance::Get().KeyPressing(DIK_SPACE)) // 폭발 준비 상태 플래그 (가정)
//    {
//        s_fSwellingProgress += fTimeDelta * 1.3f; // 약 1.5초 동안 부풀어 오름
//        if (s_fSwellingProgress > 1.0f) s_fSwellingProgress = 1.0f;
//    }
//    else
//    {
//        s_fSwellingProgress -= fTimeDelta * 2.0f; // 멀어지면 진정됨
//        if (s_fSwellingProgress < 0.0f) s_fSwellingProgress = 0.0f;
//    }
//
//
//    // 2. MoLang 변수 시뮬레이션 계산
//    // variable.leg_rot: 보통 마인크래프트에서 다리 흔들림의 기준 각도입니다.
//    float fMaxLegAngle = 35.0f; // 최대 앞뒤 회전 각도 (도 단위)
//    float s_fLegRot = std::sin(s_fWalkTime) * fMaxLegAngle;
//
//    // variable.swelling_scale1, 2: 폭발 직전 뚱뚱해지는 스케일 공식 (원작 근사치)
//    // 평소(0.0)일 때는 1.0배 정사이즈, 최대(1.0)일 때는 가로로 1.35배, 세로로 1.15배 부풀어 오름
//    float s_fSwellingScale1 = 1.0f + (s_fSwellingProgress * 0.35f); // X, Z축 (가로 늘리기)
//    float s_fSwellingScale2 = 1.0f + (s_fSwellingProgress * 0.15f); // Y축 (세로 늘리기)
//
//
//    // 3. --- [애니메이션 1] 다리 걷기 회전 (animation.creeper.legs) ---
//    // 크리퍼의 4개 다리는 엇갈리게 움직입니다. (leg0, leg3이 같이 움직이고, leg1, leg2가 같이 움직임)
//    float legRot0 = s_fLegRot;
//    float legRot1 = -s_fLegRot;
//    float legRot2 = -s_fLegRot;
//    float legRot3 = s_fLegRot;
//
//    auto pLeg0 = m_pComEntityModel->GetBone("leg0"); // 뒤쪽 오른쪽 다리
//    auto pLeg1 = m_pComEntityModel->GetBone("leg1"); // 뒤쪽 왼쪽 다리
//    auto pLeg2 = m_pComEntityModel->GetBone("leg2"); // 앞쪽 오른쪽 다리
//    auto pLeg3 = m_pComEntityModel->GetBone("leg3"); // 앞쪽 왼쪽 다리
//
//    if (pLeg0) pLeg0->SetRotation({ XMConvertToRadians(legRot0), 0.f, 0.f });
//    if (pLeg1) pLeg1->SetRotation({ XMConvertToRadians(legRot1), 0.f, 0.f });
//    if (pLeg2) pLeg2->SetRotation({ XMConvertToRadians(legRot2), 0.f, 0.f });
//    if (pLeg3) pLeg3->SetRotation({ XMConvertToRadians(legRot3), 0.f, 0.f });
//
//
//    // 4. --- [애니메이션 2] 폭발 전 부풀어 오르기 스케일 (animation.creeper.swelling) ---
//    // 모든 부위(머리, 몸통, 다리 4개)에 공통 스케일 [Scale1, Scale2, Scale1]을 적용합니다.
//    XMFLOAT3 vSwellingScale = { s_fSwellingScale1, s_fSwellingScale2, s_fSwellingScale1 };
//
//    auto pHead = m_pComEntityModel->GetBone("head");
//    auto pBody = m_pComEntityModel->GetBone("body");
//
//    if (pHead) pHead->SetScale(vSwellingScale);
//    if (pBody) pBody->SetScale(vSwellingScale);
//    if (pLeg0) pLeg0->SetScale(vSwellingScale);
//    if (pLeg1) pLeg1->SetScale(vSwellingScale);
//    if (pLeg2) pLeg2->SetScale(vSwellingScale);
//    if (pLeg3) pLeg3->SetScale(vSwellingScale);
//}

void CCreeperEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CCreeperEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        E::CB_PER_OBJECT cbPerObject{};
        cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

        //  피격 플래그 컬러필터 조립
        //if (m_eCurrentState == PIG_STATE::DIE)
        //{
        //    _float fProgress = m_fDeathTimer / 1.0f; // 0.0f ~ 1.0f

        //    // 시간이 흐를수록 완전히 시뻘개지도록 RGB 제어 (알파 채널을 깎으면 시각적 디스폰 효과)
        //    cbPerObject.vBaseColor = _float4(1.0f, 0.05f, 0.05f, 1.0f - fProgress);
        //}
        //else if (m_bIsHit)
        //{
        //    cbPerObject.vBaseColor = _float4(1.f, 0.2f, 0.2f, 1.f); // 피격 깜빡임
        //}
        //else
        //{
        //    cbPerObject.vBaseColor = _float4(1.f, 1.f, 1.f, 1.f);
        //}

        if (m_eCurrentState == CREEPER_STATE::DIE)
        {
            _float fProgress = m_fDeathTimer / 1.0f; // 0.0f ~ 1.0f

            // 시간이 흐를수록 완전히 시뻘개지도록 RGB 제어 (알파 채널을 깎으면 시각적 디스폰 효과)
            cbPerObject.vBaseColor = _float4(1.0f, 0.05f, 0.05f, 1.0f - fProgress);
        }
        else if (m_bIsHit)
        {
            cbPerObject.vBaseColor = _float4(1.f, 0.2f, 0.2f, 1.f); // 피격 깜빡임
        }
        else if (m_bColorWhite)
        {
            cbPerObject.vBaseColor = _float4(2.f, 2.f, 2.f, 2.f);
        }
        else
        {
            cbPerObject.vBaseColor = _float4(1.f, 1.f, 1.f, 1.f);
        }


        // 복셀 라이팅 값 바인딩 로직
        auto pos = GetTransform().GetPosition();
        int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
        int32_t blockY = static_cast<int32_t>(std::floor(pos.y));
        int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
        blockY += 1.8f;
        if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
        {
            cbPerObject.light = optCurrBlock->GetLight();
        }

        if (FAILED(m_pComCBufferPerObject->MapDiscard(pContext, &cbPerObject, sizeof(cbPerObject))))
        {
            return E_FAIL;
        }
        pContext->VSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
        pContext->PSSetConstantBuffers(0, 1, m_pComCBufferPerObject->GetAdressOfBuffer());
    }

    m_pComEntityModel->BindBoneMatrix(pContext);

    m_pComEntityModel->Render(pContext, ctx);
    return S_OK;
}

void CCreeperEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
{
    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);
    //XMVECTOR vWishDir = XMVectorZero();

    // 가속
    float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
    float fAddSpeed = m_fSpeed - fCurrSpeed;
    if (fAddSpeed > 0.f)
    {
        float fAccelSpeed = std::min(18.f * m_fSpeed * fTimeDelta, fAddSpeed);
        vVel += vWishDir * fAccelSpeed;
    }

    // 수평 속도 제한
    XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
    float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
    if (fHorizSpeed > m_fSpeed)
    {
        vHoriz = XMVector3Normalize(vHoriz) * m_fSpeed;
        vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
    }

    // 마찰
    if (m_bOnGround)
    {
        float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
        if (fSpeed > 0.f)
        {
            // 의지 방향이 없거나(IDLE 등), 현재 속도가 가고 싶은 속도보다 빠르면 마찰 적용
            if (XMVectorGetX(XMVector3Length(vWishDir)) < 0.01f || fSpeed > m_fSpeed)
            {
                float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
                float vy = XMVectorGetY(vVel);
                vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
            }
        }
    }

    // 중력
    if (!m_bOnGround)
        vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);

    // AABB 충돌
    const XMFLOAT3 halfExtents = { 0.25f, 0.9f, 0.25f };
    XMFLOAT3 pos = GetTransform().GetPosition();

    _float fAddY = 1.f;
    XMFLOAT3 c = { pos.x, pos.y + fAddY, pos.z };


    // Y
    float velY = XMVectorGetY(vVel);
    float prevY = c.y;
    c.y += velY * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        if (velY < 0.f)
        {
            c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
            m_bOnGround = true;
        }
        else c.y = prevY;
        vVel = XMVectorSetY(vVel, 0.f);
    }
    else m_bOnGround = false;

    _bool bHitWall = false; // 이번 프레임에 벽에 막혔는가?

    // [X 축 충돌 검사]
    float px = c.x;
    c.x += XMVectorGetX(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.x = px;
        vVel = XMVectorSetX(vVel, 0.f);

        //  움직이려는 의지(vWishDir)가 있는데 X축이 막혔다면 벽에 박은 것!
        if (fabsf(XMVectorGetX(vWishDir)) > 0.01f)
            bHitWall = true;
    }

    // [Z 축 충돌 검사]
    float pz = c.z;
    c.z += XMVectorGetZ(vVel) * fTimeDelta;
    if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
    {
        c.z = pz;
        vVel = XMVectorSetZ(vVel, 0.f);

        //  움직이려는 의지(vWishDir)가 있는데 Z축이 막혔다면 벽에 박은 것!
        if (fabsf(XMVectorGetZ(vWishDir)) > 0.01f)
            bHitWall = true;
    }


    if (m_bOnGround && bHitWall)
    {
        // 마인크래프트 고증 점프 속도 (상황에 따라 5.0f ~ 6.0f 사이 조절)
        vVel = XMVectorSetY(vVel, 6.5f);
        m_bOnGround = false; // 공중에 떴으므로 상태 변경
    }

    {
        auto beforePos = GetTransform().GetPosition();
        m_vBeforePos = _float3{ beforePos.x, beforePos.y, beforePos.z };
    }

    // 최종 좌표 적용 및 속도 백업
    GetTransform().SetPosition(_float3{ c.x, c.y - fAddY, c.z });
    XMStoreFloat3(&m_vVelocity, vVel);


    {
        //
        auto befldPos = XMLoadFloat3(&m_vBeforePos);
        auto curldPos = GetTransform().GetLoadedPostion();

        m_fAccMovedSqLen += fabsf(XMVectorGetX(XMVector3Length(curldPos - befldPos)));
    }

    {
        if (m_fAccMovedSqLen > 1.f)
        {
            m_fAccMovedSqLen = 0.f;

            auto vPos = GetTransform().GetPosition();
            auto stepBlock2 = CGameInstance::Get().GetVoxelBlock(vPos.x, vPos.y - 1, vPos.z);
            if (auto stepBlock = CGameInstance::Get().GetVoxelBlock(std::floor(vPos.x), std::floor(vPos.y) - 1, std::floor(vPos.z)))
            {
                if (auto pCam = CGameInstance::Get().GetActiveGameCamera())
                {
                    _vector vDistVec = GetTransform().GetLoadedPostion() - pCam->GetTransform().GetLoadedPostion();
                    float fDistSq = XMVectorGetX(XMVector3LengthSq(vDistVec));
                    constexpr float MaxDistance = 32.f;
                    constexpr float MaxDistanceSq = MaxDistance * MaxDistance;
                    float ratioSq = std::clamp(fDistSq / MaxDistanceSq, 0.f, 1.f);
                    float fMaxVol = 0.1f;
                    float fVol = (1.f - ratioSq) * fMaxVol;

                    CGameInstance::Get().SoundPlay(CBlock3::GetSoundStep(stepBlock.value().GetType()), fVol);
                }
            }
        }
    }
}

void CCreeperEntity::ExplodeUpdate(_float fTimeDelta)
{
    SetPendingDestroyCascade();
    auto pos = GetTransform().GetPosition();
    //pos.y += 1.8f;
    //CGameInstance::Get().VoxelProcessExplodeBlock(pos.x, pos.y, pos.z, 10.f);
    //CGameInstance::Get().AddParticleRenderExplodeSmoke(pos, 25);
   
    CActivatedTNT::ExplodeAndDamageColliding(pos, 10, 5, 100);
}

void CCreeperEntity::TakeDamage(uint32_t iDamage, _vector vAttackerPos)
{
    if (m_eCurrentState == CREEPER_STATE::DIE) return; // 이미 죽었거나 피격 쿨타임 중이면 무시
    m_fSpeed = 3.f;
    m_bIsHit = true;
    m_fHitTimer = 0.3f; // 0.3초간 빨갛게 물듦

    // ---------------------------------------------------------
    // 마크 고증: 맞으면 살짝 위 + 공격자 반대 방향으로 튕겨나가는 넉백
    // ---------------------------------------------------------

    // 1. 내 위치 가져오기 (Y축 무시하여 수평 방향만 계산)
    XMFLOAT3 myPosFloat = GetTransform().GetPosition();
    XMVECTOR vMyPosXZ = XMVectorSetY(XMLoadFloat3(&myPosFloat), 0.f);
    XMVECTOR vAttackerPosXZ = XMVectorSetY(vAttackerPos, 0.f);

    // 2. 밀려날 방향 (내 위치 - 공격자 위치)
    XMVECTOR vKnockbackDir = vMyPosXZ - vAttackerPosXZ;

    // 만약 완전히 겹쳐서 벡터 길이가 0이라면 랜덤한 수평 방향으로 밀치기
    if (XMVectorGetX(XMVector3LengthSq(vKnockbackDir)) < 0.0001f)
    {
        vKnockbackDir = XMVectorSet(
            ((rand() % 100) - 50) * 0.01f,
            0.f,
            ((rand() % 100) - 50) * 0.01f,
            0.f
        );
    }

    // 방향 정규화
    vKnockbackDir = XMVector3Normalize(vKnockbackDir);

    // 3. 수평 넉백 힘 적용 (수치는 테스트하며 조절하세요)
    float fKnockbackPower = 3.5f;
    XMVECTOR vFinalKnockback = vKnockbackDir * fKnockbackPower;

    // 4. 작성하신 수직 점프 넉백(5.0f) 합치기
    vFinalKnockback = XMVectorSetY(vFinalKnockback, 5.0f);

    // 최종 속도로 적용
    XMStoreFloat3(&m_vVelocity, vFinalKnockback);

    m_bOnGround = false;

    // ---------------------------------------------------------
    // 체력 감소 및 사망 처리
    // ---------------------------------------------------------
    m_iHeart -= iDamage;

    if (m_iHeart <= 0)
    {
        m_eCurrentState = CREEPER_STATE::DIE;
        m_fDeathTimer = 0.f;

        m_fTargetYaw = m_fRootRotRadY;

        auto pHeadBone = m_pComEntityModel->GetBone("head");
        if (pHeadBone)
        {
            pHeadBone->SetRotation({ 0.f, 0.f, 0.f });
        }

        if (auto pCam = CGameInstance::Get().GetActiveGameCamera())
        {
            _vector vDistVec = GetTransform().GetLoadedPostion() - pCam->GetTransform().GetLoadedPostion();
            float fDistSq = XMVectorGetX(XMVector3LengthSq(vDistVec));
            constexpr float MaxDistance = 32.f;
            constexpr float MaxDistanceSq = MaxDistance * MaxDistance;
            float ratioSq = std::clamp(fDistSq / MaxDistanceSq, 0.f, 1.f);
            float fMaxVol = 0.1f;
            float fVol = (1.f - ratioSq) * fMaxVol;

            // 랜덤 재생
            CGameInstance::Get().SoundPlay("CREEPER_DEATH", fVol);
        }

        // 만약 CComAnimator 내부에 머리 회전용 쿼터니언 변수(m_vCurrentHeadRotQuat)를 쓰신다면
        // 여기서 함께 XMQuaternionIdentity() 등으로 초기화해주면 더욱 안전합니다.
    }

    if (auto pCam = CGameInstance::Get().GetActiveGameCamera())
    {
        _vector vDistVec = GetTransform().GetLoadedPostion() - pCam->GetTransform().GetLoadedPostion();
        float fDistSq = XMVectorGetX(XMVector3LengthSq(vDistVec));
        constexpr float MaxDistance = 32.f;
        constexpr float MaxDistanceSq = MaxDistance * MaxDistance;
        float ratioSq = std::clamp(fDistSq / MaxDistanceSq, 0.f, 1.f);
        float fMaxVol = 0.1f;
        float fVol = (1.f - ratioSq) * fMaxVol;

        // 랜덤 재생
        const char* sounds[] = { "CREEPER_SAY_1", "CREEPER_SAY_2", "CREEPER_SAY_3", "CREEPER_SAY_4" };
        CGameInstance::Get().SoundPlay(sounds[RandInt(0, 3)], fVol);
    }
}

//void CCreeperEntity::TakeDamage(uint32_t iDamage)
//{
//    if (m_eCurrentState == CREEPER_STATE::DIE) return; // 이미 죽었거나 피격 쿨타임 중이면 무시
//
//    m_bIsHit = true;
//    m_fHitTimer = 0.3f; // 0.3초간 빨갛게 물듦
//
//    // 마크 고증: 맞으면 살짝 위+바깥으로 팅겨나가는 넉백 추가
//    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);
//    vVel = XMVectorSetY(vVel, 5.0f); // 수직 점프 넉백
//    XMStoreFloat3(&m_vVelocity, vVel);
//
//    m_bOnGround = false;
//
//    m_iHeart -= iDamage;
//
//    if (m_iHeart <= 0)
//    {
//        m_eCurrentState = CREEPER_STATE::DIE;
//        m_fDeathTimer = 0.f;
//
//
//        m_fTargetYaw = m_fRootRotRadY;
//
//
//        auto pHeadBone = m_pComEntityModel->GetBone("head");
//        if (pHeadBone)
//        {
//            pHeadBone->SetRotation({ 0.f, 0.f, 0.f });
//        }
//
//        // 만약 CComAnimator 내부에 머리 회전용 쿼터니언 변수(m_vCurrentHeadRotQuat)를 쓰신다면
//        // 여기서 함께 XMQuaternionIdentity() 등으로 초기화해주면 더욱 안전합니다aaaaa.
//    }
//}

void CCreeperEntity::ProcessDestroy(_float fTimeDelta)
{
    SetPendingDestroyCascade();
    if (auto pObj = CGameInstance::Get().GetFirstGameObjectByLayer<CExperienceOrb>("56_ExperienceOrb"))
    {
        auto pos = GetTransform().GetPosition();
        pos.x += 0.f;
        pos.y += 0.55f;
        pObj->AddOrb(pos, {}, rand() % 16, 0.3f);
    }

    CGameInstance::Get().AddParticleRenderDeathSmoke(GetTransform().GetPosition(), 10);


    CItemObject::ItemInfo info{};
    info.eItemType = CItemObject::ITEM_TYPE::ITEM_Gunpowder;
    info.iCnt = 1;
    _float3 startPos{ GetTransform().GetPosition() };
    startPos.y += 1.f;
    CItemObject::SpawnDropItemObject(info, startPos, { 0.f, 2.f, 0.f });
}

UPtr<CCreeperEntity> CCreeperEntity::Create()
{
    auto pInstance = ToUPtr(new CCreeperEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CCreeperEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CCreeperEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CCreeperEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CCreeperEntity");
        return nullptr;
    }

    return pInstance;
}
