#include "ZombieEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"

#include "CollBox.h"

#include "ExperienceOrb.h"
#include "ItemObject.h"

NS_USING(Engine)


CZombieEntity::CZombieEntity()
{
}

CZombieEntity::CZombieEntity(const CZombieEntity& rhs)
    : CMonsterEntityObject{rhs}
{

}

CZombieEntity::~CZombieEntity()
{
}

HRESULT CZombieEntity::Initialize(void* pArg)
{
    if (FAILED(CMonsterEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Zombie" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Zombie" };
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


    //m_pCenterCollider = CCollBox::Create({ 0.f, 0.49f, 0.f }, { 0.49f, 0.49f, 0.49f });
    //m_pCenterCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.25f, 0.9f, 0.25f });
    m_pCenterCollider = CCollBox::Create({ 0.f, 1.f, 0.f }, { 0.25f, 0.9f, 0.25f });
    m_pCenterCollider->SetInnerPointer(this);

    return S_OK;
}

void CZombieEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

/*

//auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    //pLeftArm->SetRotation({ XMConvertToRadians(-90.f) , 0.f, 0.f});

    //auto pRightArm = m_pComEntityModel->GetBone("rightArm");
    //pRightArm->SetRotation({ XMConvertToRadians(-90.f) , 0.f, 0.f });

    // 1. 임시 스태틱 변수 선언 (테스트용)
    static float s_fLifeTime = 0.0f;     // 누적 생존 시간 (query.life_time 역할)
    static float s_fAttackTime = 0.0f;   // 공격 애니메이션 시간 (0.0 ~ 1.0 사이, variable.attack_time 역할)

    // 시간 업데이트 (실제 코드에서는 외부에서 받아오거나 제어해야 함)
    s_fLifeTime += fTimeDelta;

    // [테스트용] 스페이스바 누르면 팔을 휘두르도록 0~1 사이로 증가
    if (CGameInstance::Get().KeyPressing(DIK_SPACE)) // (기존 코드에 있던 키 입력 변수 가정)
    {
        s_fAttackTime += fTimeDelta * 2.f; // 공격 속도
        if (s_fAttackTime > 1.0f) s_fAttackTime = 1.0f;
    }
    else
    {
        s_fAttackTime -= fTimeDelta * 2.f; // 되돌아가기
        if (s_fAttackTime < 0.0f) s_fAttackTime = 0.0f;
    }

    // 2. MoLang 삼각함수는 '도(Degree)' 기준이므로 변환 헬퍼 람다 작성
    auto sinDeg = [](float deg) { return std::sin(XMConvertToRadians(deg)); };
    auto cosDeg = [](float deg) { return std::cos(XMConvertToRadians(deg)); };

    // 3. 공통 수식 미리 계산 (연산량 최적화 및 가독성)
    float swing1 = sinDeg(s_fAttackTime * 180.0f) * 57.3f;
    float attackTimeSq = (1.0f - s_fAttackTime) * (1.0f - s_fAttackTime);
    float swing2 = sinDeg((1.0f - attackTimeSq) * 180.0f) * 57.3f;

    // 숨쉬기(Idle) 시 팔이 까닥거리는 애니메이션 수식
    float lifeSway1 = sinDeg(s_fLifeTime * 76.776372f) * 2.865f;
    float lifeSway2 = cosDeg(s_fLifeTime * 103.13244f) * 2.865f;


    // 4. --- 왼팔 (Left Arm) ---
    // X: -90.0 - ((swing1 * 1.2) - (swing2 * 0.4)) - lifeSway1
    float leftArmX = -90.0f - (swing1 * 1.2f - swing2 * 0.4f) - lifeSway1;
    // Y: 5.73 - (swing1 * 0.6)
    float leftArmY = 5.73f - (swing1 * 0.6f);
    // Z: -lifeSway2 - 2.865
    float leftArmZ = -lifeSway2 - 2.865f;

    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    if (pLeftArm)
    {
        pLeftArm->SetRotation({ XMConvertToRadians(leftArmX), XMConvertToRadians(leftArmY), XMConvertToRadians(leftArmZ) });
    }


    // 5. --- 오른팔 (Right Arm) ---
    // X: 90.0 * (is_brandishing_spear - 1.0) -> 창이 없으면(0.0) -90.0
    //    -90.0 - ((swing1 * 1.2) - (swing2 * 0.4)) + lifeSway1
    float rightArmX = -90.0f - (swing1 * 1.2f - swing2 * 0.4f) + lifeSway1;
    // Y: (swing1 * 0.6) - 5.73
    float rightArmY = (swing1 * 0.6f) - 5.73f;
    // Z: lifeSway2 + 2.865
    float rightArmZ = lifeSway2 + 2.865f;

    auto pRightArm = m_pComEntityModel->GetBone("rightArm");
    if (pRightArm)
    {
        pRightArm->SetRotation({ XMConvertToRadians(rightArmX), XMConvertToRadians(rightArmY), XMConvertToRadians(rightArmZ) });
    }


*/

void CZombieEntity::Update(E::_float fTimeDelta)
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

    auto pHead = m_pComEntityModel->GetBone("head");
    auto pBody = m_pComEntityModel->GetBone("body");
    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    auto pRightArm = m_pComEntityModel->GetBone("rightArm");
    auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg");
    auto pRightLeg = m_pComEntityModel->GetBone("rightLeg");


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

    m_bAttack = false;
    _float fDistToPlayer = pPlayer ? XMVectorGetX(XMVector3Length(vPlayerPos - vCreeperPos)) : 999.f;
    switch (m_eCurrentState)
    {
    case ZOMBIE_STATE::IDLE:
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
            m_eCurrentState = ZOMBIE_STATE::CHASE_PLAYER;
        }

        // 타이머 끝나면 무작위로 걷기 시작
        if (m_fStateTimer <= 0.f) {
            m_eCurrentState = ZOMBIE_STATE::WANDER;
            m_fStateTimer = 2.f + (rand() % 40) * 0.1f;
            m_fTargetYaw = (rand() % 360) * (XM_PI / 180.f); // 걸어갈 새로운 무작위 방향
            m_fTargetPitch = 0.f; // 걸을 때는 정면 응시
        }

        break;
    case ZOMBIE_STATE::WANDER:
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
            m_eCurrentState = ZOMBIE_STATE::CHASE_PLAYER;
        }
        else if (m_fStateTimer <= 0.f) {
            m_eCurrentState = ZOMBIE_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기
        }

        break;
    case ZOMBIE_STATE::CHASE_PLAYER:
        if (m_bIsHit)
        {
            break;
        }
        m_fSpeed = 3.5f;



        if (fDistToPlayer >= fPlayerDetectDist) {
            m_eCurrentState = ZOMBIE_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기

            m_fAttackTime = 0.f;
        }
        else
        {
            if (fDistToPlayer <= 1.f)
            {
                // 조준 상태 변환
                m_fSpeed = 0.f;
                m_bAttack = true;

                m_fAttackTime += fTimeDelta;

                m_fWalkWeight += (0.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
                if (m_fWalkWeight < 0.001f) {
                    m_fWalkWeight = 0.0f;
                    m_fWalkTime = 0.0f;
                }
            }
            else
            {
                m_fWalkTime += fTimeDelta * 7.0f;
                m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
                // 다가가기
                m_fSpeed = 3.5f;

                m_fAttackTime = 0.f;
            }

            {
                XMVECTOR vToPlayer = XMVector3Normalize(vPlayerPos - vCreeperPos);
                m_fTargetYaw = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer));
                m_fTargetPitch = -atan2f(XMVectorGetY(vToPlayer), sqrtf(XMVectorGetX(vToPlayer) * XMVectorGetX(vToPlayer) + XMVectorGetZ(vToPlayer) * XMVectorGetZ(vToPlayer)));
            }

            vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
        }

        break;
    case ZOMBIE_STATE::DIE:
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

        E::CGameInstance::Get().AddColliderGroup("Coll_ZombieCenter", m_pCenterCollider.get());
        m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

        if (m_fDeathTimer >= TOTAL_DURATION)
            ProcessDestroy(fTimeDelta);
        return;
    }

    m_pComEntityModel->ResetBonesChannel();

    {
        // 1. 현재 이동(전진) 중인지 여부 체크
        // WANDER 상태이거나 CHASE_PLAYER 상태일 때를 이동 중으로 판별합니다.
        bool bIsMoving = (m_eCurrentState == ZOMBIE_STATE::WANDER || m_eCurrentState == ZOMBIE_STATE::CHASE_PLAYER);

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


    //if (CGameInstance::Get().KeyPressing(DIK_SPACE)) // (기존 코드에 있던 키 입력 변수 가정)
    //{
    //    s_fAttackTime += fTimeDelta * 2.f; // 공격 속도
    //    if (s_fAttackTime > 1.0f) s_fAttackTime = 1.0f;
    //}
    //else
    //{
    //    s_fAttackTime -= fTimeDelta * 2.f; // 되돌아가기
    //    if (s_fAttackTime < 0.0f) s_fAttackTime = 0.0f;
    //}

    if (m_bAttack)
    {
        m_fAttackTime += fTimeDelta * 2.f; // 공격 속도
        //if (m_fAttackTime > 1.0f) m_fAttackTime = 1.0f;

        if (m_fAttackTime > 1.0f)
        {
            m_fAttackTime = 1.0;

            m_fAttackDelay += fTimeDelta;

            if (m_fAttackDelay > 1.f)
            {
                m_fAttackDelay = 0.f;
                m_fAttackTime = 0.f;
            }
        }
    }
    else
    {
        m_fAttackTime -= fTimeDelta * 2.f; // 되돌아가기
        if (m_fAttackTime < 0.0f) m_fAttackTime = 0.0f;
    }

    {
        // 2. MoLang 삼각함수는 '도(Degree)' 기준이므로 변환 헬퍼 람다 작성
        auto sinDeg = [](float deg) { return std::sin(XMConvertToRadians(deg)); };
        auto cosDeg = [](float deg) { return std::cos(XMConvertToRadians(deg)); };

        // 3. 공통 수식 미리 계산 (연산량 최적화 및 가독성)
        float swing1 = sinDeg(m_fAttackTime * 180.0f) * 57.3f;
        float attackTimeSq = (1.0f - m_fAttackTime) * (1.0f - m_fAttackTime);
        float swing2 = sinDeg((1.0f - attackTimeSq) * 180.0f) * 57.3f;

        // 숨쉬기(Idle) 시 팔이 까닥거리는 애니메이션 수식
        float lifeSway1 = sinDeg(m_fLifeTime * 76.776372f) * 2.865f;
        float lifeSway2 = cosDeg(m_fLifeTime * 103.13244f) * 2.865f;


        // 4. --- 왼팔 (Left Arm) ---
        // X: -90.0 - ((swing1 * 1.2) - (swing2 * 0.4)) - lifeSway1
        float leftArmX = -90.0f - (swing1 * 1.2f - swing2 * 0.4f) - lifeSway1;
        // Y: 5.73 - (swing1 * 0.6)
        float leftArmY = 5.73f - (swing1 * 0.6f);
        // Z: -lifeSway2 - 2.865
        float leftArmZ = -lifeSway2 - 2.865f;

        auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
        if (pLeftArm)
        {
            pLeftArm->SetRotation({ XMConvertToRadians(leftArmX), XMConvertToRadians(leftArmY), XMConvertToRadians(leftArmZ) });
        }


        // 5. --- 오른팔 (Right Arm) ---
        // X: 90.0 * (is_brandishing_spear - 1.0) -> 창이 없으면(0.0) -90.0
        //    -90.0 - ((swing1 * 1.2) - (swing2 * 0.4)) + lifeSway1
        float rightArmX = -90.0f - (swing1 * 1.2f - swing2 * 0.4f) + lifeSway1;
        // Y: (swing1 * 0.6) - 5.73
        float rightArmY = (swing1 * 0.6f) - 5.73f;
        // Z: lifeSway2 + 2.865
        float rightArmZ = lifeSway2 + 2.865f;

        auto pRightArm = m_pComEntityModel->GetBone("rightArm");
        if (pRightArm)
        {
            pRightArm->SetRotation({ XMConvertToRadians(rightArmX), XMConvertToRadians(rightArmY), XMConvertToRadians(rightArmZ) });
        }
    }

    {
        float fTcos0 = std::cos(m_fWalkTime) * 30.0f * m_fWalkWeight;

        XMFLOAT3 normalLeftLeg = { fTcos0 * -1.4f, -0.1f, -0.1f };
        XMFLOAT3 normalRightLeg = { fTcos0 * 1.4f, 0.1f, 0.1f };

        if (pLeftLeg)  pLeftLeg->SetRotation({ XMConvertToRadians(normalLeftLeg.x), XMConvertToRadians(normalLeftLeg.y), XMConvertToRadians(normalLeftLeg.z) });
        if (pRightLeg) pRightLeg->SetRotation({ XMConvertToRadians(normalRightLeg.x), XMConvertToRadians(normalRightLeg.y), XMConvertToRadians(normalRightLeg.z) });


        //float fMaxLegAngle = 35.0f; // 최대 앞뒤 회전 각도 (도 단위)
        //float fLegRot = std::sin(m_fWalkTime) * fMaxLegAngle * m_fWalkWeight;

        //float legRot0 = fLegRot;
        //float legRot1 = -fLegRot;
        //float legRot2 = -fLegRot;
        //float legRot3 = fLegRot;

        //if (pLeg0) pLeg0->SetRotation({ XMConvertToRadians(legRot0), 0.f, 0.f });
        //if (pLeg1) pLeg1->SetRotation({ XMConvertToRadians(legRot1), 0.f, 0.f });
        //if (pLeg2) pLeg2->SetRotation({ XMConvertToRadians(legRot2), 0.f, 0.f });
        //if (pLeg3) pLeg3->SetRotation({ XMConvertToRadians(legRot3), 0.f, 0.f });
    }


    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

    VelocityUpdate(fTimeDelta, vWishDir);

    E::CGameInstance::Get().AddColliderGroup("Coll_ZombieCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

}

void CZombieEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CZombieEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        E::CB_PER_OBJECT cbPerObject{};
        cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

        if (m_eCurrentState == ZOMBIE_STATE::DIE)
        {
            _float fProgress = m_fDeathTimer / 1.0f; // 0.0f ~ 1.0f

            // 시간이 흐를수록 완전히 시뻘개지도록 RGB 제어 (알파 채널을 깎으면 시각적 디스폰 효과)
            cbPerObject.vBaseColor = _float4(1.0f, 0.05f, 0.05f, 1.0f - fProgress);
        }
        else if (m_bIsHit)
        {
            cbPerObject.vBaseColor = _float4(1.f, 0.2f, 0.2f, 1.f); // 피격 깜빡임
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

void CZombieEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
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

    // 최종 좌표 적용 및 속도 백업
    GetTransform().SetPosition(_float3{ c.x, c.y - fAddY, c.z });
    XMStoreFloat3(&m_vVelocity, vVel);
}

void CZombieEntity::TakeDamage(uint32_t iDamage, _vector vAttackerPos)
{
    if (m_eCurrentState == ZOMBIE_STATE::DIE) return; // 이미 죽었거나 피격 쿨타임 중이면 무시

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
    float fKnockbackPower = 13.5f;
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
        m_eCurrentState = ZOMBIE_STATE::DIE;
        m_fDeathTimer = 0.f;

        m_fTargetYaw = m_fRootRotRadY;

        auto pHeadBone = m_pComEntityModel->GetBone("head");
        if (pHeadBone)
        {
            pHeadBone->SetRotation({ 0.f, 0.f, 0.f });
        }

        // 만약 CComAnimator 내부에 머리 회전용 쿼터니언 변수(m_vCurrentHeadRotQuat)를 쓰신다면
        // 여기서 함께 XMQuaternionIdentity() 등으로 초기화해주면 더욱 안전합니다.
    }
}

void CZombieEntity::ProcessDestroy(_float fTimeDelta)
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

    //CItemObject::ItemInfo info{};
//info.eItemType = CItemObject::ITEM_TYPE::ITEM_Gunpowder;
//info.iCnt = 1;
//_float3 startPos{ GetTransform().GetPosition() };
//startPos.y += 1.f;
//CItemObject::SpawnDropItemObject(info, startPos, { 0.f, 2.f, 0.f });
}

UPtr<CZombieEntity> CZombieEntity::Create()
{
    auto pInstance = ToUPtr(new CZombieEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CZombieEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CZombieEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CZombieEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CZombieEntity");
        return nullptr;
    }

    return pInstance;
}
