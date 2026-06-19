#include "ComAnimator.h"
#include "ComEntityModel.h"
#include "GameObject.h"


NS_USING(Engine)

CComAnimator::CComAnimator()
{
}

CComAnimator::~CComAnimator()
{
}

void CComAnimator::QuadrupedWalk(_float fTimeDelta)
{        
    auto pLeg0Bone = m_pComEntityModel->GetBone("leg0");
    auto pLeg1Bone = m_pComEntityModel->GetBone("leg1");
    auto pLeg2Bone = m_pComEntityModel->GetBone("leg2");
    auto pLeg3Bone = m_pComEntityModel->GetBone("leg3");
    if (!(pLeg0Bone && pLeg1Bone && pLeg2Bone && pLeg3Bone))
    {
        return;
    }
    m_fElapsed += fTimeDelta;

    pLeg0Bone->SetRotation({ XMConvertToRadians(cosf(m_fElapsed * 38.17f) * 80.f) , 0, 0});
    pLeg3Bone->SetRotation({ XMConvertToRadians(cosf(m_fElapsed * 38.17f) * 80.f) , 0, 0 });

    pLeg1Bone->SetRotation({ XMConvertToRadians(cosf(m_fElapsed * 38.17f) * -80.f) , 0, 0 });
    pLeg2Bone->SetRotation({ XMConvertToRadians(cosf(m_fElapsed * 38.17f) * -80.f) , 0, 0 });
}

void CComAnimator::HeadLookAt(_float fTimeDelta, _float3 vTarget)
{

    //auto pHeadBone = m_pComEntityModel->GetBone("head");
    //if (!pHeadBone) return;

    //// =================================================================
    //// 1. 공간 변환 (Space Transformation): 월드 타겟 방향 -> 머리 부모의 로컬 공간으로
    //// =================================================================
    //int32_t iParentIdx = pHeadBone->GetParentIndex();

    //_matrix matEntityWorld = GetGameObject()->GetTransform().GetLoadedWorldMatrix();
    //_matrix matParentWorld = matEntityWorld; // 부모 본이 없다면 오브젝트 월드가 곧 부모 공간

    //if (iParentIdx >= 0)
    //{
    //    // 부모 본이 있다면: [부모의 Combined 행렬] * [오브젝트 월드 행렬] = 부모의 완벽한 월드 행렬
    //    auto& pParentBone = m_pComEntityModel->GetBones()[iParentIdx];
    //    matParentWorld = XMLoadFloat4x4(pParentBone.GetCombinedTransformationMatrix()) * matEntityWorld;
    //}

    //// 머리의 실제 월드 위치와 타겟 월드 위치를 이용해 월드 방향 벡터 계산
    //XMVECTOR vHeadWorldPos = XMVector3TransformCoord(XMLoadFloat3(&pHeadBone->GetPivot()), matEntityWorld);
    //XMVECTOR vDirWorld = XMVector3Normalize(XMLoadFloat3(&vTarget) - vHeadWorldPos);

    //// 월드 방향 벡터를 '머리의 부모 월드 공간'의 역행렬로 변환하여 완벽한 로컬 방향 벡터 추출
    //_matrix matParentWorldInverse = XMMatrixInverse(nullptr, matParentWorld);
    //XMVECTOR vDirLocal = XMVector3Normalize(XMVector3TransformNormal(vDirWorld, matParentWorldInverse));


    //// =================================================================
    //// 2. 기하학적 각도 추출 및 목 꺾임 제한 (Clamp Angle via Pure Vector)
    //// =================================================================
    //// 타겟 로컬 방향 벡터(vDirLocal)를 삼각함수 연산을 위해 구조체로 분리
    //_float3 vLocal;
    //XMStoreFloat3(&vLocal, vDirLocal);

    ////  [위아래 Pitch 각도 제한] 
    //// 로컬 Y(높이)와 Z(깊이)의 비율로 탄젠트 각을 구합니다.
    //// 앞에 마이너스(-)를 붙여 정면/측면 모든 상태에서 상하 청개구리 뒤집힘 버그를 완벽하게 박멸합니다.
    //float fPitchAngle = -atan2f(vLocal.y, vLocal.z);
    //fPitchAngle = std::clamp(fPitchAngle, XMConvertToRadians(-30.f), XMConvertToRadians(30.f)); // 위아래 30도 제한

    ////  [좌우 Yaw 각도 제한]
    //// 로컬 X(좌우)와 Z(깊이)의 비율로 탄젠트 각을 구합니다.
    //float fYawAngle = atan2f(vLocal.x, vLocal.z);
    //fYawAngle = std::clamp(fYawAngle, XMConvertToRadians(-55.f), XMConvertToRadians(55.f)); // 좌우 55도 제한

    //// 제한된 각도(라디안)들을 가지고 꼬임 없는 깨끗한 목표 쿼터니언 빌드
    //XMVECTOR qTargetRot = XMQuaternionRotationRollPitchYaw(fPitchAngle, fYawAngle, 0.f);

    ////  [안전장치] 첫 프레임이나 순간적인 NaN(유효하지 않은 값) 방출 차단
    //if (XMVector4IsNaN(qTargetRot)) return;


    //// =================================================================
    //// 3. 회전 속도 보간 (Slerp) 및 최종 행렬 조립
    //// =================================================================
    //XMVECTOR qCurrentRot = XMLoadFloat4(&m_vCurrentHeadRotQuat);

    //// 첫 프레임에 현재 쿼터니언이 완전히 비어있다면 Identity(기본값)로 리셋
    //if (XMVector4Equal(qCurrentRot, XMVectorZero()))
    //{
    //    qCurrentRot = XMQuaternionIdentity();
    //}

    //// 현재 회전에서 목표 회전으로 선형 보간 (m_fHeadTurnSpeed 속도로 부드럽게 고개 회전)
    //XMVECTOR qBlendedRot = XMQuaternionSlerp(qCurrentRot, qTargetRot, fTimeDelta * m_fHeadTurnSpeed);

    //// 다음 프레임을 위해 누적된 회전 상태 저장
    //XMStoreFloat4(&m_vCurrentHeadRotQuat, qBlendedRot);

    //// 최종 보간된 쿼터니언을 회전 행렬로 변환
    //_matrix matFinalRot = XMMatrixRotationQuaternion(qBlendedRot);

    //// 머리 본 고유의 크기(Scale)와 목 위치(Translation) 유실 방지를 위한 최종 결합
    //XMMATRIX matScale = XMMatrixScaling(pHeadBone->GetScale()->x, pHeadBone->GetScale()->y, pHeadBone->GetScale()->z);
    //XMMATRIX matTrans = XMMatrixTranslation(pHeadBone->GetTranslatoin()->x, pHeadBone->GetTranslatoin()->y, pHeadBone->GetTranslatoin()->z);

    //// [크기 * 부드러운 제한 회전 * 위치]
    //XMMATRIX matFinalLocal = matScale * matFinalRot * matTrans;

    //// 본의 로컬 트랜스폼 매트릭스 오버라이드 업데이트 완료!
    //pHeadBone->UpdateTransformationMatrix(matFinalLocal);


    //auto pHeadBone = m_pComEntityModel->GetBone("head");
    //if (!pHeadBone) return;

    //// 1 ~ 4번 단계는 기존 공간 변환 로직 그대로 유지 (vDirLocal 구하기)
    //int32_t iParentIdx = pHeadBone->GetParentIndex();
    //_matrix matEntityWorld = GetGameObject()->GetTransform().GetLoadedWorldMatrix();
    //_matrix matParentWorld = matEntityWorld;
    //if (iParentIdx >= 0) {
    //    auto& pParentBone = m_pComEntityModel->GetBones()[iParentIdx];
    //    matParentWorld = XMLoadFloat4x4(pParentBone.GetCombinedTransformationMatrix()) * matEntityWorld;
    //}
    //XMVECTOR vHeadWorldPos = XMVector3TransformCoord(XMLoadFloat3(&pHeadBone->GetPivot()), matEntityWorld);
    //XMVECTOR vDirWorld = XMVector3Normalize(XMLoadFloat3(&vTarget) - vHeadWorldPos);
    //_matrix matParentWorldInverse = XMMatrixInverse(nullptr, matParentWorld);
    //XMVECTOR vDirLocal = XMVector3Normalize(XMVector3TransformNormal(vDirWorld, matParentWorldInverse));

    //// -------------------------------------------------------------
    //// 5. 완벽한 목표 로컬 회전 행렬 계산 (LookTo 함수 활용)
    //_matrix matTargetLookAt = XMMatrixLookToLH(XMVectorSet(0.f, 0.f, 0.f, 0.f), vDirLocal, XMVectorSet(0.f, 1.f, 0.f, 0.f));
    //_matrix matTargetRot = XMMatrixInverse(nullptr, matTargetLookAt);
    //XMVECTOR qTargetRot = XMQuaternionRotationMatrix(matTargetRot);

    //if (XMVector4IsNaN(qTargetRot)) return;

    //// =================================================================
    ////  [새로운 각도 제한 방식] 오일러 공식 없이 완벽하게 제한하기
    //// =================================================================

    //// 1. 타겟 로컬 방향 벡터(vDirLocal)를 평면별로 분리합니다.
    //_float3 vLocal;
    //XMStoreFloat3(&vLocal, vDirLocal);

    //// 2. 위아래(Pitch) 각도 제한: 로컬 Z와 Y 평면 기준
    //// 정면(0, 0, 1)을 바라볼 때 위아래 사잇각을 구합니다.
    //float fPitchAngle = atan2f(vLocal.y, vLocal.z); // 위를 보면 +, 아래를 보면 - 가 수학적으로 정확히 나옵니다.
    //fPitchAngle = std::clamp(fPitchAngle, XMConvertToRadians(-30.f), XMConvertToRadians(30.f));

    //// 3. 좌우(Yaw) 각도 제한: 로컬 Z와 X 평면 기준
    //float fYawAngle = atan2f(vLocal.x, vLocal.z);
    //fYawAngle = std::clamp(fYawAngle, XMConvertToRadians(-55.f), XMConvertToRadians(55.f));

    //// 4. 제한된 각도로 깨끗한 목표 쿼터니언 빌드 (오일러 순서 꼬임 방지)
    //// DirectXMath의 정석 RollPitchYaw 함수를 사용하여 다시 조립합니다.
    //qTargetRot = XMQuaternionRotationRollPitchYaw(fPitchAngle, fYawAngle, 0.f);

    //// =================================================================

    //// 7. 기존 보간(Slerp) 영역 및 행렬 조립으로 이어짐
    //XMVECTOR qCurrentRot = XMLoadFloat4(&m_vCurrentHeadRotQuat);
    //if (XMVector4Equal(qCurrentRot, XMVectorZero()))
    //{
    //    qCurrentRot = XMQuaternionIdentity();
    //}

    //XMVECTOR qBlendedRot = XMQuaternionSlerp(qCurrentRot, qTargetRot, fTimeDelta * m_fHeadTurnSpeed);
    //XMStoreFloat4(&m_vCurrentHeadRotQuat, qBlendedRot);

    //_matrix matFinalRot = XMMatrixRotationQuaternion(qBlendedRot);

    //XMMATRIX matScale = XMMatrixScaling(pHeadBone->GetScale()->x, pHeadBone->GetScale()->y, pHeadBone->GetScale()->z);
    //XMMATRIX matTrans = XMMatrixTranslation(pHeadBone->GetTranslatoin()->x, pHeadBone->GetTranslatoin()->y, pHeadBone->GetTranslatoin()->z);

    //XMMATRIX matFinalLocal = matScale * matFinalRot * matTrans;

    //pHeadBone->UpdateTransformationMatrix(matFinalLocal);


    //auto pHeadBone = m_pComEntityModel->GetBone("head");
    //if (!pHeadBone) return;

    //// 1. 머리 본의 부모 본 인덱스를 가져옵니다.
    //int32_t iParentIdx = pHeadBone->GetParentIndex();

    //// 2. 머리가 속한 '최종 부모의 월드 행렬'을 구합니다.
    //_matrix matEntityWorld = GetGameObject()->GetTransform().GetLoadedWorldMatrix();
    //_matrix matParentWorld = matEntityWorld; // 부모 본이 없다면 오브젝트 월드가 곧 부모 공간

    //
    //if (iParentIdx >= 0)
    //{
    //    // 부모 본이 있다면: [부모의 Combined 행렬] * [오브젝트 월드 행렬] = 부모의 완벽한 월드 행렬
    //    auto& pParentBone = m_pComEntityModel->GetBones()[iParentIdx]; // 인덱스로 본 가져오는 함수 필요
    //    matParentWorld = XMLoadFloat4x4(pParentBone.GetCombinedTransformationMatrix()) * matEntityWorld;
    //}

    //// 3. 머리의 실제 월드 위치 구하기
    //XMVECTOR vHeadWorldPos = XMVector3TransformCoord(XMLoadFloat3(&pHeadBone->GetPivot()), matEntityWorld);
    //XMVECTOR vTargetWorldPos = XMLoadFloat3(&vTarget);
    //XMVECTOR vDirWorld = XMVector3Normalize(vTargetWorldPos - vHeadWorldPos);

    //// 4.  [이 문제의 정답] 월드 방향 벡터를 '머리의 부모 월드 공간'의 역행렬로 변환!!
    //_matrix matParentWorldInverse = XMMatrixInverse(nullptr, matParentWorld);
    //XMVECTOR vDirLocal = XMVector3Normalize(XMVector3TransformNormal(vDirWorld, matParentWorldInverse));

    //// 5. 이제 안전해진 vDirLocal 기반으로 Right, Up, Look 빌드
    //_vector vLook = vDirLocal;
    //_vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    //_vector vRight = XMVector3Normalize(XMVector3Cross(worldUp, vLook));
    //_vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

    //XMFLOAT4X4 m;
    //XMStoreFloat4x4(&m, XMMatrixIdentity());
    //XMStoreFloat3((_float3*)&m.m[0][0], vRight);
    //XMStoreFloat3((_float3*)&m.m[1][0], vUp);
    //XMStoreFloat3((_float3*)&m.m[2][0], vLook);





    //// 6. 오일러 추출 및 적용 (기존과 동일)
    //_float3 vEuler{};
    //if (fabsf(m.m[0][2]) < 0.99999f) {
    //    vEuler.y = atan2f(m.m[0][2], m.m[2][2]);
    //    vEuler.x = asinf(-m.m[1][2]);
    //}

    //// 각도 제한 후 주입
    //vEuler.x = std::clamp(vEuler.x, XMConvertToRadians(-30.f), XMConvertToRadians(30.f));
    //vEuler.y = std::clamp(vEuler.y, XMConvertToRadians(-60.f), XMConvertToRadians(60.f));
    //vEuler.z = 0.f;

    //pHeadBone->UpdateTransformationMatrix(XMLoadFloat4x4(&m));


    //auto pHeadBone = m_pComEntityModel->GetBone("head");
    //if (!pHeadBone)
    //{
    //    return;
    //}

    //;
    //XMLoadFloat3(&pHeadBone->GetLocalPivot());
    ////auto vHeadPosWorld = XMVector3TransformCoord(XMLoadFloat3(&pHeadBone->GetLocalPivot()), GetGameObject()->GetTransform().GetLoadedCombinedWorldMatrix());

    ////auto worldMat = GetGameObject()->GetTransform().GetLoadedCombinedWorldMatrix();

    //XMMATRIX OffsetFromParentPivot = XMMatrixTranslationFromVector(XMLoadFloat3(&pHeadBone->GetLocalPivot()));

    //auto vHeadPosWorld = GetGameObject()->GetTransform().GetLoadedPostion();
    //auto myPos = GetGameObject()->GetTransform().GetLoadedPostion();
    ////    entityWorld);
    //XMVECTOR vDir = XMVector3Normalize(XMLoadFloat3(&vTarget) - vHeadPosWorld);
    //{
    //    _vector vLook = vDir;
    //    _vector worldUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    //    _vector vRight = XMVector3Normalize(XMVector3Cross(worldUp, vLook));
    //    _vector vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
    //    _matrix matRot = XMMatrixIdentity();
    //    matRot.r[0] = XMVectorSetW(vRight, 0.f);
    //    matRot.r[1] = XMVectorSetW(vUp, 0.f);
    //    matRot.r[2] = XMVectorSetW(vLook, 0.f);

    //    pHeadBone->UpdateTransformationMatrix(matRot);
    //}
}

void CComAnimator::SetSpiderDefaultLegPose(_float fTimeDelta)
{
    // 8개의 모든 다리 본을 가져옵니다.
    auto pLeg0 = m_pComEntityModel->GetBone("leg0");
    auto pLeg1 = m_pComEntityModel->GetBone("leg1");
    auto pLeg2 = m_pComEntityModel->GetBone("leg2");
    auto pLeg3 = m_pComEntityModel->GetBone("leg3");
    auto pLeg4 = m_pComEntityModel->GetBone("leg4");
    auto pLeg5 = m_pComEntityModel->GetBone("leg5");
    auto pLeg6 = m_pComEntityModel->GetBone("leg6");
    auto pLeg7 = m_pComEntityModel->GetBone("leg7");

    if (!(pLeg0 && pLeg1 && pLeg2 && pLeg3 && pLeg4 && pLeg5 && pLeg6 && pLeg7))
        return;

    // 회전 구조: { Pitch(X), Yaw(Y), Roll(Z) }
    // JSON의 축 기준에 맞춰 방향과 각도를 변환한 고정 포즈 값입니다.

    // leg0 (오른쪽 앞다리) : Y=45도, Z=-45도
    pLeg0->SetRotation({ 0.f, XMConvertToRadians(45.0f),  XMConvertToRadians(-45.0f) });
    // leg1 (왼쪽 앞다리)   : Y=-45도, Z=45도
    pLeg1->SetRotation({ 0.f, XMConvertToRadians(-45.0f), XMConvertToRadians(45.0f) });

    // leg2 (오른쪽 중앞)   : Y=22.5도, Z=-33.3도
    pLeg2->SetRotation({ 0.f, XMConvertToRadians(22.5f),  XMConvertToRadians(-33.3f) });
    // leg3 (왼쪽 중앞)     : Y=-22.5도, Z=33.3도
    pLeg3->SetRotation({ 0.f, XMConvertToRadians(-22.5f), XMConvertToRadians(33.3f) });

    // leg4 (오른쪽 중뒤)   : Y=-22.5도, Z=-33.3도
    pLeg4->SetRotation({ 0.f, XMConvertToRadians(-22.5f), XMConvertToRadians(-33.3f) });
    // leg5 (왼쪽 중뒤)     : Y=22.5도, Z=33.3도
    pLeg5->SetRotation({ 0.f, XMConvertToRadians(22.5f),  XMConvertToRadians(33.3f) });

    // leg6 (오른쪽 뒷다리) : Y=-45도, Z=-45도
    pLeg6->SetRotation({ 0.f, XMConvertToRadians(-45.0f), XMConvertToRadians(-45.0f) });
    // leg7 (왼쪽 뒷다리)   : Y=45도, Z=45도
    pLeg7->SetRotation({ 0.f, XMConvertToRadians(45.0f),  XMConvertToRadians(45.0f) });
}

void CComAnimator::SetEnderManBasePose(_float fTimeDelta)
{
    auto pBody = m_pComEntityModel->GetBone("body");
    auto pHead = m_pComEntityModel->GetBone("head");
    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    auto pRightArm = m_pComEntityModel->GetBone("rightArm");
    auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg");
    auto pRightLeg = m_pComEntityModel->GetBone("rightLeg");

    if (!(pBody && pHead && pLeftArm && pRightArm && pLeftLeg && pRightLeg))
        return;

    const _float fPixelScale = 1.0f / 16.0f;

    // 1. 모든 본의 평행이동(Translation) 채널을 원점(0)으로 리셋합니다.
    // 지오메트리 파싱 단계에서 이미 완벽한 높이와 위치로 뼈대가 조립되어 있기 때문입니다.
    pBody->SetTranslation({ 0.0f, 0.0f, 0.0f });
    pHead->SetTranslation({ 0.0f, 0.0f, 0.0f });
    pLeftArm->SetTranslation({ 0.0f, 0.0f, 0.0f });
    pLeftLeg->SetTranslation({ 0.0f, 0.0f, 0.0f });
    pRightLeg->SetTranslation({ 0.0f, 0.0f, 0.0f });

    // 2. [유일한 예외] 오른팔(rightArm) 보정
    // RightToLeft()의 반전 연산 때문에 오른팔 피벗이 몸통 안쪽(+3.0)으로 파고들었습니다.
    // 이를 왼팔(-5.0)과 완벽한 대칭점인 바깥쪽(+5.0 위치)으로 밀어내기 위해 X축 오프셋을 줍니다.
    // 공식: (목표 대칭 위치 5.0 - RightToLeft로 변환된 현재 피벗 X) * fPixelScale
    _float rightArmOffsetX = (5.0f * fPixelScale) - pRightArm->GetPivot().x;

    // 오른팔만 X축으로 밀어주고 Y, Z는 원래 지오메트리를 유지(0.0)합니다.
    pRightArm->SetTranslation({ rightArmOffsetX, 0.0f, 0.0f });
}

void CComAnimator::HumanoidBob(_float fTimeDelta)
{// 양팔 본을 가져옵니다.
    auto pLeftArm = m_pComEntityModel->GetBone("leftArm");
    auto pRightArm = m_pComEntityModel->GetBone("rightArm");

    if (!(pLeftArm && pRightArm))
        return;

    // 실시간 타이머 누적 (query.life_time 대용)
    m_fElapsed += fTimeDelta;

    // [JSON 스크립트 공식 이식]
    // 103.2f는 속도 계수, 2.865f는 최대 회전 각도(도 단위)입니다.
    // 마인크래프트의 공식 연산 결과는 '도(Degree)' 단위이므로 DirectXMath에 넣기 전 라디안으로 변환해야 합니다.

    // 1. 공통 cos 값 계산
    _float cosValue = cosf(m_fElapsed * 103.2f) * 2.865f + 2.865f;

    // 2. 오른팔 (rightArm) : (cos * 2.865) + 2.865 -> Z축 정방향 회전
    _float rightDegZ = cosValue;

    // 3. 왼팔 (leftArm) : ((cos * 2.865) + 2.865) * -1.0 -> Z축 역방향 회전 (좌우 대칭)
    _float leftDegZ = cosValue * -1.0f;

    // 변환 행렬 구조에 주입 (Pitch=0, Yaw=0, Roll=Z축회전)
    // 기존에 BasePose나 다른 애니메이션에서 지정한 Pitch/Yaw 회전이 있다면 
    // GetRotation()을 받아와서 Z값만 변경(+= 또는 대입)해주는 구조가 안전합니다.

    // 여기서는 밥 애니메이션 고유의 로컬 회전만 대입하는 기준입니다.
    pLeftArm->SetRotation({ 0.0f, 0.0f, XMConvertToRadians(leftDegZ) });
    pRightArm->SetRotation({ 0.0f, 0.0f, XMConvertToRadians(rightDegZ) });
}

HRESULT CComAnimator::Initialize(void* pArg)
{
    auto pDesc = static_cast<DESC*>(pArg);
    if (!pDesc->pComEntityModel)  assert(false);

    m_pComEntityModel = pDesc->pComEntityModel;
    if (FAILED(CComponent::Initialize(pArg)))
    {
        return E_FAIL;
    }

    
    return S_OK;
}

UPtr<CComAnimator>  CComAnimator::Create()
{
    auto pInstance = ToUPtr(new CComAnimator{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Created : CComAnimator");
        return nullptr;
    }
    return pInstance;
}

UPtr<CPrototype> CComAnimator::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CComAnimator{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CComAnimator");
        return nullptr;
    }
    return pInstance;
}
