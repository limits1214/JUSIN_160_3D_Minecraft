#include "PlayerEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"

#include "CollSphere.h"
#include "CollBox.h"
NS_USING(Engine)





CPlayerEntity::CPlayerEntity()
{
}

CPlayerEntity::CPlayerEntity(const CPlayerEntity& rhs)
    : CPlayerEntityObject{rhs}
{

}

CPlayerEntity::~CPlayerEntity()
{
}

void CPlayerEntity::UpdateGUI()
{
    CPlayerEntityObject::UpdateGUI();

    if (ImGui::TreeNode("Control"))
    {
        ImGui::Text("Control: %i", m_bControl);
        ImGui::Text("KeyPressingW: %i", m_bKeyPressingW);
        ImGui::Text("KeyPressingA: %i", m_bKeyPressingA);
        ImGui::Text("KeyPressingS: %i", m_bKeyPressingS);
        ImGui::Text("KeyPressingD: %i", m_bKeyPressingD);
        ImGui::Text("KeyPressingQ: %i", m_bKeyPressingQ);
        ImGui::Text("KeyPressingE: %i", m_bKeyPressingE);
        ImGui::Text("KeyPressingShift: %i", m_bKeyPressingShift);
        ImGui::Text("KeyPressingSpace: %i", m_bKeyPressingSpace);
        ImGui::Text("MouseMoveX: %i", m_iMouseMoveX);
        ImGui::Text("MouseMoveY: %i", m_iMouseMoveY);
        ImGui::Text("MouseMoveZ: %i", m_iMouseMoveZ);
        ImGui::Text("MousePressingLeft: %i", m_bMousePressingLeft);
        ImGui::Text("MousePressingRight: %i", m_bMousePressingRight);

        ImGui::Text("NumKeyPressing_1: %i", m_iNumKeyPressing[1]);
        ImGui::Text("NumKeyPressing_2: %i", m_iNumKeyPressing[2]);
        ImGui::Text("NumKeyPressing_3: %i", m_iNumKeyPressing[3]);
        ImGui::Text("NumKeyPressing_4: %i", m_iNumKeyPressing[4]);
        ImGui::Text("NumKeyPressing_5: %i", m_iNumKeyPressing[5]);
        ImGui::Text("NumKeyPressing_6: %i", m_iNumKeyPressing[6]);
        ImGui::Text("NumKeyPressing_7: %i", m_iNumKeyPressing[7]);
        ImGui::Text("NumKeyPressing_8: %i", m_iNumKeyPressing[8]);
        ImGui::Text("NumKeyPressing_9: %i", m_iNumKeyPressing[9]);

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("CAMERATYPE"))
    {
        if (ImGui::Button("FPS"))
        {
            m_eCameraType = CAMERA_TYPE::FPS;
        }
        if (ImGui::Button("TPS"))
        {
            m_eCameraType = CAMERA_TYPE::TPS;
        }
        if (ImGui::Button("TPS_BACK"))
        {
            m_eCameraType = CAMERA_TYPE::TPS_BACK;
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("MODETYPE"))
    {
        if (ImGui::Button("GOD"))
        {
            m_eModeType = MODE_TYPE::GOD;
        }
        if (ImGui::Button("GRAVITY"))
        {
            m_eModeType = MODE_TYPE::GRAVITY;
        }

        ImGui::TreePop();
    }

    if (ImGui::Button("GetCurrPosBlock"))
    {
        _float3 pos = GetTransform().GetPosition();

        int32_t wbx = (int32_t)floorf(pos.x);
        int32_t wby = (int32_t)floorf(pos.y);
        int32_t wbz = (int32_t)floorf(pos.z);

        std::optional<CBlock3> block = CGameInstance::Get().GetVoxelBlock(wbx, wby, wbz);
        int x = 0;
    }

    if (auto pHeadBone = m_pComEntityModel->GetBone("head"))
    {
        
        ImGui::Text("head x: %f, y: %f, ", XMConvertToDegrees(pHeadBone->GetRotation()->x), XMConvertToDegrees(pHeadBone->GetRotation()->y));
    }
    if (auto pRootBone = m_pComEntityModel->GetBone("root"))
    {

        ImGui::Text("root x: %f, y: %f, ", XMConvertToDegrees(pRootBone->GetRotation()->x), XMConvertToDegrees(pRootBone->GetRotation()->y));
    }

    ImGui::Text("m_fRootRotRadY: %f", m_fRootRotRadY);

    ImGui::InputFloat3("vVelocity", (float*)&m_vVelocity, "%.3f", ImGuiInputTextFlags_ReadOnly);
}

HRESULT CPlayerEntity::Initialize(void* pArg)
{
    if (FAILED(CPlayerEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

    {
        CComEntityModel::DESC componentDesc{};
        componentDesc.pGameObject = this;
        componentDesc.viBufferId = { "MC_ENTITY_VIBuffer", "Steve" };
        componentDesc.geometryId = { "MC_ENTITY_GEOMETRY", "Steve" };
        auto pProto = CGameInstance::Get().ClonePrototype("PERMANENT", "Prototype_Component_EntityModel", &componentDesc);
        if (pProto == nullptr)
        {
            return E_FAIL;
        }
        m_pComEntityModel = AddComponent("Com_EntityModel", static_uptr_cast<CComEntityModel>(std::move(pProto)));
    }


   
    //m_pCenterCollider = CCollSphere::Create({0.f, 0.3f, 0.f}, 0.3f);
    m_pCenterCollider = CCollBox::Create({ 0.f, 1.f, 0.f }, { 0.25f, 0.9f, 0.25f });

    return S_OK;
}

void CPlayerEntity::PriorityUpdate(E::_float fTimeDelta)
{
    m_pPlayerCamera = CGameInstance::Get().GetGameCamera("Player");
    m_pActivePlayerCamera = CGameInstance::Get().GetActiveGameCamera("Player");
    m_bControl = CGameInstance::Get().GetMouseFix() && m_pActivePlayerCamera;
    if (m_bControl)
    {
        m_bKeyPressingW = CGameInstance::Get().KeyPressing(DIK_W);
        m_bKeyPressingA = CGameInstance::Get().KeyPressing(DIK_A);
        m_bKeyPressingS = CGameInstance::Get().KeyPressing(DIK_S);
        m_bKeyPressingD = CGameInstance::Get().KeyPressing(DIK_D);
        m_bKeyPressingQ = CGameInstance::Get().KeyPressing(DIK_Q);
        m_bKeyPressingE = CGameInstance::Get().KeyPressing(DIK_E);
        m_bKeyPressingShift = CGameInstance::Get().KeyPressing(DIK_LSHIFT);
        m_bKeyPressingSpace = CGameInstance::Get().KeyPressing(DIK_SPACE);
        m_iMouseMoveX = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::X);
        m_iMouseMoveY = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::Y);
        m_iMouseMoveZ = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::Z);
        m_bMousePressingLeft = CGameInstance::Get().MousePressing(MOUSEKEYSTATE::LB);
        m_bMousePressingRight = CGameInstance::Get().MousePressing(MOUSEKEYSTATE::RB);
        m_iNumKeyPressing[1] = CGameInstance::Get().KeyPressing(DIK_1);
        m_iNumKeyPressing[2] = CGameInstance::Get().KeyPressing(DIK_2);
        m_iNumKeyPressing[3] = CGameInstance::Get().KeyPressing(DIK_3);
        m_iNumKeyPressing[4] = CGameInstance::Get().KeyPressing(DIK_4);
        m_iNumKeyPressing[5] = CGameInstance::Get().KeyPressing(DIK_5);
        m_iNumKeyPressing[6] = CGameInstance::Get().KeyPressing(DIK_6);
        m_iNumKeyPressing[7] = CGameInstance::Get().KeyPressing(DIK_7);
        m_iNumKeyPressing[8] = CGameInstance::Get().KeyPressing(DIK_8);
        m_iNumKeyPressing[9] = CGameInstance::Get().KeyPressing(DIK_9);

    }
    else
    {
        m_bKeyPressingW = false;
        m_bKeyPressingA = false;
        m_bKeyPressingS = false;
        m_bKeyPressingD = false;
        m_bKeyPressingQ = false;
        m_bKeyPressingE = false;
        m_bKeyPressingShift = false;
        m_bKeyPressingSpace = false;
        m_iMouseMoveX = 0;
        m_iMouseMoveY = 0;
        m_iMouseMoveZ = 0;
        m_bMousePressingLeft = false;
        m_bMousePressingRight = false;
        memset(&m_iNumKeyPressing, 0, sizeof(m_iNumKeyPressing));
    }
}

/*
root
├─ waist
│  └─ body
│     ├─ head
│     │  └─ hat
│     ├─ jacket
│     ├─ cape
│     ├─ rightArm
│     │  ├─ rightSleeve
│     │  └─ rightItem
│     └─ leftArm
│        ├─ leftSleeve
│        └─ leftItem
└─ rightLeg (parent="root")
   └─ rightPants
└─ leftLeg (parent="root")
   └─ leftPants
*/
void CPlayerEntity::Update(E::_float fTimeDelta)
{
    m_pComEntityModel->ResetBonesChannel();
    if (m_eCameraType == CAMERA_TYPE::FPS)
    {
        PlayerMove(fTimeDelta);

        // camera control
        {
            PlayerCameraTrace(fTimeDelta);
        }

        if (auto tmpCamera = CGameInstance::Get().GetGameCamera("Player"))
        {
            //m_hRightItem
            if (m_hRightItem)
            {
                if (auto item = CGameInstance::Get().GetGameObjectByHandle(m_hRightItem.value()))
                {
                    static float elapsedTmp = 0;
                    elapsedTmp += fTimeDelta;
                    float t = fmodf(elapsedTmp, 1.f);  // 0~1 반복

                    float rotX = -sinf(t * XM_PI * 1.f) * XMConvertToRadians(25.f);

                    XMMATRIX matAnim = XMMatrixRotationX(rotX);
                    tmpCamera->GetTransform().Update();
                    auto tmp =  tmpCamera->GetTransform().GetLoadedCombinedWorldMatrix();
                    _float4x4 mat;
                    XMStoreFloat4x4(&mat, tmp);
                    item->GetTransform().SetParentWorldMatrix(mat);
                }
            }
        }
    } // End if FPS
    else // NOT FPS
    {
        /*
        머리 돌리기
        카메라 위치로 머리를 돌려야함
        이때 머리는 루트의 영향을 받아버림
        머리가 45도 이상 돌릴때는 머리가 아니라 루트를 돌려야함
        y축을 돌릴때는 머리와 루트를 두개 고려해야하지만
        x축을 돌리는건 오직 머리만 고려함
    */
        {
            if (m_pActivePlayerCamera)
            {
                auto pHeadBone = m_pComEntityModel->GetBone("head");
                auto pRootBone = m_pComEntityModel->GetBone("root");

                if (pHeadBone && pRootBone)
                {
                    _float adjustY = 0.f;
                    _float adjustX = 1.f;
                    if (m_eCameraType == CAMERA_TYPE::TPS_BACK)
                    {
                        adjustY = XMConvertToRadians(180.f);
                        adjustX = -1.f;
                    }

                    // 목표는 카메라가 돌린 x,y 값
                    float targetRotYRad = XMConvertToRadians(m_pActivePlayerCamera->GetTransform().GetRotationEuler().y);
                    float targetRotXRad = XMConvertToRadians(m_pActivePlayerCamera->GetTransform().GetRotationEuler().x);

                    bool bMovingLeftRight = m_bKeyPressingA || m_bKeyPressingD;
                    bool bMovingForwardBackward = m_bKeyPressingW || m_bKeyPressingS;



                    // 왼쪽 오른쪽로 움직이고 있으면 Root는 각 45도 방향으로
                    if (bMovingLeftRight)
                    {
                        float sign;
                        if (m_bKeyPressingA)
                        {
                            sign = -1.f;
                        }
                        else
                        {
                            sign = 1.f;
                        }

                        float threshold = XMConvertToRadians(45.f);

                        float toRotYRadDiff = targetRotYRad - m_fRootRotRadY;;

                        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
                        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

                        //(sign * threshold)

                        float delta = (toRotYRadDiff + (threshold * sign)) * 5.f * fTimeDelta;
                        m_fRootRotRadY += delta;
                        //pHeadBone->GetRotation()->y += (threshold * sign);
                        pHeadBone->GetRotation()->y += toRotYRadDiff;
                        pHeadBone->GetRotation()->x += targetRotXRad;

                        pRootBone->GetRotation()->y += m_fRootRotRadY;
                    }

                    // 앞뒤로 움직이고 있으면 Root는 보고 있는 방향으로
                    else if (bMovingForwardBackward)
                    {
                        float toRotYRadDiff = targetRotYRad - m_fRootRotRadY;

                        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
                        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

                        float sign = toRotYRadDiff > 0 ? 1.f : -1.f;

                        float threshold = XMConvertToRadians(0.f);

                        if (fabsf(toRotYRadDiff) > threshold)
                        {
                            float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
                            m_fRootRotRadY += delta;

                            pHeadBone->GetRotation()->y += toRotYRadDiff;
                        }
                        else
                        {
                            pHeadBone->GetRotation()->y += (threshold * sign);
                        }


                        pRootBone->GetRotation()->y += m_fRootRotRadY;

                        pHeadBone->GetRotation()->x += targetRotXRad;
                    }
                    // 움직이지 않는 상태
                    else
                    {
                        float toRotYRadDiff = targetRotYRad - m_fRootRotRadY;

                        while (toRotYRadDiff > XM_PI) toRotYRadDiff -= XM_2PI;
                        while (toRotYRadDiff < -XM_PI) toRotYRadDiff += XM_2PI;

                        float sign = toRotYRadDiff > 0 ? 1.f : -1.f;

                        float threshold = XMConvertToRadians(45.f);

                        if (fabsf(toRotYRadDiff) > threshold)
                        {
                            float delta = (toRotYRadDiff - (threshold * sign)) * 5.f * fTimeDelta;
                            m_fRootRotRadY += delta;
                            pHeadBone->GetRotation()->y += (threshold * sign);
                        }
                        else
                        {
                            pHeadBone->GetRotation()->y += toRotYRadDiff;
                        }


                        pRootBone->GetRotation()->y += m_fRootRotRadY;

                        pHeadBone->GetRotation()->x += targetRotXRad;
                    }
                }
            }
        }


        // move head bone
        {
            if (false && m_pActivePlayerCamera)
            {
                auto pHeadBone = m_pComEntityModel->GetBone("head");
                auto pRootBone = m_pComEntityModel->GetBone("root");
                if (pHeadBone && pRootBone)
                {
                    _float adjustY = 0.f;
                    _float adjustX = 1.f;
                    if (m_eCameraType == CAMERA_TYPE::TPS_BACK)
                    {
                        adjustY = XMConvertToRadians(180.f);
                        adjustX = -1.f;
                    }

                    auto playerCamEulerRot = m_pActivePlayerCamera->GetTransform().GetRotationEuler();
                    float targetHeadY = XMConvertToRadians(playerCamEulerRot.y) + adjustY;
                    float targetHeadX = XMConvertToRadians(playerCamEulerRot.x) * adjustX;

                    // targetHeadY 정규화
                    while (targetHeadY > XM_PI)  targetHeadY -= XM_2PI;
                    while (targetHeadY < -XM_PI) targetHeadY += XM_2PI;

                    // 멤버에서 읽되, 비교용으로만 정규화
                    float currRootNorm = m_fRootRotRadY;
                    while (currRootNorm > XM_PI)  currRootNorm -= XM_2PI;
                    while (currRootNorm < -XM_PI) currRootNorm += XM_2PI;

                    // head가 root 기준으로 얼마나 돌아있나
                    float relativeAngle = targetHeadY - currRootNorm;
                    while (relativeAngle > XM_PI)  relativeAngle -= XM_2PI;
                    while (relativeAngle < -XM_PI) relativeAngle += XM_2PI;

                    const float HEAD_LIMIT = XMConvertToRadians(45.f);
                    const float ROOT_SPEED = 10.f;
                    bool bForwardMoving = m_bKeyPressingW || m_bKeyPressingS;
                    bool bLRMoving = m_bKeyPressingA || m_bKeyPressingD;

                    // root를 얼마나 틀지 계산하는 람다
                    auto SmoothRoot = [&](float targetRootY) -> float
                        {
                            float delta = targetRootY - currRootNorm;
                            while (delta > XM_PI)  delta -= XM_2PI;
                            while (delta < -XM_PI) delta += XM_2PI;
                            // m_fRootRotY에 delta만큼 더함 (정규화 없이 누적)
                            return m_fRootRotRadY + delta * ROOT_SPEED * fTimeDelta;
                        };

                    if (bLRMoving)
                    {
                        float sideOffset = m_bKeyPressingA ? -HEAD_LIMIT : HEAD_LIMIT;
                        float targetRootY = targetHeadY + sideOffset;

                        m_fRootRotRadY = SmoothRoot(targetRootY);

                        // 새 root 기준 head 상대각
                        float newRootNorm = m_fRootRotRadY;
                        while (newRootNorm > XM_PI)  newRootNorm -= XM_2PI;
                        while (newRootNorm < -XM_PI) newRootNorm += XM_2PI;

                        float newRelative = targetHeadY - newRootNorm;
                        while (newRelative > XM_PI)  newRelative -= XM_2PI;
                        while (newRelative < -XM_PI) newRelative += XM_2PI;
                        newRelative = std::clamp(newRelative, -HEAD_LIMIT, HEAD_LIMIT);

                        pRootBone->AddRotation({ 0.f, newRootNorm, 0.f });
                        pHeadBone->AddRotation({ targetHeadX, newRelative, 0.f });
                    }
                    else if (bForwardMoving)
                    {
                        m_fRootRotRadY = SmoothRoot(targetHeadY);

                        float newRootNorm = m_fRootRotRadY;
                        while (newRootNorm > XM_PI)  newRootNorm -= XM_2PI;
                        while (newRootNorm < -XM_PI) newRootNorm += XM_2PI;

                        float newRelative = targetHeadY - newRootNorm;
                        while (newRelative > XM_PI)  newRelative -= XM_2PI;
                        while (newRelative < -XM_PI) newRelative += XM_2PI;
                        newRelative = std::clamp(newRelative, -HEAD_LIMIT, HEAD_LIMIT);

                        pRootBone->AddRotation({ 0.f, newRootNorm, 0.f });
                        pHeadBone->AddRotation({ targetHeadX, newRelative, 0.f });
                    }
                    else
                    {
                        // 정지: head가 45도 안이면 root 안 움직임
                        if (fabsf(relativeAngle) <= HEAD_LIMIT)
                        {
                            // m_fRootRotY 변경 없음
                            pRootBone->AddRotation({ 0.f, currRootNorm, 0.f });
                            pHeadBone->AddRotation({ targetHeadX, relativeAngle, 0.f });
                        }
                        else
                        {
                            // 45도 초과 → root를 head 방향으로 밀어줌
                            float sign = (relativeAngle > 0.f) ? 1.f : -1.f;
                            float targetRootY = targetHeadY - sign * HEAD_LIMIT;

                            m_fRootRotRadY = SmoothRoot(targetRootY);

                            float newRootNorm = m_fRootRotRadY;
                            while (newRootNorm > XM_PI)  newRootNorm -= XM_2PI;
                            while (newRootNorm < -XM_PI) newRootNorm += XM_2PI;

                            pRootBone->AddRotation({ 0.f, newRootNorm, 0.f });
                            pHeadBone->AddRotation({ targetHeadX, sign * HEAD_LIMIT, 0.f });
                        }
                    }
                }
            }
        }

        if (1)
        {
            static float fTmp2 = 0;
            fTmp2 += fTimeDelta;;
            float lifeTime = fTmp2;
            float bob = (cosf(lifeTime * XMConvertToRadians(103.2f)) * 2.865f) + 2.865f;

            if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
            {
                pLeftArm->GetRotation()->z += XMConvertToRadians(-bob);
            }

            if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
            {
                pRightArm->GetRotation()->z += XMConvertToRadians(bob);
            }
        }

        if (1)
        {
            static float fWalkCycle = 0.f;
            bool bMoving = m_bKeyPressingA || m_bKeyPressingD || m_bKeyPressingW || m_bKeyPressingS;
            if (bMoving)
            {
                fWalkCycle += fTimeDelta * 6.28f;
                if (fWalkCycle > 6.28f) {
                    fWalkCycle -= 6.28f;
                }

                float tcos0 = cos(fWalkCycle) * 30;

                // leftarm, leftleg, rightarm, rightleg 회전 적용
                if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
                {
                    pLeftArm->GetRotation()->x += XMConvertToRadians(tcos0);
                }

                if (auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg"))
                {
                    pLeftLeg->GetRotation()->x += XMConvertToRadians(tcos0 * -1.4f);

                }

                if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
                {
                    pRightArm->GetRotation()->x += XMConvertToRadians(-tcos0);
                }

                if (auto pRightLeg = m_pComEntityModel->GetBone("rightLeg"))
                {
                    pRightLeg->GetRotation()->x += XMConvertToRadians(tcos0 * 1.4f);
                }
            }
        }

        

        PlayerMove(fTimeDelta);



        /////

        if (1)
        {
            if (m_bKeyPressingShift)
            {
                constexpr float fScale = 0.0625f;

                if (auto pWaist = m_pComEntityModel->GetBone("waist"))
                {
                    pWaist->GetRotation()->x += XMConvertToRadians(28.f);
                    pWaist->GetTranslatoin()->y -= 1.f * fScale;
                }

                if (auto pHead = m_pComEntityModel->GetBone("head"))
                {
                    pHead->GetRotation()->x += -XMConvertToRadians(28.f);

                }
            }
        }



        {
            if (m_bMousePressingLeft)
            {
                m_bAttacking = true;
                m_fAttackTime = 0.0f;
                m_fAttackDuration = 0.3f;  // 공격 지속 시간
            }

            if (m_bAttacking)
            {
                // attack_time: 0.0 ~ 1.0
                m_fAttackTime += fTimeDelta / m_fAttackDuration;
                m_fAttackTime = std::min(m_fAttackTime, 1.0f);

                float attackTime = m_fAttackTime;
                float attackBodyRotY = m_iMouseMoveX;  // 현재 head 회전값

                // body 회전
                if (auto pBody = m_pComEntityModel->GetBone("body"))
                {
                    float t = 1.0f - pow(1.0f - attackTime, 4.0f);
                    float leftArmRotX = -(sin(t * 3.14159f) * 1.2f + sin(attackTime * 3.14159f)) * 10.0f;
                    //pBody->GetRotation()->y += XMConvertToRadians(leftArmRotX);
                }

                // leftarm 회전
                if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
                {
                    float t = 1.0f - pow(1.0f - attackTime, 4.0f);
                    float leftArmRotX = -(sin(t * 3.14159f) * 1.2f + sin(attackTime * 3.14159f)) * 10.0f;

                    pLeftArm->GetRotation()->x += XMConvertToRadians(leftArmRotX);
                }

                // rightarm 회전
                if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
                {
                    float t = 1.0f - pow(1.0f - attackTime, 4.0f);
                    float sinT = sin(t * 3.14159f);

                    float rightArmRotX = -(sinT * 1.2f + sin(attackTime * 3.14159f)) * 30.0f;

                    float rightArmRotY = -(sinT != 0.0f ?
                        (-90.0f * sinT) + 30.0f :
                        0.0f);

                    pRightArm->GetRotation()->x += XMConvertToRadians(rightArmRotX);
                    pRightArm->GetRotation()->y += XMConvertToRadians(rightArmRotY);

                }

                // 공격 완료
                if (m_fAttackTime >= 1.0f)
                {
                    m_bAttacking = false;
                    m_fAttackTime = 0.0f;
                }
            }
        }



        // camera control
        {
            PlayerCameraTrace(fTimeDelta);
        }
    }
}



void CPlayerEntity::LateUpdate(E::_float fTimeDelta)
{
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    
    GetTransform().Update();

    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

    E::CGameInstance::Get().AddColliderGroup("Coll_PlayerCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

    //if (auto item = m_pComEntityArmModel->GetBone("rightItem"))
    //{
    //    for (auto& child : GetChildrenNode())
    //    {
    //        if (child->GetObjectTag() == "DropItem_WoodPickaxe")
    //        {
    //            _float4x4 mat;
    //            auto tmp = XMLoadFloat4x4(item->GetCombinedTransformationMatrix()) * GetTransform().GetLoadedCombinedWorldMatrix();
    //            XMStoreFloat4x4(&mat, tmp);
    //            child->GetTransform().SetParentWorldMatrix(mat);
    //        }
    //    }
    //}
}

HRESULT CPlayerEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    if (m_eCameraType == CAMERA_TYPE::FPS)
    {
        
    }
    else
    {
        {
            auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
            D3D11_MAPPED_SUBRESOURCE mappedSubResource;
            if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
            {

                E::CB_PER_OBJECT cbPerObject{};
                cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
                XMStoreFloat4x4(&cbPerObject.matWVP, GetTransform().GetLoadedWorldMatrix() * ctx.matViewProj);

                memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
                pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
            }
            pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
            pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        }

        m_pComEntityModel->BindBoneMatrix();

        m_pComEntityModel->Render(pContext, ctx);
    }
    
    return S_OK;
}

void CPlayerEntity::PlayerCameraTrace(_float fTimeDelta)
{
    if (m_pActivePlayerCamera)
    {
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        m_pActivePlayerCamera->GetTransform().AddRotation(vUp, fTimeDelta * 10.f * m_iMouseMoveX);

        _float3 euler = m_pActivePlayerCamera->GetTransform().GetRotationEuler();

        float delta = fTimeDelta * 10.f * m_iMouseMoveY;
        if (m_eCameraType == CAMERA_TYPE::TPS_BACK)
        {
            delta *= -1.f;
        }
        float next = euler.x + delta;

        if (next <= 89.f && next >= -89.f)
        {
            _vector vRight = m_pActivePlayerCamera->GetTransform().GetState(STATE::RIGHT);
            m_pActivePlayerCamera->GetTransform().AddRotation(vRight, delta);
        }

        if (m_eCameraType == CAMERA_TYPE::FPS)
        {
            if (m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = false;
                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerPos = GetTransform().GetPosition();
            m_pActivePlayerCamera->GetTransform().SetPosition(XMVectorSet(playerPos.x, playerPos.y + 1.62f, playerPos.z + 0.f, 1.f));
        }
        else if (m_eCameraType == CAMERA_TYPE::TPS)
        {
            if (m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = false;
                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerLook = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
            auto playerPos = GetTransform().GetLoadedPostion();

            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);

            m_pActivePlayerCamera->GetTransform().SetPosition(tmp);
        }
        else
        {
            if (!m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = true;
                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerLook = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
            auto playerPos = GetTransform().GetLoadedPostion();

            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);

            m_pActivePlayerCamera->GetTransform().SetPosition(tmp);
        }
    }
}

void CPlayerEntity::PlayerMove(_float fTimeDelta)
{
    XMVECTOR vVel = XMLoadFloat3(&m_vVelocity);

    // 1. 수평 입력 → wishDir
    XMVECTOR vWishDir = XMVectorZero();
    if (m_pActivePlayerCamera)
    {
        XMVECTOR camLook = XMVector3Normalize(XMVectorSetY(m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK), 0.f));
        XMVECTOR camRight = XMVector3Normalize(XMVectorSetY(m_pActivePlayerCamera->GetTransform().GetState(STATE::RIGHT), 0.f));

        if (m_bKeyPressingW) vWishDir += camLook;
        if (m_bKeyPressingS) vWishDir -= camLook;
        if (m_bKeyPressingD) vWishDir += camRight;
        if (m_bKeyPressingA) vWishDir -= camRight;

        if (XMVectorGetX(XMVector3LengthSq(vWishDir)) > 0.f)
            vWishDir = XMVector3Normalize(vWishDir);
    }

    if (m_eModeType == MODE_TYPE::GRAVITY)
    {
        // 2. 가속
        float fCurrSpeed = XMVectorGetX(XMVector3Dot(XMVectorSetY(vVel, 0.f), vWishDir));
        float fAddSpeed = m_fPlayerSpeed - fCurrSpeed;
        if (fAddSpeed > 0.f)
        {
            float fAccel = m_bOnGround ? 18.f : 18.f;
            float fAccelSpeed = std::min(fAccel * m_fPlayerSpeed * fTimeDelta, fAddSpeed);
            vVel += vWishDir * fAccelSpeed;
        }

        // bunnyhop remove
        XMVECTOR vHoriz = XMVectorSetY(vVel, 0.f);
        float fHorizSpeed = XMVectorGetX(XMVector3Length(vHoriz));
        if (fHorizSpeed > m_fPlayerSpeed)
        {
            vHoriz = XMVector3Normalize(vHoriz) * m_fPlayerSpeed;
            vVel = XMVectorSetY(vHoriz, XMVectorGetY(vVel));
        }

        // 3. 마찰 (입력 없을 때만)
        bool bHasInput = XMVectorGetX(XMVector3LengthSq(vWishDir)) > 0.f;
        if (m_bOnGround && !bHasInput)
        {
            float fSpeed = XMVectorGetX(XMVector3Length(XMVectorSetY(vVel, 0.f)));
            if (fSpeed > 0.f)
            {
                float fNewSpeed = std::max(fSpeed - fSpeed * 15.f * fTimeDelta, 0.f);
                float vy = XMVectorGetY(vVel);
                vVel = XMVectorSetY(vVel * (fNewSpeed / fSpeed), vy);
            }
        }

        // 중력
        if (!m_bOnGround)
            vVel = XMVectorSetY(vVel, XMVectorGetY(vVel) - 20.f * fTimeDelta);

        // 점프
        if (m_bKeyPressingSpace && m_bOnGround)
        {
            vVel = XMVectorSetY(vVel, 7.f);
            m_bOnGround = false;
        }

        // AABB 충돌
        const XMFLOAT3 halfExtents = { 0.25f, 0.9f, 0.25f };
        XMFLOAT3 pos = GetTransform().GetPosition();
        XMFLOAT3 c = { pos.x, pos.y + 1.f, pos.z };

        // Y
        float velY = XMVectorGetY(vVel);
        float prevY = c.y;  // 이동 전 저장
        c.y += velY * fTimeDelta;
        if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
        {
            if (velY < 0.f)  // 내려가다 충돌 → 땅
            {
                c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;// + 0.001f 딱붙어서 안움직이는현상
                m_bOnGround = true;
            }
            else  // 올라가다 천장
            {
                c.y = prevY;  // ← 그냥 이전 위치로 복구
            }
            vVel = XMVectorSetY(vVel, 0.f);
        }
        else m_bOnGround = false;

        // X
        float px = c.x;
        c.x += XMVectorGetX(vVel) * fTimeDelta;
        if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
        {
            c.x = px; vVel = XMVectorSetX(vVel, 0.f);
        }

        // Z
        float pz = c.z;
        c.z += XMVectorGetZ(vVel) * fTimeDelta;
        if (CGameInstance::Get().VoxelAABBOverlap(c, halfExtents))
        {
            c.z = pz; vVel = XMVectorSetZ(vVel, 0.f);
        }

        GetTransform().SetPosition(_float3{ c.x, c.y - 1.f, c.z });
    }
    else  // GOD - 마찰/가속 없이 즉시 이동
    {
        XMVECTOR vHorizVel = vWishDir * m_fPlayerSpeed * 5.f;

        if (m_bKeyPressingSpace) vHorizVel = XMVectorSetY(vHorizVel, 20.f);
        else if (m_bKeyPressingShift) vHorizVel = XMVectorSetY(vHorizVel, -20.f);
        else                          vHorizVel = XMVectorSetY(vHorizVel, 0.f);

        vVel = vHorizVel;  // velocity 직접 세팅 (관성 없음)
        GetTransform().AddPosition(vVel * fTimeDelta);
    }

    XMStoreFloat3(&m_vVelocity, vVel);
}


UPtr<CPlayerEntity> CPlayerEntity::Create()
{
    auto pInstance = ToUPtr(new CPlayerEntity{});
    if (FAILED(pInstance->InitializePrototype()))
    {
        MSG_BOX("Failed to Create: CPlayerEntity");
        return nullptr;
    }

    return pInstance;
}

UPtr<CPrototype> CPlayerEntity::Clone(void* pArg)
{
    auto pInstance = ToUPtr(new CPlayerEntity{ *this });
    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned: CPlayerEntity");
        return nullptr;
    }

    return pInstance;
}
