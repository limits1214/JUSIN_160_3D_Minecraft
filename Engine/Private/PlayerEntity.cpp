#include "PlayerEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"

#include "CollSphere.h"
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

    
    m_pCenterCollider = CCollSphere::Create({0.f, 0.3f, 0.f}, 0.3f);

    return S_OK;
}

void CPlayerEntity::PriorityUpdate(E::_float fTimeDelta)
{
    m_pPlayerCamera = CGameInstance::Get().GetActiveGameCamera("Player");
    m_bControl = CGameInstance::Get().GetMouseFix() && m_pPlayerCamera;
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
    
   

    // move head bone
    {
        if (m_pPlayerCamera)
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


                auto playerCamEulerRot = m_pPlayerCamera->GetTransform().GetRotationEuler();



               
                float targetHeadY = XMConvertToRadians(playerCamEulerRot.y) + adjustY;
                float targetHeadX = XMConvertToRadians(playerCamEulerRot.x) * adjustX;

                while (targetHeadY > XM_PI) targetHeadY -= XM_2PI;
                while (targetHeadY < -XM_PI) targetHeadY += XM_2PI;

                float currRootRotY = pRootBone->GetRotation()->y;

                // currRootRotY 도 정규화
                while (currRootRotY > XM_PI) currRootRotY -= XM_2PI;
                while (currRootRotY < -XM_PI) currRootRotY += XM_2PI;

                float relativeAngle = targetHeadY - currRootRotY;
                while (relativeAngle > XM_PI) relativeAngle -= XM_2PI;
                while (relativeAngle < -XM_PI) relativeAngle += XM_2PI;

                const float HEAD_LIMIT = XMConvertToRadians(45.f);
                bool bForwardMoving = m_bKeyPressingW || m_bKeyPressingS;
                bool bLRMoving = m_bKeyPressingA || m_bKeyPressingD;

                // 이동 중엔 빠르게, 정지 중엔 느리게
                float ROOT_SPEED = (bForwardMoving || bLRMoving) ? 10.f : 10.f;

                //bool bMoving = m_bKeyPressingW || m_bKeyPressingS || m_bKeyPressingA || m_bKeyPressingD;

                if (bLRMoving)
                {
                    // A = 왼쪽(-45도), D = 오른쪽(+45도)
                    float sideOffset = 0.f;
                    if (m_bKeyPressingA) sideOffset = -HEAD_LIMIT;
                    if (m_bKeyPressingD) sideOffset = HEAD_LIMIT;

                    // Root 목표 = 카메라 방향 + 옆 오프셋
                    float newRootY = targetHeadY + sideOffset;
                    float rootDelta = newRootY - currRootRotY;
                    while (rootDelta > XM_PI) rootDelta -= XM_2PI;
                    while (rootDelta < -XM_PI) rootDelta += XM_2PI;

                    float smoothRootY = currRootRotY + rootDelta * ROOT_SPEED * fTimeDelta;
                    while (smoothRootY > XM_PI) smoothRootY -= XM_2PI;
                    while (smoothRootY < -XM_PI) smoothRootY += XM_2PI;

                    auto rootRot = *pRootBone->GetRotation();
                    rootRot.y = smoothRootY;
                    pRootBone->SetRotation(rootRot);

                    // Head 는 새 Root 기준 상대각 — 45도 clamp
                    float newRelative = targetHeadY - smoothRootY;
                    while (newRelative > XM_PI) newRelative -= XM_2PI;
                    while (newRelative < -XM_PI) newRelative += XM_2PI;
                    newRelative = std::max(-HEAD_LIMIT, std::min(HEAD_LIMIT, newRelative));

                    auto headRot = *pHeadBone->GetRotation();
                    headRot.x = targetHeadX;
                    headRot.y = newRelative;
                    pHeadBone->SetRotation(headRot);
                }
                else if (bForwardMoving)
                {
                    // Root 를 카메라 방향으로 빠르게 맞춤
                    float rootDelta = targetHeadY - currRootRotY;
                    while (rootDelta > XM_PI) rootDelta -= XM_2PI;
                    while (rootDelta < -XM_PI) rootDelta += XM_2PI;

                    float smoothRootY = currRootRotY + rootDelta * ROOT_SPEED * fTimeDelta;
                    while (smoothRootY > XM_PI) smoothRootY -= XM_2PI;
                    while (smoothRootY < -XM_PI) smoothRootY += XM_2PI;

                    auto rootRot = *pRootBone->GetRotation();
                    rootRot.y = smoothRootY;
                    pRootBone->SetRotation(rootRot);

                    // Head 는 새 Root 기준 상대각 — 45도 clamp 유지
                    float newRelative = targetHeadY - smoothRootY;
                    while (newRelative > XM_PI) newRelative -= XM_2PI;
                    while (newRelative < -XM_PI) newRelative += XM_2PI;
                    newRelative = std::max(-HEAD_LIMIT, std::min(HEAD_LIMIT, newRelative));

                    auto headRot = *pHeadBone->GetRotation();
                    headRot.x = targetHeadX;
                    headRot.y = newRelative;
                    pHeadBone->SetRotation(headRot);
                }
                else
                {
                    // 정지 중 — 45도 제한
                    if (abs(relativeAngle) < HEAD_LIMIT)
                    {
                        auto headRot = *pHeadBone->GetRotation();
                        headRot.x = targetHeadX;
                        headRot.y = relativeAngle;
                        pHeadBone->SetRotation(headRot);
                    }
                    else
                    {
                        float sign = (relativeAngle > 0.f) ? 1.f : -1.f;

                        float newRootY = targetHeadY - sign * HEAD_LIMIT;
                        float rootDelta = newRootY - currRootRotY;
                        while (rootDelta > XM_PI) rootDelta -= XM_2PI;
                        while (rootDelta < -XM_PI) rootDelta += XM_2PI;

                        float smoothRootY = currRootRotY + rootDelta * ROOT_SPEED * fTimeDelta;
                        while (smoothRootY > XM_PI) smoothRootY -= XM_2PI;
                        while (smoothRootY < -XM_PI) smoothRootY += XM_2PI;

                        auto rootRot = *pRootBone->GetRotation();
                        rootRot.y = smoothRootY;
                        pRootBone->SetRotation(rootRot);

                        auto headRot = *pHeadBone->GetRotation();
                        headRot.x = targetHeadX;
                        headRot.y = sign * HEAD_LIMIT;
                        pHeadBone->SetRotation(headRot);
                    }
                }
            }
        }
    }

    if (m_eModeType == MODE_TYPE::GRAVITY)
    {
        float fGravity = 9.81f * 0.01f;

        //GetTransform()
        //GetTransform().SetPosition(

        _float3 pos = GetTransform().GetPosition();
        int32_t wbx = (int32_t)floorf(pos.x);
        int32_t wby = (int32_t)floorf(pos.y);
        int32_t wbz = (int32_t)floorf(pos.z);

        auto block = CGameInstance::Get().GetVoxelBlock(wbx, wby-1, wbz);

        if (block.has_value() && block.value().GetType() == CBlock3::TYPE::AIR)
        {
            m_vVelocity.y -= fGravity * fTimeDelta;

            GetTransform().AddPosition(m_vVelocity);
        }
        else
        {
            m_vVelocity = {};
            auto pos = GetTransform().GetPosition();
            pos.y = wby;
            GetTransform().SetPosition(pos);
        }
    }

    if (m_bKeyPressingW)
    {
        if (m_pPlayerCamera)
        {
            auto trsf = m_pPlayerCamera->GetTransform().GetLoadedWorldMatrix();
            auto next = GetTransform().GetLoadedPostion() + trsf.r[2] * 10.f * fTimeDelta;
            GetTransform().SetPosition(next);
        }
    }
    if (m_bKeyPressingA)
    {
        if (m_pPlayerCamera)
        {
            auto trsf = m_pPlayerCamera->GetTransform().GetLoadedWorldMatrix();
            auto next = GetTransform().GetLoadedPostion() + trsf.r[0] * 10.f * -fTimeDelta;
            GetTransform().SetPosition(next);
        }
    }
    if (m_bKeyPressingS)
    {
        if (m_pPlayerCamera)
        {
            auto trsf = m_pPlayerCamera->GetTransform().GetLoadedWorldMatrix();
            auto next = GetTransform().GetLoadedPostion() + trsf.r[2] * 10.f * -fTimeDelta;
            GetTransform().SetPosition(next);
        }
    }
    if (m_bKeyPressingD)
    {
        if (m_pPlayerCamera)
        {
            auto trsf = m_pPlayerCamera->GetTransform().GetLoadedWorldMatrix();
            auto next = GetTransform().GetLoadedPostion() + trsf.r[0] * 10.f * fTimeDelta;
            GetTransform().SetPosition(next);
        }
    }


    if (m_pPlayerCamera)
    { // camera control
        {
            _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
            m_pPlayerCamera->GetTransform().AddRotation(vUp, fTimeDelta * 10.f * m_iMouseMoveX);

            _float3 euler = m_pPlayerCamera->GetTransform().GetRotationEuler();

            float delta = fTimeDelta * 10.f * m_iMouseMoveY;
            if (m_eCameraType == CAMERA_TYPE::TPS_BACK)
            {
                delta *= -1.f;
            }
            float next = euler.x + delta;

            if (next <= 89.f && next >= -89.f)
            {
                _vector vRight = m_pPlayerCamera->GetTransform().GetState(STATE::RIGHT);
                m_pPlayerCamera->GetTransform().AddRotation(vRight, delta);
            }
        }
        if (m_eCameraType == CAMERA_TYPE::FPS)
        {
            if (m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = false;
                m_pPlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerPos = GetTransform().GetPosition();
            m_pPlayerCamera->GetTransform().SetPosition(XMVectorSet(playerPos.x, playerPos.y + 1.8f, playerPos.z + 0.f, 1.f));
        }
        else if (m_eCameraType == CAMERA_TYPE::TPS)
        {
            if (m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = false;
                m_pPlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerLook = m_pPlayerCamera->GetTransform().GetState(STATE::LOOK);
            auto playerPos = GetTransform().GetLoadedPostion();

            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);

            m_pPlayerCamera->GetTransform().SetPosition(tmp);
        }
        else
        {
            if (!m_bPlayerCameraLookBack)
            {
                m_bPlayerCameraLookBack = true;
                m_pPlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
            }

            auto playerLook = m_pPlayerCamera->GetTransform().GetState(STATE::LOOK);
            auto playerPos = GetTransform().GetLoadedPostion();

            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);

            m_pPlayerCamera->GetTransform().SetPosition(tmp);
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


    //if (CGameInstance::Get().KeyPressing(DIK_RIGHT))
    //{
    //    if (auto pHeadBone = m_pComEntityModel->GetBone("head"))
    //    {
    //        pHeadBone->AddRotation({0.f, XMConvertToRadians(90.f) * fTimeDelta, 0.f});
    //    }
    //}

    //if (CGameInstance::Get().KeyPressing(DIK_LEFT))
    //{
    //    if (auto pHeadBone = m_pComEntityModel->GetBone("head"))
    //    {
    //        pHeadBone->AddRotation({ 0.f, -XMConvertToRadians(90.f) * fTimeDelta, 0.f });
    //    }
    //}

    if (auto cam = CGameInstance::Get().GetActiveGameCamera("Player"))
    {
        if (auto item = m_pComEntityModel->GetBone("rightItem"))
        {
            for (auto& child : GetChildrenNode())
            {
                if (child->GetObjectTag() == "DropItem_WoodPickaxe")
                {
                    _float4x4 mat;
                    auto tmp = XMLoadFloat4x4(item->GetCombinedTransformationMatrix()) * GetTransform().GetLoadedCombinedWorldMatrix();
                    XMStoreFloat4x4(&mat, tmp);
                    child->GetTransform().SetParentWorldMatrix(mat);
                }
            }
        }

        if (auto pHeadBone = m_pComEntityModel->GetBone("head"))
        {
            if (false && CGameInstance::Get().GetMouseFix())
            {
                if (auto mouseX = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::X))
                {
                    m_vHeadRotation.y += 10.f * fTimeDelta * mouseX;
                }
                    
                if (auto mouseY = CGameInstance::Get().MouseMove(MOUSEMOVESTATE::Y))
                {
                    float nextPitch = m_vHeadRotation.x + 10.f * fTimeDelta * (mouseY);

                    if (nextPitch <= 89.f && nextPitch >= -89.f)
                    {
                        m_vHeadRotation.x = nextPitch;
                    }
                }

                float headAbsoluteYaw = m_vHeadRotation.y;
                float headAbsolutePitch = m_vHeadRotation.x;

                static float rootAccumulatedYaw = 0.f;

                // ✅ 이동 키 확인
                bool bMoving = CGameInstance::Get().KeyPressing(DIK_W) ||
                    CGameInstance::Get().KeyPressing(DIK_S) ||
                    CGameInstance::Get().KeyPressing(DIK_A) ||
                    CGameInstance::Get().KeyPressing(DIK_D);

                float headRelativeYaw;

                if (bMoving)
                {
                    // ✅ 이동 중일 때: 부드럽게 root를 head 방향으로 회전
                    float targetRootYaw = headAbsoluteYaw;
                    float diff = targetRootYaw - rootAccumulatedYaw;

                    // ±180도 범위 정규화
                    if (diff > 180.f) diff -= 360.f;
                    if (diff < -180.f) diff += 360.f;

                    // 부드럽게 회전 (보간)
                    float lerpSpeed = 5.f * fTimeDelta;  // 속도 조정 가능
                    rootAccumulatedYaw += diff * lerpSpeed;

                    headRelativeYaw = headAbsoluteYaw - rootAccumulatedYaw;
                }
                else
                {
                    // ✅ 정지 중일 때: 45도 제한
                    headRelativeYaw = headAbsoluteYaw - rootAccumulatedYaw;

                    if (headRelativeYaw > 45.f)
                    {
                        float excess = headRelativeYaw - 45.f;
                        rootAccumulatedYaw += excess;
                        headRelativeYaw = 45.f;
                    }
                    else if (headRelativeYaw < -45.f)
                    {
                        float excess = headRelativeYaw + 45.f;
                        rootAccumulatedYaw += excess;
                        headRelativeYaw = -45.f;
                    }
                }

                // root 회전 적용
                if (auto pRootBone = m_pComEntityModel->GetBone("root"))
                {
                    _matrix matRootRot = XMMatrixRotationY(XMConvertToRadians(rootAccumulatedYaw));
                    _matrix rootBaseMat = XMLoadFloat4x4(pRootBone->GeTransformationMatrix());
                    _vector rootPos = rootBaseMat.r[3];

                    _matrix rootFinalMat = matRootRot;
                    //rootFinalMat.r[3] = rootPos;

                    pRootBone->UpdateTransformationMatrix(rootFinalMat);
                }

                // head 회전 적용
                _matrix matHeadRot =
                    XMMatrixRotationX(XMConvertToRadians(headAbsolutePitch)) *
                    XMMatrixRotationY(XMConvertToRadians(headRelativeYaw));

                _matrix headBaseMat = XMLoadFloat4x4(pHeadBone->GeTransformationMatrix());
                _vector headPos = headBaseMat.r[3];

                _matrix headFinalMat = matHeadRot;
                //headFinalMat.r[3] = headPos;

                pHeadBone->UpdateTransformationMatrix(headFinalMat);

                // 카메라 회전
                _matrix matCamRot =
                    XMMatrixRotationX(XMConvertToRadians(headAbsolutePitch)) *
                    XMMatrixRotationY(XMConvertToRadians(headAbsoluteYaw));

                cam->GetTransform().SetState(STATE::RIGHT, matCamRot.r[0]);
                cam->GetTransform().SetState(STATE::UP, matCamRot.r[1]);
                cam->GetTransform().SetState(STATE::LOOK, matCamRot.r[2]);

                // 이동
                if (CGameInstance::Get().KeyPressing(DIK_W))
                {
                    _vector vCurrentPos = GetTransform().GetLoadedPostion();
                    _vector vLookDir = XMVector3Normalize(matCamRot.r[2]);
                    _vector vNewPos = vCurrentPos + vLookDir * 10.f * fTimeDelta;
                    GetTransform().SetPosition(vNewPos);
                }

                if (CGameInstance::Get().KeyPressing(DIK_S))
                {
                    _vector vCurrentPos = GetTransform().GetLoadedPostion();
                    _vector vLookDir = XMVector3Normalize(matCamRot.r[2]);
                    _vector vNewPos = vCurrentPos - vLookDir * 10.f * fTimeDelta;
                    GetTransform().SetPosition(vNewPos);
                }

                if (CGameInstance::Get().KeyPressing(DIK_A))
                {
                    _vector vCurrentPos = GetTransform().GetLoadedPostion();
                    _vector vRightDir = XMVector3Normalize(matCamRot.r[0]);
                    _vector vNewPos = vCurrentPos - vRightDir * 10.f * fTimeDelta;
                    GetTransform().SetPosition(vNewPos);
                }

                if (CGameInstance::Get().KeyPressing(DIK_D))
                {
                    _vector vCurrentPos = GetTransform().GetLoadedPostion();
                    _vector vRightDir = XMVector3Normalize(matCamRot.r[0]);
                    _vector vNewPos = vCurrentPos + vRightDir * 10.f * fTimeDelta;
                    GetTransform().SetPosition(vNewPos);
                }

                auto pos = GetTransform().GetPosition();
                cam->GetTransform().SetState(STATE::POSITION, XMVectorSet(pos.x, pos.y + 1.7f, pos.z, 1.f));

                if (m_eCameraType == CAMERA_TYPE::FPS)
                {
                    cam->GetTransform().GoStraight(0.4f);
                }
                else if (m_eCameraType == CAMERA_TYPE::TPS)
                {
                    cam->GetTransform().GoStraight(-5.f);
                }
            }
        }
    }

    //if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
    //{
    //    static float tmp = 0;
    //    tmp += fTimeDelta * 90.f;
    //    _matrix matLeftArmRot = XMMatrixRotationX(XMConvertToRadians(tmp)) * XMMatrixRotationY(XMConvertToRadians(tmp * 0.5f));
    //    _matrix leftArmBaseMat = XMLoadFloat4x4(pLeftArm->GeTransformationMatrix());
    //    _vector leftArmPos = leftArmBaseMat.r[3];
    //    _matrix leftArmFinalMat = matLeftArmRot;
    //    //leftArmFinalMat.r[3] = leftArmPos;
    //    pLeftArm->UpdateTransformationMatrix(leftArmFinalMat);
    //}

    //UpdateAttackAnimation(fTimeDelta);

    if(false)
    {
        constexpr float fScale = 0.0625f;

        if (auto pRoot = m_pComEntityModel->GetBone("root"))
        {
            _matrix matRot =
                XMMatrixRotationX(
                    XMConvertToRadians(28.f));

            _matrix matPos =
                XMMatrixTranslation(
                    0.f,
                    1.25f * fScale,
                    -9.f * fScale);

            pRoot->UpdateTransformationMatrix(
                matRot * matPos);
        }

        if (auto pBody = m_pComEntityModel->GetBone("body"))
        {
            _matrix matPos =
                XMMatrixTranslation(
                    0.f,
                    -2.f * fScale,
                    0.f);

            pBody->UpdateTransformationMatrix(matPos);
        }

        if (auto pHead = m_pComEntityModel->GetBone("head"))
        {
            auto tmp = XMLoadFloat4x4(pHead->GeTransformationMatrix());
            _matrix matPos =
                XMMatrixTranslation(
                    0.f,
                    -1.f * fScale,
                    0.f);

            pHead->UpdateTransformationMatrix(tmp * matPos );
        }

        if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
        {
            _matrix matRot =
                XMMatrixRotationX(
                    XMConvertToRadians(-5.7f));

            pLeftArm->UpdateTransformationMatrix(matRot);
        }

        if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
        {
            _matrix matRot =
                XMMatrixRotationX(
                    XMConvertToRadians(-5.7f));

            pRightArm->UpdateTransformationMatrix(matRot);
        }

        if (auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg"))
        {
            _matrix matRot =
                XMMatrixRotationX(
                    XMConvertToRadians(-28.f));

            pLeftLeg->UpdateTransformationMatrix(matRot);
        }

        if (auto pRightLeg = m_pComEntityModel->GetBone("rightLeg"))
        {
            _matrix matRot =
                XMMatrixRotationX(
                    XMConvertToRadians(-28.f));

            pRightLeg->UpdateTransformationMatrix(matRot);
        }
    }
    if (0)
    {
        static float fTmp2 = 0;
        fTmp2 += fTimeDelta;;
        float lifeTime = fTmp2;
        float bob = (cosf(lifeTime * XMConvertToRadians(103.2f)) * 2.865f) + 2.865f;

        if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
        {
            pLeftArm->SetRotation({0,0, XMConvertToRadians(-bob) });
            //_matrix matLeftArmRot = XMMatrixRotationZ(XMConvertToRadians(-bob));
            //pLeftArm->UpdateTransformationMatrix(matLeftArmRot);
        }

        if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
        {
            pRightArm->SetRotation({ 0,0, XMConvertToRadians(bob) });
            //_matrix matRightArmRot = XMMatrixRotationZ(XMConvertToRadians(bob));
            //pRightArm->UpdateTransformationMatrix(matRightArmRot);
        }
    }

    if(false)
    {
        bool bMoving = CGameInstance::Get().KeyPressing(DIK_W) ||
            CGameInstance::Get().KeyPressing(DIK_S) ||
            CGameInstance::Get().KeyPressing(DIK_A) ||
            CGameInstance::Get().KeyPressing(DIK_D);

        static float fWalkCycle = 0.f;

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
                _matrix matLeftArmRot = XMMatrixRotationX(XMConvertToRadians(tcos0));
                _matrix leftArmBaseMat = XMLoadFloat4x4(pLeftArm->GeTransformationMatrix());
                _vector leftArmPos = leftArmBaseMat.r[3];
                _matrix leftArmFinalMat = matLeftArmRot;
                //leftArmFinalMat.r[3] = leftArmPos;
                pLeftArm->UpdateTransformationMatrix(leftArmFinalMat);
            }

            if (auto pLeftLeg = m_pComEntityModel->GetBone("leftLeg"))
            {
                _matrix matLeftLegRot = XMMatrixRotationX(XMConvertToRadians(tcos0 * -1.4f));
                _matrix leftLegBaseMat = XMLoadFloat4x4(pLeftLeg->GeTransformationMatrix());
                _vector leftLegPos = leftLegBaseMat.r[3];
                _matrix leftLegFinalMat = matLeftLegRot;
                //leftLegFinalMat.r[3] = leftLegPos;
                pLeftLeg->UpdateTransformationMatrix(leftLegFinalMat);
            }

            if (auto pRightArm = m_pComEntityModel->GetBone("rightArm"))
            {
                _matrix matRightArmRot = XMMatrixRotationX(XMConvertToRadians(-tcos0));
                _matrix rightArmBaseMat = XMLoadFloat4x4(pRightArm->GeTransformationMatrix());
                _vector rightArmPos = rightArmBaseMat.r[3];
                _matrix rightArmFinalMat = matRightArmRot;
                //rightArmFinalMat.r[3] = rightArmPos;
                pRightArm->UpdateTransformationMatrix(rightArmFinalMat);
            }

            if (auto pRightLeg = m_pComEntityModel->GetBone("rightLeg"))
            {
                _matrix matRightLegRot = XMMatrixRotationX(XMConvertToRadians(tcos0 * 1.4f));
                _matrix rightLegBaseMat = XMLoadFloat4x4(pRightLeg->GeTransformationMatrix());
                _vector rightLegPos = rightLegBaseMat.r[3];
                _matrix rightLegFinalMat = matRightLegRot;
                //rightLegFinalMat.r[3] = rightLegPos;
                pRightLeg->UpdateTransformationMatrix(rightLegFinalMat);
            }
        }
        else
        {
            // Idle 상태 - 애니메이션 적용 안함
        }
   }
}

HRESULT CPlayerEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    if (m_eCameraType == CAMERA_TYPE::FPS)
    {
        return S_OK;
    }

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
    return S_OK;
}

void CPlayerEntity::UpdateAttackAnimation(float fTimeDelta)
{
    //if (!m_bAttacking) return;

    // attack_time: 0.0 ~ 1.0
    m_fAttackTime += fTimeDelta / m_fAttackDuration;
    m_fAttackTime = std::min(m_fAttackTime, 1.0f);

    float attackTime = m_fAttackTime;
    float attackBodyRotY = m_vHeadRotation.y;  // 현재 head 회전값

    // body 회전
    if (auto pBody = m_pComEntityModel->GetBone("body"))
    {
        _matrix matBodyRot = XMMatrixRotationY(XMConvertToRadians(attackBodyRotY));
        _matrix bodyBaseMat = XMLoadFloat4x4(pBody->GeTransformationMatrix());
        _vector bodyPos = bodyBaseMat.r[3];
        _matrix bodyFinalMat = matBodyRot;
        //bodyFinalMat.r[3] = bodyPos;
        pBody->UpdateTransformationMatrix(bodyFinalMat);
    }

    // leftarm 회전
    if (auto pLeftArm = m_pComEntityModel->GetBone("leftArm"))
    {
        float t = 1.0f - pow(1.0f - attackTime, 4.0f);
        float leftArmRotX = -(sin(t * 3.14159f) * 1.2f + sin(attackTime * 3.14159f)) * 10.0f;

        _matrix matLeftArmRot = XMMatrixRotationX(XMConvertToRadians(leftArmRotX));
        _matrix leftArmBaseMat = XMLoadFloat4x4(pLeftArm->GeTransformationMatrix());
        _vector leftArmPos = leftArmBaseMat.r[3];
        _matrix leftArmFinalMat = matLeftArmRot;
        //leftArmFinalMat.r[3] = leftArmPos;
        pLeftArm->UpdateTransformationMatrix(leftArmFinalMat);
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

        _matrix matRightArmRot =
            XMMatrixRotationX(XMConvertToRadians(rightArmRotX)) *
            XMMatrixRotationY(XMConvertToRadians(rightArmRotY));

        _matrix rightArmBaseMat = XMLoadFloat4x4(pRightArm->GeTransformationMatrix());
        _vector rightArmPos = rightArmBaseMat.r[3];
        _matrix rightArmFinalMat = matRightArmRot;
        //rightArmFinalMat.r[3] = rightArmPos;
        pRightArm->UpdateTransformationMatrix(rightArmFinalMat);
    }

    // 공격 완료
    if (m_fAttackTime >= 1.0f)
    {
        m_bAttacking = false;
        m_fAttackTime = 0.0f;
    }
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
