#include "ChickenEntity.h"


#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"
#include "ComAnimator.h"
#include "ComConstantBuffer.h"
#include "CameraObject.h"

#include "ExperienceOrb.h"

#include "ItemObject.h"

#include "CollBox.h"
NS_USING(Engine)

CChickenEntity::CChickenEntity()
{
}

CChickenEntity::CChickenEntity(const CChickenEntity& rhs)
    : CAnimalEntityObject{rhs}
{

}

CChickenEntity::~CChickenEntity()
{
}
static float fTmp = 0.f;
void CChickenEntity::UpdateGUI()
{
    CAnimalEntityObject::UpdateGUI();

    ImGui::DragFloat("Tmp", &fTmp, 0.01, -10.f, 10.f);
}

HRESULT CChickenEntity::Initialize(void* pArg)
{
    if (FAILED(CAnimalEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Chicken" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Chicken" };
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

    m_pCenterCollider = CCollBox::Create({ 0.f, 0.5f, 0.f }, { 0.25f, 0.5f, 0.25f });
    m_pCenterCollider->SetInnerPointer(this);
    return S_OK;
}

void CChickenEntity::PriorityUpdate(E::_float fTimeDelta)
{
}

void CChickenEntity::Update(E::_float fTimeDelta)
{
    m_fStateTimer -= fTimeDelta;
    XMVECTOR vWishDir = XMVectorZero();
    auto pPlayer = CGameInstance::Get().GetGameObjectByHandle(m_hPlayer);

    _float fPlayerDetectDist = 4.f;

    auto playerpos = pPlayer->GetTransform().GetPosition();
    playerpos.y += 1.5f;

    auto chickenpos = GetTransform().GetPosition();
    chickenpos.y += 1.325f;

    XMVECTOR vChickenPos = XMLoadFloat3(&chickenpos);
    XMVECTOR vPlayerPos = pPlayer ? XMLoadFloat3(&playerpos) : XMVectorZero();

    auto vDir = XMVector3Normalize(vPlayerPos - vChickenPos);

    auto pLeg0 = m_pComEntityModel->GetBone("leg0"); // 뒤쪽 오른쪽 다리
    auto pLeg1 = m_pComEntityModel->GetBone("leg1"); // 뒤쪽 왼쪽 다리
    auto pHead = m_pComEntityModel->GetBone("head");
    auto pBody = m_pComEntityModel->GetBone("body");


    if (m_eCurrentState == CHICKEN_STATE::DIE)
    {
        ProcessDropDropItem(fTimeDelta);

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

        E::CGameInstance::Get().AddColliderGroup("Coll_ChickenCenter", m_pCenterCollider.get());
        m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

        if (m_fDeathTimer >= TOTAL_DURATION)
            ProcessDestroy(fTimeDelta);
        return;
    }

    // 1. 매 프레임 애니메이션 및 본 채널 초기화
    m_pComEntityModel->ResetBonesChannel();

    // 2. 피격 타이머 업데이트 (피격 시 붉어지는 필터 컬러 처리)
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
            m_eCurrentState = CHICKEN_STATE::DIE;
            return;
        }
    }

    // 3. AI 상태 결정 및 가고 싶은 방향(vWishDir) 계산 변수들

    _float fDistToPlayer = pPlayer ? XMVectorGetX(XMVector3Length(vPlayerPos - vChickenPos)) : 999.f;

    m_fStateTimer -= fTimeDelta;

    // 강제 상태 전환: 피격당하면 무조건 도망(FLEE) 상태로 전환
    if (m_bIsHit && m_eCurrentState != CHICKEN_STATE::FLEE)
    {
        m_eCurrentState = CHICKEN_STATE::FLEE;
        m_fStateTimer = 3.0f; // 3초간 미친 듯이 도망
    }

    // 4. 상태 머신 (FSM) 패턴 구동
    switch (m_eCurrentState)
    {
    case CHICKEN_STATE::IDLE:
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
            m_eCurrentState = CHICKEN_STATE::LOOK_AT_PLAYER;
        }

        // 타이머 끝나면 무작위로 걷기 시작
        if (m_fStateTimer <= 0.f) {
            m_eCurrentState = CHICKEN_STATE::WANDER;
            m_fStateTimer = 2.f + (rand() % 40) * 0.1f;
            m_fTargetYaw = (rand() % 360) * (XM_PI / 180.f); // 걸어갈 새로운 무작위 방향
            m_fTargetPitch = 0.f; // 걸을 때는 정면 응시
        }
        break;

    case CHICKEN_STATE::WANDER:
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
            m_eCurrentState = CHICKEN_STATE::LOOK_AT_PLAYER;
        }
        else if (m_fStateTimer <= 0.f) {
            m_eCurrentState = CHICKEN_STATE::IDLE;
            m_fStateTimer = 1.f + (rand() % 30) * 0.1f; // 1~4초 쉬기
        }
        break;

    case CHICKEN_STATE::LOOK_AT_PLAYER:
        if (m_bIsHit)
        {
            break;
        }
        m_fSpeed = 0.f;
        if (pPlayer)
        {
            // 플레이어를 바라보는 TargetYaw 역산
            XMVECTOR vToPlayer = XMVector3Normalize(XMVectorSetY(vPlayerPos - vChickenPos, 0.f));
            m_fTargetYaw = atan2f(XMVectorGetX(vToPlayer), XMVectorGetZ(vToPlayer));

            //m_pComAnimator->HeadLookAt(fTimeDelta, pPlayer->GetTransform().GetPosition());
        }

        if (fDistToPlayer > 5.0f || m_fStateTimer <= 0.f) {
            m_eCurrentState = CHICKEN_STATE::IDLE;
            m_fStateTimer = 1.0f;
        }
        break;

    case CHICKEN_STATE::FLEE:
        if (m_bIsHit)
        {
            break;
        }
        m_fSpeed = 4.5f; // 겁먹고 뛰어가는 빠른 속도 (마크 고증)
        if (pPlayer)
        {
            // 플레이어 반대 방향(도망 방향) 벡터 구하기
            XMVECTOR vFleeDir = XMVector3Normalize(XMVectorSetY(vChickenPos - vPlayerPos, 0.f));
            m_fTargetYaw = atan2f(XMVectorGetX(vFleeDir), XMVectorGetZ(vFleeDir));
            vWishDir = vFleeDir;
        }
        else {
            vWishDir = XMVectorSet(sinf(m_fTargetYaw), 0.f, cosf(m_fTargetYaw), 0.f);
        }

        // 걷기 애니메이션 배속 재생 (뛰는 것처럼 보이게)
        m_pComAnimator->QuadrupedWalk(fTimeDelta * 0.3f);

        if (m_fStateTimer <= 0.f) {
            m_eCurrentState = CHICKEN_STATE::IDLE;
            m_fStateTimer = 2.0f;
        }
        break;
    }

    // 5.  몸통(Root) 회전 보간 처리 (탁탁 끊기지 않고 부드럽게 싹 돌게 만듬)
    // 현재 각도에서 목표 각도로 매 프레임 조금씩 Lerp 회전
    float fAngleDiff = m_fTargetYaw - m_fRootRotRadY;
    // 고각 오차 보정 (-PI ~ PI 사이로 각도 범위 래핑)
    while (fAngleDiff < -XM_PI) fAngleDiff += XM_2PI;
    while (fAngleDiff > XM_PI) fAngleDiff -= XM_2PI;
    m_fRootRotRadY += fAngleDiff * 8.f * fTimeDelta;

    auto pRoot = m_pComEntityModel->GetBone("body");
    pRoot->GetRotation()->y = m_fRootRotRadY;

    if (XMVectorGetX(XMVector3Length(vWishDir)) > 0.01f)
    {
        vWishDir = XMVector3Normalize(vWishDir);
    }
    else
    {
        vWishDir = XMVectorZero();
    }

    {
        float fMaxLegAngle = 35.0f; // 최대 앞뒤 회전 각도 (도 단위)
        float fLegRot = std::sin(m_fWalkTime) * fMaxLegAngle * m_fWalkWeight;

        float legRot0 = fLegRot;
        float legRot1 = -fLegRot;

        if (pLeg0) pLeg0->SetRotation({ XMConvertToRadians(legRot0), 0.f, 0.f });
        if (pLeg1) pLeg1->SetRotation({ XMConvertToRadians(legRot1), 0.f, 0.f });
    }




    // 7. 계층 구조 변환 매트릭스 빌드
    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

    //  계산된 정밀한 의지 방향(vWishDir)을 물리 가속 연산 장치로 패스!
    VelocityUpdate(fTimeDelta, vWishDir);

    // 8. 충돌 그룹 등록 및 트랜스폼 동기화
    E::CGameInstance::Get().AddColliderGroup("Coll_ChickenCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());
}

void CChickenEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    GetTransform().Update();
}

HRESULT CChickenEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        E::CB_PER_OBJECT cbPerObject{};
        cbPerObject.matWorld = *GetTransform().GetCombinedWorldMatrix();
        XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedCombinedWorldMatrix() * ctx.matViewProj);

        //  피격 플래그 컬러필터 조립
        if (m_eCurrentState == CHICKEN_STATE::DIE)
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

void CChickenEntity::VelocityUpdate(E::_float fTimeDelta, _fvector vWishDir)
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
    const XMFLOAT3 halfExtents = { 0.25f, 0.5f, 0.25f };
    XMFLOAT3 pos = GetTransform().GetPosition();

    _float fAddY = 0.49f + fTmp;
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



void CChickenEntity::TakeDamage(uint32_t iDamage, _vector vAttackerPos)
{
    if (m_eCurrentState == CHICKEN_STATE::DIE) return; // 이미 죽었거나 피격 쿨타임 중이면 무시
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
        m_eCurrentState = CHICKEN_STATE::DIE;
        m_fDeathTimer = 0.f;

        m_fTargetYaw = m_fRootRotRadY;

        auto pHeadBone = m_pComEntityModel->GetBone("head");
        if (pHeadBone)
        {
            pHeadBone->SetRotation({ 0.f, 0.f, 0.f });
        }
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
            // 랜덤 재생
            const char* sounds[] = { "CHICKEN_HURT_1", "CHICKEN_HURT_2" };
            CGameInstance::Get().SoundPlay(sounds[RandInt(0, 1)], fVol);
        }
    }

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
        // 랜덤 재생
        const char* sounds[] = { "CHICKEN_HURT_1", "CHICKEN_HURT_2"};
        CGameInstance::Get().SoundPlay(sounds[RandInt(0, 1)], fVol);
    }
}

void CChickenEntity::ProcessDropDropItem(_float fTimeDelta)
{
}

void CChickenEntity::ProcessDestroy(_float fTimeDelta)
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
    info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Chicken;
    info.iCnt = 1;
    _float3 startPos{ GetTransform().GetPosition() };
    startPos.y += 1.f;
    CItemObject::SpawnDropItemObject(info, startPos, { 0.f, 2.f, 0.f });
}

UPtr<CChickenEntity> CChickenEntity::Create()
{
    auto pInstance = ToUPtr(new CChickenEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CChickenEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CChickenEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CChickenEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CChickenEntity");
        return nullptr;
    }

    return pInstance;
}
