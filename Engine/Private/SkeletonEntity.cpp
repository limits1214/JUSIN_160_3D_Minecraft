#include "SkeletonEntity.h"

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
#include "HandHeldItemObject.h"

#include "ArrowEntity.h"
NS_USING(Engine)

CSkeletonEntity::CSkeletonEntity()
{
}

CSkeletonEntity::CSkeletonEntity(const CSkeletonEntity& rhs)
    : CMonsterEntityObject{rhs}
{

}

CSkeletonEntity::~CSkeletonEntity()
{
}

HRESULT CSkeletonEntity::Initialize(void* pArg)
{
    if (FAILED(CMonsterEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Skeleton" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Skeleton" };
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

    {
        E::CHandHeldItemObject::DESC Desc{};
        Desc.sObjectTag = "SkeletonHandHeldItem";
        //"24_HandHeldItem", "ITEM", "Prototype_GameObject_HandHeldItem"
        if (auto handle = CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_HandHeldItem", "76_SKELETON_HANDHELD_ITEM", &Desc))
        {
            m_hHandHeld = handle.value();

            if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CHandHeldItemObject>(m_hHandHeld))
            {
                CItemObject::ItemInfo info{ CItemObject::ITEM_TYPE::ITEM_Bow_Standby };
                pObj->SetItemInfo(info);
                pObj->SetVIBufferID({ "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(info) });
                pObj->SetRender(true);
            }
        }
        else
        {
            return E_FAIL;
        }
    }

    return S_OK;
}

void CSkeletonEntity::PriorityUpdate(E::_float fTimeDelta)
{
}


/*
홀딩
// 테스트용 상태 변수
static bool s_bIsHoldingLeft = false;
static bool s_bIsHoldingRight = true; // 오른손에 무언가를 들고 있다고 가정

// 왼팔
float leftArmX_Holding = s_bIsHoldingLeft ? -18.0f : 0.0f;
auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
if (pLeftArm)
{
    pLeftArm->SetRotation({ XMConvertToRadians(leftArmX_Holding), 0.f, 0.f });
}

// 오른팔
float rightArmX_Holding = s_bIsHoldingRight ? -18.0f : 0.0f;
auto pRightArm = m_pComEntityModel->GetBone("rightArm");
if (pRightArm)
{
    pRightArm->SetRotation({ XMConvertToRadians(rightArmX_Holding), 0.f, 0.f });
}

*/


/*

// 1. 상태 변수 설정
    static float s_fLifeTime = 0.0f;
    s_fLifeTime += fTimeDelta;

    // 대상을 바라보는 카메라/시선 각도 (도 단위, 임시값)
    // 플레이어를 쳐다보도록 Pitch(X)와 Yaw(Y)를 계산해서 넣어주어야 합니다.
    float targetRotX = 0.0f;  // 위아래 조준 각도
    float targetRotY = 0.0f;  // 좌우 조준 각도

    // 2. 헬퍼 람다
    auto sinDeg = [](float deg) { return std::sin(XMConvertToRadians(deg)); };
    auto cosDeg = [](float deg) { return std::cos(XMConvertToRadians(deg)); };

    // 3. 숨쉬기(Idle) 흔들림 공통 수식
    float bowSway1 = sinDeg(s_fLifeTime * 76.8f) * 2.865f;
    float bowSway2 = cosDeg(s_fLifeTime * 103.2f) * 2.865f;

    // 4. --- 왼팔 (활을 뻗어 들고 있는 팔) ---
    // X: targetRotX - 90.0 - bowSway1
    float bowLeftArmX = targetRotX - 90.0f - bowSway1;
    // Y: targetRotY + 28.65
    float bowLeftArmY = targetRotY + 28.65f;
    // Z: -bowSway2 - 2.865
    float bowLeftArmZ = -bowSway2 - 2.865f;

    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    if (pLeftArm)
    {
        pLeftArm->SetRotation({ XMConvertToRadians(bowLeftArmX), XMConvertToRadians(bowLeftArmY), XMConvertToRadians(bowLeftArmZ) });
    }

    // 5. --- 오른팔 (활시위를 당기고 있는 팔) ---
    // X: targetRotX - 90.0 + bowSway1
    float bowRightArmX = targetRotX - 90.0f + bowSway1;
    // Y: targetRotY - 5.73
    float bowRightArmY = targetRotY - 5.73f;
    // Z: bowSway2 + 2.865
    float bowRightArmZ = bowSway2 + 2.865f;

    auto pRightArm = m_pComEntityModel->GetBone("rightArm");
    if (pRightArm)
    {
        pRightArm->SetRotation({ XMConvertToRadians(bowRightArmX), XMConvertToRadians(bowRightArmY), XMConvertToRadians(bowRightArmZ) });
    }


*/
//
//void CSkeletonEntity::Update(E::_float fTimeDelta)
//{
//    m_fStateTimer -= fTimeDelta;
//    XMVECTOR vWishDir = XMVectorZero();
//    auto pPlayer = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer);
//
//    _float fPlayerDetectDist = 10.f;
//
//    auto playerpos = pPlayer->GetTransform().GetPosition();
//    playerpos.y += 1.5f;
//
//    auto creeperpos = GetTransform().GetPosition();
//    creeperpos.y += 1.325f;
//
//    XMVECTOR vCreeperPos = XMLoadFloat3(&creeperpos);
//    XMVECTOR vPlayerPos = pPlayer ? XMLoadFloat3(&playerpos) : XMVectorZero();
//
//    auto vDir = XMVector3Normalize(vPlayerPos - vCreeperPos);
//
//
//    auto pHead = m_pComEntityModel->GetBone("head");
//    auto pBody = m_pComEntityModel->GetBone("body");
//    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
//    auto pRightArm = m_pComEntityModel->GetBone("rightArm");
//    auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg");
//    auto pRightLeg = m_pComEntityModel->GetBone("rightLeg");
//
//
//    //m_fSwellingProgress;
//
//    if (m_bIsHit)
//    {
//        m_fHitTimer -= fTimeDelta;
//        if (m_fHitTimer <= 0.f)
//        {
//            m_bIsHit = false;
//            m_fHitTimer = 0.f;
//        }
//
//        if (m_iHeart <= 0)
//        {
//            //m_eCurrentState = PIG_STATE::DIE;
//            //return;
//        }
//    }
//
//    m_bBowAndArrow = false;
//    _float fDistToPlayer = pPlayer ? XMVectorGetX(XMVector3Length(vPlayerPos - vCreeperPos)) : 999.f;
//    switch (m_eCurrentState)
//    {
//    case SKELETON_STATE::IDLE:
//        m_fSpeed = 0.f; // 정지
//
//        m_fWalkWeight += (0.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
//        if (m_fWalkWeight < 0.001f) {
//            m_fWalkWeight = 0.0f;
//            m_fWalkTime = 0.0f;
//        }
//
//        // -------------------------------------------------------------
//        // [IDLE 전용 시선 처리] 플레이어가 없을 때 무작위로 고개 돌리기
//        // -------------------------------------------------------------
//        if (fDistToPlayer >= fPlayerDetectDist)
//        {
//            m_fIdleLookTimer -= fTimeDelta;
//            if (m_fIdleLookTimer <= 0.f)
//            {
//                // 1~3초마다 한 번씩 고개를 돌림
//                m_fIdleLookTimer = 1.0f + (rand() % 20) * 0.1f;
//
//                // 현재 몸통 방향(m_fRootRotRadY)을 기준으로 좌우 35도 이내의 무작위 각도를 타겟으로 설정
//                float fRandomOffset = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(45.f);
//                m_fTargetYaw = m_fRootRotRadY + fRandomOffset;
//
//                // 가끔 위아래도 살짝 쳐다보게 설정 (-10도 ~ 10도)
//                m_fTargetPitch = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(20.f);
//            }
//        }
//        else // 플레이어가 4블록 이내로 접근하면 쳐다봄 (CHASE 전환)
//        {
//            m_eCurrentState = SKELETON_STATE::CHASE_PLAYER;
//        }
//
//        // 타이머 끝나면 무작위로 걷기 시작
//        if (m_fStateTimer <= 0.f) {
//            m_eCurrentState = SKELETON_STATE::WANDER;
//            m_fStateTimer = 2.f + (rand() % 40) * 0.1f;
//            m_fTargetYaw = (rand() % 360) * (XM_PI / 180.f); // 걸어갈 새로운 무작위 방향
//            m_fTargetPitch = 0.f; // 걸을 때는 정면 응시
//        }
//
//        break;
//    case SKELETON_STATE::WANDER:
//        m_fSpeed = 1.5f; // 평화롭게 걷는 속도
//
//        m_fWalkTime += fTimeDelta * 7.0f;
//        m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
//
//        // 가고 싶은 방향 벡터 추출 (TargetYaw 기준 정면)
//        vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
//        m_pComAnimator->QuadrupedWalk(fTimeDelta * 0.1f); // 걷기 애니메이션
//
//        if (fDistToPlayer < fPlayerDetectDist) {
//            m_eCurrentState = SKELETON_STATE::CHASE_PLAYER;
//        }
//        else if (m_fStateTimer <= 0.f) {
//            m_eCurrentState = SKELETON_STATE::IDLE;
//            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기
//        }
//
//        break;
//    case SKELETON_STATE::CHASE_PLAYER:
//        m_fSpeed = 3.5f;
//
//        m_fWalkTime += fTimeDelta * 7.0f;
//        m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
//
//        if (fDistToPlayer >= fPlayerDetectDist) {
//            m_eCurrentState = SKELETON_STATE::IDLE;
//            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기
//
//
//        }
//        else
//        {
//            if (fDistToPlayer <= 5.f)
//            {
//                // 조준
//                m_fSpeed = 0.f;
//                m_bBowAndArrow = true;
//
//            }
//            else
//            {
//                // 딸가기
//                m_fSpeed = 3.5f;
//            }
//
//
//
//            {
//                XMVECTOR vToPlayer = XMVector3Normalize(vPlayerPos - vCreeperPos);
//                m_fTargetYaw = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer));
//                m_fTargetPitch = -atan2f(XMVectorGetY(vToPlayer), sqrtf(XMVectorGetX(vToPlayer) * XMVectorGetX(vToPlayer) + XMVectorGetZ(vToPlayer) * XMVectorGetZ(vToPlayer)));
//            }
//
//            vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
//        }
//
//        break;
//    case SKELETON_STATE::DIE:
//        m_fDeathTimer += fTimeDelta;
//
//        m_pComEntityModel->ResetBonesChannel();
//        m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
//
//        constexpr float FALL_DURATION = 0.5f;
//        constexpr float TOTAL_DURATION = 1.5f;
//
//        float fFallT = std::min(m_fDeathTimer / FALL_DURATION, 1.0f);
//        float fEased = 1.f - (1.f - fFallT) * (1.f - fFallT);
//        float fRoll = XMConvertToRadians(90.f) * fEased;
//
//        // 1. Yaw 쿼터니언 (바라보는 방향 고정)
//        _vector qYaw = XMQuaternionRotationAxis(
//            XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fRootRotRadY);
//
//        // 2. Yaw 적용 후 실제 Look 방향(월드 Z를 Yaw로 회전)
//        _vector vLook = XMVector3Rotate(
//            XMVectorSet(0.f, 0.f, 1.f, 0.f), qYaw);
//
//        // 3. 그 Look축을 기준으로 Roll
//        _vector qRoll = XMQuaternionRotationAxis(vLook, fRoll);
//
//        // 4. Roll * Yaw 순서로 결합 (Roll이 월드 공간에서 먼저)
//        _vector qFinal = XMQuaternionMultiply(qYaw, qRoll);
//
//        GetTransform().SetQuaternion(qFinal);
//
//        VelocityUpdate(fTimeDelta, XMVectorZero());
//
//        E::CGameInstance::Get().AddColliderGroup("Coll_PigCenter", m_pCenterCollider.get());
//        m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());
//
//        if (m_fDeathTimer >= TOTAL_DURATION)
//            ProcessDestroy(fTimeDelta);
//        break;
//    }
//
//    m_pComEntityModel->ResetBonesChannel();
//
//    {
//        // 1. 현재 이동(전진) 중인지 여부 체크
//        // WANDER 상태이거나 CHASE_PLAYER 상태일 때를 이동 중으로 판별합니다.
//        bool bIsMoving = (m_eCurrentState == SKELETON_STATE::WANDER || m_eCurrentState == SKELETON_STATE::CHASE_PLAYER);
//
//        // 타겟 방향과 몸통 방향의 차이 계산
//        float toRotYRadDiff = m_fTargetYaw - m_fRootRotRadY;
//
//        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
//        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;
//
//        float fTargetHeadY = 0.f;
//
//        if (m_bBowAndArrow)
//        {
//
//        }
//        else if (bIsMoving)
//        {
//            // -------------------------------------------------------------
//            // [규칙 A] 전진(이동) 중일 때: 몸통과 머리를 동일선상으로 정렬
//            // -------------------------------------------------------------
//            // 몸통(Root)이 타겟 방향(이동 방향)을 향해 딜레이 없이 직접 회전합니다.
//            // (이동 중일 때는 회전 속도를 8.f~10.f 정도로 높여주면 더 빠릿하게 돕니다)
//            m_fRootRotRadY += toRotYRadDiff * 8.f * fTimeDelta;
//
//            // 머리의 목표 로컬 Y 각도는 0 즉, 몸통과 똑같은 정면(동일선상)을 바라봅니다.
//            fTargetHeadY = 0.f;
//        }
//        else
//        {
//            // -------------------------------------------------------------
//            // [규칙 B] 제자리에 서 있을 때 (IDLE): 기존의 45도 시선 분리 로직
//            // -------------------------------------------------------------
//            float sign = toRotYRadDiff > 0 ? 1.f : -1.f;
//            float threshold = XMConvertToRadians(45.f);
//
//            if (fabsf(toRotYRadDiff) > threshold)
//            {
//                float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
//                m_fRootRotRadY += delta;
//                fTargetHeadY = threshold * sign;
//            }
//            else
//            {
//                fTargetHeadY = toRotYRadDiff;
//            }
//        }
//
//        // 2. Head Y 보간 처리 (목표 각도를 향해 부드럽게 회전)
//        float headYDiff = fTargetHeadY - m_fHeadRotRadY;
//        m_fHeadRotRadY += headYDiff * 10.f * fTimeDelta;
//
//        // 3. Pitch (상하 회전) 보간 처리 - 기존 유지
//        float pitchDiff = m_fTargetPitch - m_fHeadRotRadX;
//        m_fHeadRotRadX += pitchDiff * 5.f * fTimeDelta;
//
//        // 4. 최종 계산된 누적 변수들을 Bone에 절대값으로 대입
//        if (pHead)
//        {
//            pHead->GetRotation()->y = m_fHeadRotRadY;
//            pHead->GetRotation()->x = m_fHeadRotRadX;
//        }
//        if (pBody)
//        {
//            pBody->GetRotation()->y = m_fRootRotRadY;
//        }
//    }
//
//    //static float fLifeTime = 0.f;
//    //fLifeTime += fTimeDelta;
//
//    m_fLifeTime += fTimeDelta;
//
//    if (m_bBowAndArrow)
//    {
//        // 1. 타이머 업데이트 (팔의 숨쉬기 흔들림용)
//        m_fLifeTime += fTimeDelta;
//
//        // 2. 실시간으로 계산된 Pitch/Yaw(라디안)를 도(Degree) 단위로 변환
//        float targetRotX = XMConvertToDegrees(m_fTargetPitch);
//        float targetRotY = XMConvertToDegrees(m_fTargetYaw - m_fRootRotRadY);
//
//        // 3. 헬퍼 람다
//        auto sinDeg = [](float deg) { return std::sin(XMConvertToRadians(deg)); };
//        auto cosDeg = [](float deg) { return std::cos(XMConvertToRadians(deg)); };
//
//        // 4. 숨쉬기(Idle) 흔들림 공통 수식
//        float bowSway1 = sinDeg(m_fLifeTime * 76.8f) * 2.865f;
//        float bowSway2 = cosDeg(m_fLifeTime * 103.2f) * 2.865f;
//
//        // 5. --- 왼팔 (활을 뻗어 들고 있는 팔) ---
//        float bowLeftArmX = targetRotX - 90.0f - bowSway1;
//        float bowLeftArmY = targetRotY + 28.65f;
//        float bowLeftArmZ = -bowSway2 - 2.865f;
//
//        auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
//        if (pLeftArm)
//        {
//            pLeftArm->SetRotation({
//                XMConvertToRadians(bowLeftArmX),
//                XMConvertToRadians(bowLeftArmY),
//                XMConvertToRadians(bowLeftArmZ)
//                });
//        }
//
//        // 6. --- 오른팔 (활시위를 당기고 있는 팔) ---
//        float bowRightArmX = targetRotX - 90.0f + bowSway1;
//        float bowRightArmY = targetRotY - 5.73f;
//        float bowRightArmZ = bowSway2 + 2.865f;
//
//        auto pRightArm = m_pComEntityModel->GetBone("rightArm");
//        if (pRightArm)
//        {
//        }
//        pRightArm->SetRotation({
//            XMConvertToRadians(bowRightArmX),
//            XMConvertToRadians(bowRightArmY),
//            XMConvertToRadians(bowRightArmZ)
//            });
//    }
//
//    // 7. --- 다리 처리 (조준 시 가만히 서 있기) ---
//    // X축(앞뒤 회전)을 0.0f로 밀어버려서 걷는 애니메이션을 완전히 멈춥니다.
//    //auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg");
//    //auto pRightLeg = m_pComEntityModel->GetBone("rightLeg");
//
//    if (pLeftLeg) {
//        pLeftLeg->SetRotation({
//            XMConvertToRadians(0.0f),         //  앞뒤 흔들림 없음 (가만히 서 있음)
//            XMConvertToRadians(-0.1f),
//            XMConvertToRadians(-0.1f)
//            });
//    }
//    if (pRightLeg) {
//        pRightLeg->SetRotation({
//            XMConvertToRadians(0.0f),         // 앞뒤 흔들림 없음 (가만히 서 있음)
//            XMConvertToRadians(0.1f),
//            XMConvertToRadians(0.1f)
//            });
//    }
//    else
//    {
//        {
//            // [0. 타이머 업데이트]
//            //m_fLifeTime += fTimeDelta; // Bob 애니메이션용 (항상 흐름)
//
//            // 이동 중이면 m_fWalkWeight는 1.0을 향해 가고, 멈추면 0.0을 향해 가도록 
//            // 이전 FSM(상태머신) 코드에서 제어되고 있어야 합니다.
//
//
//            // -------------------------------------------------------------
//            // 1. BOB 애니메이션 계산 (항상 작동, Z축 제어)
//            // -------------------------------------------------------------
//            float fBobDegreeArg = m_fLifeTime * 103.2f;
//            float fCosValue = std::cos(XMConvertToRadians(fBobDegreeArg));
//
//            float fRightArmBobZ = (fCosValue * 2.865f) + 2.865f;
//            float fLeftArmBobZ = fRightArmBobZ * -1.0f;
//
//
//            // -------------------------------------------------------------
//            // 2. MOVE 애니메이션 계산 (m_fWalkWeight에 의해 자동 온/오프, X축 제어)
//            // -------------------------------------------------------------
//            float fMaxArmAngle = 30.0f;
//
//            // 만약 멈춰있다면(m_fWalkWeight == 0), fTcos0은 자연스럽게 0이 됩니다!
//            float fTcos0 = std::cos(m_fWalkTime) * fMaxArmAngle * m_fWalkWeight;
//
//            float fLeftArmX = fTcos0;
//            float fRightArmX = -fTcos0;
//
//            // 다리도 Move에 맞춰 함께 계산
//            float fLeftLegX = fTcos0 * -1.4f;
//            float fRightLegX = fTcos0 * 1.4f;
//
//
//            // -------------------------------------------------------------
//            // 3. 최종 본(Bone)에 회전값 적용 (X축과 Z축을 결합)
//            // -------------------------------------------------------------
//
//            // 팔: Move(앞뒤 X) + Bob(옆으로 Z) 결합
//            if (pLeftArm) {
//                pLeftArm->SetRotation({
//                    XMConvertToRadians(fLeftArmX),
//                    0.0f,
//                    XMConvertToRadians(fLeftArmBobZ)
//                    });
//            }
//            if (pRightArm) {
//                pRightArm->SetRotation({
//                    XMConvertToRadians(fRightArmX),
//                    0.0f,
//                    XMConvertToRadians(fRightArmBobZ)
//                    });
//            }
//
//            // 다리: Move(앞뒤 X) + 고정 오프셋 적용 (다리는 Bob의 영향을 받지 않음)
//            if (pLeftLeg) {
//                pLeftLeg->SetRotation({
//                    XMConvertToRadians(fLeftLegX),
//                    XMConvertToRadians(-0.1f),
//                    XMConvertToRadians(-0.1f)
//                    });
//            }
//            if (pRightLeg) {
//                pRightLeg->SetRotation({
//                    XMConvertToRadians(fRightLegX),
//                    XMConvertToRadians(0.1f),
//                    XMConvertToRadians(0.1f)
//                    });
//            }
//        }
//    }
//    
//
//
//
//
//    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
//
//    VelocityUpdate(fTimeDelta, vWishDir);
//
//    E::CGameInstance::Get().AddColliderGroup("Coll_SkeletonCenter", m_pCenterCollider.get());
//    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());
//}

void CSkeletonEntity::Update(E::_float fTimeDelta)
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

    // 상태 체크 시작 전 초기화
    m_bBowAndArrow = false;
    _float fDistToPlayer = pPlayer ? XMVectorGetX(XMVector3Length(vPlayerPos - vCreeperPos)) : 999.f;

    // -------------------------------------------------------------
    // 1. FSM 상태 머신 처리
    // -------------------------------------------------------------
    switch (m_eCurrentState)
    {
    case SKELETON_STATE::IDLE:
        m_fSpeed = 0.f;

        m_fWalkWeight += (0.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
        if (m_fWalkWeight < 0.001f) {
            m_fWalkWeight = 0.0f;
            m_fWalkTime = 0.0f;
        }

        if (fDistToPlayer >= fPlayerDetectDist)
        {
            m_fIdleLookTimer -= fTimeDelta; // 원본 변수명 확인 필요 (기존 m_fIdleLookTimer 참고)
            if (m_fIdleLookTimer <= 0.f)
            {
                m_fIdleLookTimer = 1.0f + (rand() % 20) * 0.1f;
                float fRandomOffset = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(45.f);
                m_fTargetYaw = m_fRootRotRadY + fRandomOffset;
                m_fTargetPitch = ((rand() % 200) - 100) / 100.f * XMConvertToRadians(20.f);
            }
        }
        else
        {
            m_eCurrentState = SKELETON_STATE::CHASE_PLAYER;
        }

        if (m_fStateTimer <= 0.f) {
            m_eCurrentState = SKELETON_STATE::WANDER;
            m_fStateTimer = 2.f + (rand() % 40) * 0.1f;
            m_fTargetYaw = (rand() % 360) * (XM_PI / 180.f);
            m_fTargetPitch = 0.f;
        }
        break;

    case SKELETON_STATE::WANDER:
        m_fSpeed = 1.5f;

        m_fWalkTime += fTimeDelta * 7.0f;
        m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;

        vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
        m_pComAnimator->QuadrupedWalk(fTimeDelta * 0.1f);

        if (fDistToPlayer < fPlayerDetectDist) {
            m_eCurrentState = SKELETON_STATE::CHASE_PLAYER;
        }
        else if (m_fStateTimer <= 0.f) {
            m_eCurrentState = SKELETON_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f;
        }
        break;

    case SKELETON_STATE::CHASE_PLAYER:
        if (fDistToPlayer >= fPlayerDetectDist) {
            m_eCurrentState = SKELETON_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f;
        }
        else
        {
            if (fDistToPlayer <= 7.f)
            {
                // 조준 상태 변환
                m_fSpeed = 0.f;
                m_bBowAndArrow = true;

                m_fArrowShootTimer += fTimeDelta;
                if (m_fArrowShootTimer > 1.f)
                {
                    m_fArrowShootTimer = 0.f;

                    {
                        E::CArrowEntity::DESC Desc{};
                        Desc.sObjectTag = "Arrow";
                        if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArrowEntity",
                            "49_ARROW", &Desc))
                        {
                            if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CArrowEntity>(handle.value()))
                            {
                                //GetTransform().GetPosition();
                                //const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
                                auto pos = GetTransform().GetPosition();
                                pos.y += 1.5f;
                                //auto look = GetTransform().GetState(STATE::LOOK);
                                //auto pos = XMLoadFloat3(&rayOrigin2);
                                //auto look = XMLoadFloat3(&rayDir2);

                                pObj->Shoot(XMLoadFloat3(&pos), vDir, 30.f);
                            }
                        }
                    }
                }
            }
            else
            {
                m_fWalkTime += fTimeDelta * 7.0f;
                m_fWalkWeight += (1.0f - m_fWalkWeight) * fTimeDelta * 8.0f;
                // 다가가기
                m_fSpeed = 3.5f;
            }

            {
                XMVECTOR vToPlayer = XMVector3Normalize(vPlayerPos - vCreeperPos);
                m_fTargetYaw = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer));
                m_fTargetPitch = -atan2f(XMVectorGetY(vToPlayer), sqrtf(XMVectorGetX(vToPlayer) * XMVectorGetX(vToPlayer) + XMVectorGetZ(vToPlayer) * XMVectorGetZ(vToPlayer)));
            }

            vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
        }
        break;

    case SKELETON_STATE::DIE:
        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CHandHeldItemObject>(m_hHandHeld))
        {
            pObj->SetPendingDestroyCascade();
        }

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

        E::CGameInstance::Get().AddColliderGroup("Coll_SkeletonCenter", m_pCenterCollider.get());
        m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

        if (m_fDeathTimer >= TOTAL_DURATION)
            ProcessDestroy(fTimeDelta);
        return;
    }

    m_pComEntityModel->ResetBonesChannel();

    // -------------------------------------------------------------
    // 2. 조준 보간용 가중치(m_fBowWeight) 업데이트
    // -------------------------------------------------------------
    if (m_bBowAndArrow)
    {
        m_fBowWeight = 1.f;
        //m_fBowWeight += (1.0f - m_fBowWeight) * fTimeDelta * 7.0f;  // 7.0f는 보간 속도
        m_fWalkWeight += (0.0f - m_fWalkWeight) * fTimeDelta * 7.0f; // 조준 시 걷기 모션은 부드럽게 끎
    }
    else
    {
        m_fBowWeight = 0.;
        //m_fBowWeight += (0.0f - m_fBowWeight) * fTimeDelta * 7.0f;
    }

    m_fLifeTime += fTimeDelta; // 애니메이션 타임 누적


    // -------------------------------------------------------------
    // 3. 회전 처리 (몸통 및 머리 시선 분리)
    // -------------------------------------------------------------
    {
        bool bIsMoving = (m_eCurrentState == SKELETON_STATE::WANDER || (m_eCurrentState == SKELETON_STATE::CHASE_PLAYER && !m_bBowAndArrow));
        float toRotYRadDiff = m_fTargetYaw - m_fRootRotRadY;

        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

        float fTargetHeadY = 0.f;

        if (m_bBowAndArrow)
        {
            // [추가] 조준 중일 때도 몸통이 타겟을 향해 기민하게 회전해야 합니다.
            m_fRootRotRadY += toRotYRadDiff * 10.f * fTimeDelta;
            fTargetHeadY = 0.f; // 머리는 몸통 정면 고정 (팔이 조준선을 정렬하므로)
        }
        else if (bIsMoving)
        {
            m_fRootRotRadY += toRotYRadDiff * 8.f * fTimeDelta;
            fTargetHeadY = 0.f;
        }
        else // IDLE 상태 시선 분리
        {
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

        m_fHeadRotRadY += (fTargetHeadY - m_fHeadRotRadY) * 10.f * fTimeDelta;
        m_fHeadRotRadX += (m_fTargetPitch - m_fHeadRotRadX) * 5.f * fTimeDelta;

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


    // -------------------------------------------------------------
    // 4. 포즈(Pose) 계산 및 블렌딩(Blending) 처리
    // -------------------------------------------------------------

    // [A. 기본 Pose 생성] 평소 Move + Bob 성분
    float fBobDegreeArg = m_fLifeTime * 103.2f;
    float fCosValue = std::cos(XMConvertToRadians(fBobDegreeArg));
    float fRightArmBobZ = (fCosValue * 2.865f) + 2.865f;
    float fLeftArmBobZ = fRightArmBobZ * -1.0f;

    float fTcos0 = std::cos(m_fWalkTime) * 30.0f * m_fWalkWeight;

    XMFLOAT3 normalLeftArm = { fTcos0, 0.0f, fLeftArmBobZ };
    XMFLOAT3 normalRightArm = { -fTcos0, 0.0f, fRightArmBobZ };
    XMFLOAT3 normalLeftLeg = { fTcos0 * -1.4f, -0.1f, -0.1f };
    XMFLOAT3 normalRightLeg = { fTcos0 * 1.4f, 0.1f, 0.1f };

    // [B. 활 조준 Pose 생성] 화살 겨누기 + 미세한 흔들림 성분
    float targetRotX = XMConvertToDegrees(m_fTargetPitch);
    float targetRotY = XMConvertToDegrees(m_fTargetYaw - m_fRootRotRadY);

    auto sinDeg = [](float deg) { return std::sin(XMConvertToRadians(deg)); };
    auto cosDeg = [](float deg) { return std::cos(XMConvertToRadians(deg)); };
    float bowSway1 = sinDeg(m_fLifeTime * 76.8f) * 2.865f;
    float bowSway2 = cosDeg(m_fLifeTime * 103.2f) * 2.865f;

    XMFLOAT3 bowLeftArm = { targetRotX - 90.0f - bowSway1, targetRotY + 28.65f, -bowSway2 - 2.865f };
    XMFLOAT3 bowRightArm = { targetRotX - 90.0f + bowSway1, targetRotY - 5.73f, bowSway2 + 2.865f };
    XMFLOAT3 bowLeftLeg = { 0.0f, -0.1f, -0.1f }; // 조준 시 다리는 차렷 자세가 목표
    XMFLOAT3 bowRightLeg = { 0.0f, 0.1f, 0.1f };

    // [C. 두 포즈 사이를 선형 보간(Lerp)]
    auto LerpPose = [](const XMFLOAT3& normal, const XMFLOAT3& bow, float weight) -> XMFLOAT3 {
        return XMFLOAT3(
            normal.x + (bow.x - normal.x) * weight,
            normal.y + (bow.y - normal.y) * weight,
            normal.z + (bow.z - normal.z) * weight
        );
        };

    XMFLOAT3 finalLeftArm = LerpPose(normalLeftArm, bowLeftArm, m_fBowWeight);
    XMFLOAT3 finalRightArm = LerpPose(normalRightArm, bowRightArm, m_fBowWeight);
    XMFLOAT3 finalLeftLeg = LerpPose(normalLeftLeg, bowLeftLeg, m_fBowWeight);
    XMFLOAT3 finalRightLeg = LerpPose(normalRightLeg, bowRightLeg, m_fBowWeight);


    // -------------------------------------------------------------
    // 5. 최종 계산된 보간 각도를 Bone에 적용
    // -------------------------------------------------------------
    if (pLeftArm)  pLeftArm->SetRotation({ XMConvertToRadians(finalLeftArm.x), XMConvertToRadians(finalLeftArm.y), XMConvertToRadians(finalLeftArm.z) });
    if (pRightArm) pRightArm->SetRotation({ XMConvertToRadians(finalRightArm.x), XMConvertToRadians(finalRightArm.y), XMConvertToRadians(finalRightArm.z) });
    if (pLeftLeg)  pLeftLeg->SetRotation({ XMConvertToRadians(finalLeftLeg.x), XMConvertToRadians(finalLeftLeg.y), XMConvertToRadians(finalLeftLeg.z) });
    if (pRightLeg) pRightLeg->SetRotation({ XMConvertToRadians(finalRightLeg.x), XMConvertToRadians(finalRightLeg.y), XMConvertToRadians(finalRightLeg.z) });


    // -------------------------------------------------------------
    // 6. 행렬 업데이트 및 컬라이더 처리
    // -------------------------------------------------------------
    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

    VelocityUpdate(fTimeDelta, vWishDir);

    E::CGameInstance::Get().AddColliderGroup("Coll_SkeletonCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());
}

void CSkeletonEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();

    //if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CHandHeldItemObject>(m_hHandHeld))
    //{
    //    auto pBoneRightItem = m_pComEntityModel->GetBone("rightItem");
    //    auto wordMat = GetTransform().GetLoadedWorldMatrix();
    //    //pObj->GetTransform().SetParentWorldMatrix(wordMat);

    //    XMMATRIX matFinalParent = wordMat;

    //    _float4x4 matParent;
    //    XMStoreFloat4x4(&matParent, matFinalParent);
    //    pObj->GetTransform().SetParentWorldMatrix(matParent);
    //}

    if (auto pItem = CGameInstance::Get().GetGameObjectByHandleT<CHandHeldItemObject>(m_hHandHeld))
    {
        auto pRightItem = m_pComEntityModel->GetBone("rightItem");
        auto pos = GetTransform().GetPosition();

        // 1. 기존 로컬 행렬 가져옴
        _matrix matLocal = XMLoadFloat4x4(pRightItem->GetCombinedTransformationMatrix());

        
        _matrix matRotateY =  XMMatrixRotationZ(XMConvertToRadians(45.0f))
            * XMMatrixRotationY(XMConvertToRadians(90.0f))
            * XMMatrixRotationX(XMConvertToRadians(90.0f))
            * XMMatrixTranslation(-0.0625f, -0.1f, 0.1f);
        _matrix matResult = matRotateY * matLocal;

        // 4. 위치 성분에 오프셋 더하기
        matResult.r[3] = XMVectorAdd(matResult.r[3], XMLoadFloat3(&pos)); // 월드 위치 적용

        _float4x4 asdf;
        XMStoreFloat4x4(&asdf, matResult);
        pItem->GetTransform().SetParentWorldMatrix(asdf);
    }
}

HRESULT CSkeletonEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        E::CB_PER_OBJECT cbPerObject{};
        cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

        if (m_eCurrentState == SKELETON_STATE::DIE)
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

void CSkeletonEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
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

//void CSkeletonEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
//{
//}

void CSkeletonEntity::TakeDamage(uint32_t iDamage)
{
    if (m_eCurrentState == SKELETON_STATE::DIE) return; // 이미 죽었거나 피격 쿨타임 중이면 무시

    m_bIsHit = true;
    m_fHitTimer = 0.3f; // 0.3초간 빨갛게 물듦

    // 마크 고증: 맞으면 살짝 위+바깥으로 팅겨나가는 넉백 추가
    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);
    vVel = XMVectorSetY(vVel, 5.0f); // 수직 점프 넉백
    XMStoreFloat3(&m_vVelocity, vVel);

    m_bOnGround = false;

    m_iHeart -= iDamage;

    if (m_iHeart <= 0)
    {
        m_eCurrentState = SKELETON_STATE::DIE;
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

void CSkeletonEntity::ProcessDestroy(_float fTimeDelta)
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

UPtr<CSkeletonEntity> CSkeletonEntity::Create()
{
    auto pInstance = ToUPtr(new CSkeletonEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CSkeletonEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CSkeletonEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CSkeletonEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CSkeletonEntity");
        return nullptr;
    }

    return pInstance;
}
