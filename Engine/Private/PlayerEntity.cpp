#include "PlayerEntity.h"

#include "Resources.h"
#include "GameInstance.h"
#include "ComEntityModel.h"

#include "CollSphere.h"
#include "CollBox.h"

#include "DestroyStage.h"

#include "VoxelManager3.h"

#include "UIController.h"

#include "DropItem.h"
#include "DropBlock.h"

#include "UIItem.h"

#include "FurnaceStorage.h"
#include "ChestStorage.h"

#include "PigEntity.h"
#include "CowEntity.h"
#include "ChickenEntity.h"
#include "SkeletonEntity.h"
#include "ZombieEntity.h"
#include "CreeperEntity.h"
#include "SpiderEntity.h"
#include "EnderManEntity.h"
#include "EnderDragonEntity.h"

#include "HandHeldBlock.h"
#include "HandHeldItem.h"

#include "PlayerFPSArm.h"

#include "ArmorEntity.h";

#include "ExperienceOrb.h"

#include "ActivatedTNT.h"
#include "ArrowEntity.h"


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
    ImGui::Text("PLAY: %i", m_ePlay);
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

        ImGui::Text("NumKeyPressing_1: %i", m_bNumKeyPressing[1]);
        ImGui::Text("NumKeyPressing_2: %i", m_bNumKeyPressing[2]);
        ImGui::Text("NumKeyPressing_3: %i", m_bNumKeyPressing[3]);
        ImGui::Text("NumKeyPressing_4: %i", m_bNumKeyPressing[4]);
        ImGui::Text("NumKeyPressing_5: %i", m_bNumKeyPressing[5]);
        ImGui::Text("NumKeyPressing_6: %i", m_bNumKeyPressing[6]);
        ImGui::Text("NumKeyPressing_7: %i", m_bNumKeyPressing[7]);
        ImGui::Text("NumKeyPressing_8: %i", m_bNumKeyPressing[8]);
        ImGui::Text("NumKeyPressing_9: %i", m_bNumKeyPressing[9]);

        ImGui::TreePop();
    }

    if (ImGui::Button("spawnPig"))
    {
        {
        	E::CPigEntity::DESC Desc{};
        	Desc.sObjectTag = "Pig";
        	if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PigEntity",
        		"02_TEST_ENTITY", &Desc))
        	{
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CPigEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    pObj->SetPlayer(GetHandle());
                }
        	}
        }
    }
    if (ImGui::Button("spawnCow"))
    {
        {
            E::CCowEntity::DESC Desc{};
            Desc.sObjectTag = "Cow";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CowEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CCowEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnChicken"))
    {
        {
            E::CChickenEntity::DESC Desc{};
            Desc.sObjectTag = "Chicken";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ChickenEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CChickenEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnSkeleton"))
    {
        {
            E::CSkeletonEntity::DESC Desc{};
            Desc.sObjectTag = "Skeleton";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_SkeletonEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CSkeletonEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnZombie"))
    {
        {
            E::CZombieEntity::DESC Desc{};
            Desc.sObjectTag = "Skeleton";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ZombieEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CZombieEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnCreeper"))
    {
        {
            E::CCreeperEntity::DESC Desc{};
            Desc.sObjectTag = "Creeper";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CreeperEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CCreeperEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnSpider"))
    {
        {
            E::CSpiderEntity::DESC Desc{};
            Desc.sObjectTag = "Spider";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_SpiderEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CSpiderEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnEnderMan"))
    {
        {
            E::CEnderManEntity::DESC Desc{};
            Desc.sObjectTag = "EnderMan";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_EnderManEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CEnderManEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawnEnderDragon"))
    {
        {
            E::CEnderDragonEntity::DESC Desc{};
            Desc.sObjectTag = "EnderDragon";
            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_EnderDragonEntity",
                "02_TEST_ENTITY", &Desc))
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CEnderDragonEntity>(handle.value()))
                {
                    pObj->GetTransform().SetPosition(GetTransform().GetLoadedPostion());
                    //pObj->SetPlayer(GetHandle());
                }
            }
        }
    }

    if (ImGui::Button("spawn activatdTNT"))
    {
        //
        if (auto pObj = CGameInstance::Get().GetFirstGameObjectByLayer<CActivatedTNT>("89_ACTIVATED_TNT"))
        {

            auto pos = GetTransform().GetPosition();
            //pos.x += 1.f;
            pos.y += 1.f;
            CActivatedTNT::SActivatedTNTData data{};
            data.vPos = pos;
            pObj->AddBlock(data);
            //pObj->AddOrb(pos, {}, rand() % 16);

        }
    }

    if (ImGui::Button("Test ExpOrb"))
    {
        //56_ExperienceOrb
        if (auto pObj = CGameInstance::Get().GetFirstGameObjectByLayer<CExperienceOrb>("56_ExperienceOrb"))
        {
            auto pos = GetTransform().GetPosition();
            pos.x += 7.f + Randf(-1.f, 1.f);
            pos.z += 7.f + Randf(-1.f, 1.f);
            pObj->AddOrb(pos, {}, rand() % 16, Randf(0.3f, 1.f));
        }
    }

    if (ImGui::Button("Test Drop Item"))
    {
        if(1)
        {
            CItemObject::ItemInfo ItemInfo{};
            //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_WoodPickaxe;
            ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;

            //CUIItem::GetPerUIByType(ItemInfo);

            E::CDropItem::DESC Desc{};
            Desc.sObjectTag = "DropItem_WoodPickaxe";
            Desc.viBufferId = { "MC_ITEM_VIBuffer", CDropItemObject::GetVIBufferName(ItemInfo) };
            if (auto woodPixaxeHandle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
                CDropItemObject::GetDropItemLayer(ItemInfo), &Desc))
            {
               
                   
                //playerObj->SetRightItemHandle(woodPixaxeHandle);
                if (auto woodPixaxeObj = E::CGameInstance::Get().GetGameObjectByHandleT<CDropItem>(woodPixaxeHandle.value()))
                {
                    CItemObject::ItemInfo ItemInfo{};
                    //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_WoodPickaxe;
                    ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;
                    auto pos = GetTransform().GetPosition();
                    pos.x += 1.f;
                    ;
                    woodPixaxeObj->AddDropItemObject(ItemInfo, pos, {}, { CItemObject::GetPackedTexIdByType(CItemObject::ITEM_TYPE::ITEM_WoodPickaxe) });
                    pos.x += 1.f;

                    //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_CopperPickaxe;
                    ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_CopperPickaxe;
                    woodPixaxeObj->AddDropItemObject(ItemInfo, pos, {}, { CItemObject::GetPackedTexIdByType(CItemObject::ITEM_TYPE::ITEM_CopperPickaxe) });
                    pos.x += 1.f;

                    //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_WoodPickaxe;
                    ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;
                    woodPixaxeObj->AddDropItemObject(ItemInfo, pos, {}, { CItemObject::GetPackedTexIdByType(CItemObject::ITEM_TYPE::ITEM_WoodPickaxe) });
                    pos.x += 1.f;
                    woodPixaxeObj->AddDropItemObject(ItemInfo, pos, {}, { CItemObject::GetPackedTexIdByType(CItemObject::ITEM_TYPE::ITEM_WoodPickaxe) });

                    //woodPixaxeObj->GetTransform().SetPosition(XMVectorSet(0.f, 0.2f, 0.4f, 1.f));
                    //woodPixaxeObj->GetTransform().SetRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -90);
                    //woodPixaxeObj->SetParentNode(playerObj);
                }
            }
        }
        //if (0)
        //{
        //    E::CDropBlock::DESC Desc{};
        //    Desc.sObjectTag = "CDropBlock_CubeItemDirt";
        //    Desc.viBufferId = { "MC_ITEM_VIBuffer", "CubeItemDirt" };
        //    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropBlock",
        //        "01_DROPITEM", &Desc))
        //    {
        //        if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandleT<CDropBlock>(handle.value()))
        //        {
        //           
        //            auto pos = GetTransform().GetPosition();
        //            /*              tmp.x += 1.f;
        //                          woodPixaxeObj->GetTransform().SetPosition(tmp);*/

        //            std::vector a{ PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) ,PackTexId(9, 0) };
        //            pObj->AddDropItemObject(pos, {0.f, 4.f, 0.f}, a);
        //        }
        //    }
        //}
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
    m_RenderPassFlags = ETOUI(RENDERPASS::DEFAULT) | ETOUI(RENDERPASS::PLAYER_INVEN_UI);

    auto* pDesc = static_cast<DESC*>(pArg);
    if (FAILED(CPlayerEntityObject::Initialize(pArg)))
    {
        return E_FAIL;
    }

   
    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Steve" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Steve" };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_EntityModel", "Com_EntityModel", &Desc, &m_pComEntityModel)))
        {
            return E_FAIL;
        };
    }

    //
    {
        CComEntityModel::DESC Desc{};
        Desc.viBufferId = { "MC_ENTITY_VIBuffer", "Steve" };
        Desc.geometryId = { "MC_ENTITY_GEOMETRY", "Steve" };
        if (FAILED(AddComponentFromProto("PERMANENT", "Prototype_Component_EntityModel", "Com_InventoryPlayerEntityModel", &Desc, &m_pComInventoryPlayerEntityModel)))
        {
            return E_FAIL;
        };
    }
    
   
    //m_pCenterCollider = CCollSphere::Create({0.f, 0.3f, 0.f}, 0.3f);
    m_pCenterCollider = CCollBox::Create({ 0.f, 1.f, 0.f }, { 0.25f, 0.9f, 0.25f });
    m_pCenterCollider->SetInnerPointer(this);

    m_pMeleeAttackCollider = CCollBox::Create({ 0.f, 0.f, 0.f }, { 0.5f, 0.5f, 0.5f });

    ReadyPlayerItem();

    // TEST
    CItemObject::RecipeInitialize();

    {
        {
            E::CPlayerFPSArm::DESC Desc{};
            Desc.sObjectTag = "PlayerFPSArm";
            if (auto fpsArmHandle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_PlayerFPSArm",
                "10_PlayerArm", &Desc))
            {
                m_hPlayerFPSArm = fpsArmHandle.value();
                //playerObj->SetRightItemHandle(fpsArmHandle);
                if (auto fpsArmObj = E::CGameInstance::Get().GetGameObjectByHandle(fpsArmHandle.value()))
                {
                    fpsArmObj->GetTransform().SetPosition(XMVectorSet(0.1f, -0.15f, 0.05f, 1.f));

                    fpsArmObj->GetTransform().AddQuaternion(XMQuaternionRotationRollPitchYaw(
                        XMConvertToRadians(-90.f),
                        XMConvertToRadians(30.f),
                        XMConvertToRadians(0.f)
                    ));

                    //fpsArmObj->GetTransform().SetScale(XMVectorSet(0.3f, 0.3f, 0.3f, 1.f));
                }
            }
        }
    }
    return S_OK;
}

void CPlayerEntity::PriorityUpdate(E::_float fTimeDelta)
{
    
    m_pPlayerCamera = Cast<CPlayerCamera>(CGameInstance::Get().GetGameCamera("Player"));
    m_pActivePlayerCamera = Cast<CPlayerCamera>(CGameInstance::Get().GetActiveGameCamera("Player"));
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
        m_bNumKeyPressing[1] = CGameInstance::Get().KeyPressing(DIK_1);
        m_bNumKeyPressing[2] = CGameInstance::Get().KeyPressing(DIK_2);
        m_bNumKeyPressing[3] = CGameInstance::Get().KeyPressing(DIK_3);
        m_bNumKeyPressing[4] = CGameInstance::Get().KeyPressing(DIK_4);
        m_bNumKeyPressing[5] = CGameInstance::Get().KeyPressing(DIK_5);
        m_bNumKeyPressing[6] = CGameInstance::Get().KeyPressing(DIK_6);
        m_bNumKeyPressing[7] = CGameInstance::Get().KeyPressing(DIK_7);
        m_bNumKeyPressing[8] = CGameInstance::Get().KeyPressing(DIK_8);
        m_bNumKeyPressing[9] = CGameInstance::Get().KeyPressing(DIK_9);

        m_bKeyDownE = CGameInstance::Get().KeyDown(DIK_E);
        m_bKeyDownQ = CGameInstance::Get().KeyDown(DIK_Q);
        m_bMouseDownLeft = CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB);
        m_bMouseDownRight = CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB);
        m_bMouseUpRight = CGameInstance::Get().MouseUp(MOUSEKEYSTATE::RB);
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
        memset(&m_bNumKeyPressing, 0, sizeof(m_bNumKeyPressing));
        m_bKeyDownE = false;
        m_bKeyDownQ = false;
        m_bMouseDownLeft = false;
        m_bMouseDownRight = false;
        m_bMouseUpRight = false;
    }

}

void CPlayerEntity::Update(E::_float fTimeDelta)
{
    if (m_bDeath)
    {
        GetUIController()->GetDeathScreen()->SetRender(true);

        //m_fDeathAnimTimer

        {

            m_fDeathAnimTimer += fTimeDelta;

            m_pComEntityModel->ResetBonesChannel();
            m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);

            constexpr float FALL_DURATION = 0.5f;
            constexpr float TOTAL_DURATION = 1.5f;

            float fFallT = std::min(m_fDeathAnimTimer / FALL_DURATION, 1.0f);
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

            //PlayerMove(fTimeDelta);
            if (m_fDeathAnimTimer <= TOTAL_DURATION)
            {
                CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
            }
            
            GetTransform().Update();
            //VelocityUpdate(fTimeDelta, XMVectorZero());
            if (m_fDeathAnimTimer >= TOTAL_DURATION && m_fDeathAnimTimer <= TOTAL_DURATION + 0.02f)
            {
                CGameInstance::Get().AddParticleRenderDeathSmoke(GetTransform().GetPosition(), 10);
            }
        }
    }
    else
    {
        ProcessHealthRegenTimer(fTimeDelta);
        ProcessHungerTimer(fTimeDelta);

        JudgeDeathUpdate(fTimeDelta);

        GetUIController()->GetDeathScreen()->SetRender(false);

        ProcessActionUpdate(fTimeDelta);

        ProcessThrowItem(fTimeDelta);
        ProcessHandHeldItem(fTimeDelta);

        ProcessLeftClick(fTimeDelta);
        ProcessRightClick(fTimeDelta);


        ProcessArmorEntities(fTimeDelta);
        ProcessPlayerOpenInvenArmorEntities(fTimeDelta);
        ProcessUI(fTimeDelta);
    }
    


}



void CPlayerEntity::LateUpdate(E::_float fTimeDelta)
{
    //JudgeDeathUpdate(fTimeDelta);
    if (m_bDeath)
    {
        return;
    }
    CGameInstance::Get().AddRenderObject(RENDERGROUP::NONBLEND, this);
    
    GetTransform().Update();

    m_pComEntityModel->UpdateBoneMatrix(fTimeDelta);
    m_pComInventoryPlayerEntityModel->UpdateBoneMatrix(fTimeDelta);
    E::CGameInstance::Get().AddColliderGroup("Coll_PlayerCenter", m_pCenterCollider.get());
    m_pCenterCollider->Transform(GetTransform().GetLoadedWorldMatrix());

    //if (E::CGameInstance::Get().MousePressing(MOUSEKEYSTATE::LB))
    //{
    //    if (m_pActivePlayerCamera)
    //    {
    //        E::CGameInstance::Get().AddColliderGroup("Coll_PlayerMeleeAttack", m_pMeleeAttackCollider.get());
    //        //_float3 startPos{};
    //        //XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
    //        auto currPos = GetTransform().GetState(STATE::POSITION);
    //        currPos = currPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f);
    //        auto lookDir = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
    //        auto targetPos = currPos + lookDir;
    //        auto mat = XMMatrixTranslationFromVector(targetPos);
    //        m_pMeleeAttackCollider->Transform(mat);
    //    }
    //    
    //}

    if (E::CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
    {
        if (m_pActivePlayerCamera)
        {
            // 1. 이번 프레임에 플레이어 근접 공격 그룹 활성화
            E::CGameInstance::Get().AddColliderGroup("Coll_PlayerMeleeAttack", m_pMeleeAttackCollider.get());

            // 2. 플레이어 발바닥 기준 위치 가져오기
            auto currPos = GetTransform().GetState(STATE::POSITION);

            // 3. 눈높이(카메라 높이 약 1.62f ~ 1.8f) 보정
            currPos = currPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f);

            // 4. 카메라 시선 방향 및 공격 리치(거리) 계산
            auto lookDir = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
            float fAttackReach = 2.0f; // 2.5블록 거리까지 리치 확장

            // 5. 최종 월드 공격 중심점 계산 및 행렬 주입
            auto targetPos = currPos + (lookDir * fAttackReach);
            auto mat = XMMatrixTranslationFromVector(targetPos);

            m_pMeleeAttackCollider->Transform(mat);
        }
    }

   

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
    ProcessItemColliding(fTimeDelta);
    ProcessExpOrbColliding(fTimeDelta);

    if (auto pCollGroup = CGameInstance::Get().GetColliderGroup("Coll_PigCenter"))
    {
        if (auto pMeleeCollGroup = CGameInstance::Get().GetColliderGroup("Coll_PlayerMeleeAttack"))
        {
            auto meleecoll = pMeleeCollGroup->front();
            for (auto& pColl : *pCollGroup)
            {
                if (CGameInstance::Get().IntersectColl(pColl, meleecoll))
                {
                    if (auto pObj = Cast<CPigEntity>(pColl->GetInnerPointer()))
                    {
                        pObj->TakeDamage(1);
                        //auto pHint = static_cast<CDropBlock::CollHint*>(pColl->GetInnerHint2());
                        //const auto& itemInfo = pHint->iter->itemInfo;
                        //auto* pUIController = GetUIController();

                        ////if (SUCCEEDED(pUIController->Getinventory()->AddItemToInventory(itemInfo)))
                        ////{
                        ////    pObj->GetDropItemObjects().erase(pHint->iter);
                        ////}

                        //if (SUCCEEDED(ProcessItemGain(itemInfo)))
                        //{
                        //    pObj->GetDropItemObjects().erase(pHint->iter);
                        //}
                    }
                }
            }
        }
        
    }

}

HRESULT CPlayerEntity::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    switch (ctx.pass)
    {
    case RENDERPASS::DEFAULT:
        return RenderDefault(pContext, ctx);
    case RENDERPASS::SHADOW:
        return RenderShadow(pContext, ctx);
    case RENDERPASS::PLAYER_INVEN_UI:
        return RenderPlayerInvenUI(pContext, ctx);
    }
}

HRESULT CPlayerEntity::RenderDefault(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
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

        m_pComEntityModel->BindBoneMatrix(pContext);

        m_pComEntityModel->Render(pContext, ctx);
    }

    return S_OK;
}

HRESULT CPlayerEntity::RenderShadow(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

HRESULT CPlayerEntity::RenderPlayerInvenUI(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    {
        auto pCbPerObject = E::CGameInstance::Get().GetResourceFirst<E::CResCBuffer>(TAG_RES_GRP_PERMANENT_BUFFER, "CB_PerObject");
        D3D11_MAPPED_SUBRESOURCE mappedSubResource;
        if (SUCCEEDED(pContext->Map(pCbPerObject->GetCBuffer().Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource)))
        {
            ;
            E::CB_PER_OBJECT cbPerObject{};
            XMStoreFloat4x4(&cbPerObject.matWorld, XMMatrixIdentity());
            //cbPerObject.matWorld = *GetTransform().GetWorldMatrix();
            XMStoreFloat4x4(&cbPerObject.matWVP, XMLoadFloat4x4(&cbPerObject.matWorld) * ctx.matViewProj);

            memcpy(mappedSubResource.pData, &cbPerObject, sizeof(cbPerObject));
            pContext->Unmap(pCbPerObject->GetCBuffer().Get(), 0);
        }
        pContext->VSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
        pContext->PSSetConstantBuffers(0, 1, pCbPerObject->GetCBuffer().GetAddressOf());
    }
    
    m_pComInventoryPlayerEntityModel->BindBoneMatrix(pContext);

    m_pComInventoryPlayerEntityModel->Render(pContext, ctx);
    return S_OK;
}

void CPlayerEntity::AddRenderPassPlayerInvenUIPass()
{
    if (auto cam = CGameInstance::Get().GetGameCamera("PlayerInvenUI"))
    {
        CGameInstance::Get().RendererDrawPlayerInvenUIPass();
        //auto playerPos = GetTransform().GetPosition();
        //auto camPos = playerPos;
        //camPos.z += 4.f;
        //cam->GetTransform().SetPosition(XMLoadFloat3(&camPos));
        //cam->GetTransform().LookAt(XMLoadFloat3(&playerPos));
    }
}

void CPlayerEntity::ProcessDestroyStage(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pDestroyStage = GetDestroyStage();
    if (!pDestroyStage) return;

    static float fElapsed = 0;
    

    //
    
    //CGameInstance::Get().VoxelBlockRaycast()
    
    const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
    auto pos = rayOrigin2;
    if (m_eCameraType != CAMERA_TYPE::FPS)
    {
        pos = GetTransform().GetPosition();
        pos.y += 1.8f;
    }

    std::optional<std::pair<XMINT3, uint8_t>> currDestoryTarget{};
    CVoxelManager3::BLOCK_RAY_RESULT res;
    if (CGameInstance::Get().VoxelBlockRaycast(pos, rayDir2, 5.f, res))
    {
        if (res.block)
        {
            currDestoryTarget = { XMINT3{res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ}, static_cast<uint8_t>(res.block.value().GetType())};
            pDestroyStage->GetTransform().SetPosition(_float3{ (float)res.iWorldBlockX, (float)res.iWorldBlockY, (float)res.iWorldBlockZ });
        }
        else
        {
            currDestoryTarget = std::nullopt;
        }
    }
    else
    {
        currDestoryTarget = std::nullopt;
    }
    auto eq = [](const std::optional<std::pair<XMINT3, uint8_t>>& a,
        const std::optional<std::pair<XMINT3, uint8_t>>& b) {
            if (a.has_value() != b.has_value()) return false;
            if (!a.has_value()) return true;
            const auto& [pa, pb] = *a;
            const auto& [qa, qb] = *b;
            return pa.x == qa.x && pa.y == qa.y && pa.z == qa.z && pb == qb;
        };

    if (!eq(currDestoryTarget, m_DestoryStageRaycastTarget)) 
    { 
        m_DestoryStageRaycastTarget = currDestoryTarget;
        m_bDestoryStageStart = false;
    }
   
    if (!m_DestoryStageRaycastTarget)
    {
        pDestroyStage->SetRender(false);
        m_bDestoryStageStart = false;
        return;
    }

    m_pActivePlayerCamera->GetRay();
    if (m_bMousePressingLeft)
    {
        if (!m_bDestoryStageStart)
        {
            m_bDestoryStageStart = true;

            pDestroyStage->SetRender(true);
            fElapsed = 0;
        }

        fElapsed += fTimeDelta;

        float blockDestroyRate = 1.f;
        float blockDestroyTime = 0.5f;
        float goal = blockDestroyTime * blockDestroyRate;

        pDestroyStage->SetFrameIndex(uint32_t(fElapsed / (goal / 9.f)));

        if (res.block)
        {
            auto currX = res.iWorldBlockX;
            auto currY = res.iWorldBlockY;
            auto currZ = res.iWorldBlockZ;
            switch (res.eHitFace)
            {
            case FACE_DIR::POS_X:  currX += 1; break; // +X 면을 쳤으니 오른쪽 공기 칸
            case FACE_DIR::NEG_X:  currX -= 1; break; // -X 면을 쳤으니 왼쪽 공기 칸
            case FACE_DIR::POS_Y:  currY += 1; break; // +Y 면(윗면)을 쳤으니 위쪽 공기 칸
            case FACE_DIR::NEG_Y:  currY -= 1; break; // -Y 면(밑면)을 쳤으니 아래쪽 공기 칸
            case FACE_DIR::POS_Z:  currZ += 1; break; // +Z 면(앞면)을 쳤으니 앞쪽 공기 칸
            case FACE_DIR::NEG_Z:  currZ -= 1; break; // -Z 면(뒷면)을 쳤으니 뒤쪽 공기 칸
            default: break;
            }

            if (auto optLightBlock = CGameInstance::Get().GetVoxelBlock(currX, currY, currZ))
            {
                pDestroyStage->SetLight(optLightBlock->GetLight());
            }

            const auto&[off, halfExt] = CBlock3::GetOutlineExtents(res.block->GetType());

            _float3 vOri = {
                0.5f + off.x - halfExt.x,
                0.5f + off.y - halfExt.y,
                0.5f + off.z - halfExt.z,
            };
            _float3 vExt = {
                halfExt.x * 2.f,
                halfExt.y * 2.f,
                halfExt.z * 2.f,
            };

            //MakeCubeQuads(vOri, vExt);
            pDestroyStage->MakeCubeQuads(vOri, vExt);
        }
        
        
        //res.block.value(), 
        //texId = 
        _float4 vColor = { 1.f, 1.f, 1.f, 1.f };
        uint32_t baseColorABGR = CBlock3::GetBaseColor(res.block.value().GetType());
        if (baseColorABGR != 0xFF)
        {
            _float4 blockTint;
            blockTint.x = ((baseColorABGR >> 0) & 0xFF) / 255.f; // R
            blockTint.y = ((baseColorABGR >> 8) & 0xFF) / 255.f; // G
            blockTint.z = ((baseColorABGR >> 16) & 0xFF) / 255.f; // B
            blockTint.w = ((baseColorABGR >> 24) & 0xFF) / 255.f; // A
           vColor = blockTint;
        }
        uint32_t iPackedTexID = PackTexId(9, static_cast<uint32_t>(CBlock3::GetTexType(res.block.value().GetType(), FACE_DIR::POS_X)));
        CGameInstance::Get().AddParticleRenderDestruct(
            { (float)res.iWorldBlockX + 0.5f, (float)res.iWorldBlockY + 0.5f, (float)res.iWorldBlockZ + 0.5f },
            iPackedTexID,
            1,
            vColor);

        if (pDestroyStage->GetFrameIndex() == 9)
        {
            CGameInstance::Get().AddParticleRenderDestruct(
                { (float)res.iWorldBlockX + 0.5f, (float)res.iWorldBlockY + 0.5f, (float)res.iWorldBlockZ + 0.5f },
                iPackedTexID,
                1,
                vColor);

            CBlock3 newBlock{};
            newBlock.SetType(CBlock3::TYPE::AIR);
            CGameInstance::Get().VoxelProcessPlayerBlockSet(res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ, newBlock);

            //CGameInstance::Get().VoxelProcessExplodeBlock(res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ, 3.f);
        

            CItemObject::ItemInfo ItemInfo{};
            ItemInfo.block = res.block;
            ItemInfo.iCnt = 1;

            CItemObject::DestoryBlockAfterProcess(ItemInfo, XMINT3{ res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ });
            CItemObject::DestoryBlockItemConverter(ItemInfo);
/*            DestroyStageEndAfterProcess(ItemInfo, XMINT3{ res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ });
            DestroyStageEndItemConverter(ItemInfo);*/
            

        
            

            auto pos = _float3{ (float)res.iWorldBlockX, (float)res.iWorldBlockY,  (float)res.iWorldBlockZ };
            pos.x += 0.5f;
            pos.y += 0.5f;
            pos.z += 0.5f;

            CItemObject::SpawnDropItemObject(ItemInfo, pos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
        }
    }
    else
    {
        pDestroyStage->SetRender(false);
        m_bDestoryStageStart = false;
    }
}
//
//void CPlayerEntity::DestroyStageEndAfterProcess(const CItemObject::ItemInfo& info, const XMINT3& wbLocatoin)
//{
//    switch (info.block->GetType())
//    {
//    case CBlock3::TYPE::FURNACE:
//    {
//        auto pStorage = CGameInstance::Get().GetWorldFurnaceStorage()->GetStorage(wbLocatoin);
//        _float3 wbFloat = { (float)wbLocatoin.x,(float)wbLocatoin.y, (float)wbLocatoin.z };
//        wbFloat.x += 0.5f;
//        wbFloat.y += 0.5f;
//        wbFloat.z += 0.5f;
//        if (pStorage->fuel)
//        {
//            for (uint32_t i = 0; i < pStorage->fuel->iCnt; ++i)
//            {
//                auto copy = pStorage->fuel.value();
//                copy.iCnt = 1;
//                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
//            }
//        }
//        if (pStorage->ingredient)
//        {
//            for (uint32_t i = 0; i < pStorage->ingredient->iCnt; ++i)
//            {
//                auto copy = pStorage->ingredient.value();
//                copy.iCnt = 1;
//                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
//            }
//        }
//        if (pStorage->result)
//        {
//            for (uint32_t i = 0; i < pStorage->result->iCnt; ++i)
//            {
//                auto copy = pStorage->result.value();
//                copy.iCnt = 1;
//                CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
//            }
//        }
//        CGameInstance::Get().GetWorldFurnaceStorage()->DelStorage(wbLocatoin);
//    }
//        return;
//
//    case CBlock3::TYPE::CHEST:
//    {
//        auto pStorage = CGameInstance::Get().GetWorldChestStorage()->GetStorage(wbLocatoin);
//        _float3 wbFloat = { (float)wbLocatoin.x,(float)wbLocatoin.y, (float)wbLocatoin.z };
//        wbFloat.x += 0.5f;
//        wbFloat.y += 0.5f;
//        wbFloat.z += 0.5f;
//
//        for (uint32_t i = 0; i < pStorage->items.size(); ++i)
//        {
//            if (pStorage->items[i].has_value())
//            {
//                uint32_t cnt = pStorage->items[i]->iCnt;
//                for (uint32_t j = 0; j < cnt; ++j)
//                {
//                    auto copy = pStorage->items[i].value();
//                    copy.iCnt = 1;
//                    CItemObject::SpawnDropItemObject(copy, wbFloat, { 0.f, 2.f, 0.f });
//                }
//            }
//        }
//
//
//        CGameInstance::Get().GetWorldChestStorage()->DelStorage(wbLocatoin);
//    }
//
//        return;
//    }
//}
//
//void CPlayerEntity::DestroyStageEndItemConverter(CItemObject::ItemInfo& info)
//{
//    switch (info.block->GetType())
//    {
//    case CBlock3::TYPE::GRASS:
//        info.block->SetType(CBlock3::TYPE::DIRT);
//        return;
//    case CBlock3::TYPE::STONE:
//        info.block->SetType(CBlock3::TYPE::COBBLESTONE);
//        return;
//    case CBlock3::TYPE::STONE_COAL_ORE:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Coal;
//        return;
//    case CBlock3::TYPE::STONE_COPPER_ORE:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Copper;
//        return;
//    case CBlock3::TYPE::STONE_IRON_ORE:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Iron;
//        return;
//    case CBlock3::TYPE::STONE_GOLD_ORE:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Raw_Gold;
//        return;
//    case CBlock3::TYPE::STONE_DIAMOND_ORE:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Diamond;
//        return;
//    case CBlock3::TYPE::TORCH_ON:
//        info.block = std::nullopt;
//        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Torch;
//        return;
//    }
//}

CDestroyStage* CPlayerEntity::GetDestroyStage() const
{
    return CGameInstance::Get().GetGameObjectByHandleT<CDestroyStage>(m_hDestroyStage);
}

void CPlayerEntity::ProcessLeftClick(float fTimeDelta)
{
    ProcessDestroyStage(fTimeDelta);
}

void CPlayerEntity::ProcessRightClick(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;

    if (false && m_bMousePressingRight)
    {
        auto currHotbarIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
        if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
        {
            if (
                hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby
                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0
                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1
                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2
                )
            {
                if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby)
                {
                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0;
                }
                else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0)
                {
                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1;
                }
                else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1)
                {
                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2;
                }
                else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2)
                {
                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Standby;

                    {
                        E::CArrowEntity::DESC Desc{};
                        Desc.sObjectTag = "Arrow";
                        if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArrowEntity",
                            "49_ARROW", &Desc))
                        {
                            if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CArrowEntity>(handle.value()))
                            {
                                const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
                                auto pos = GetTransform().GetPosition();
                                pos.y += 1.8f;
                                //auto look = GetTransform().GetState(STATE::LOOK);
                                //auto pos = XMLoadFloat3(&rayOrigin2);
                                auto look = XMLoadFloat3(&rayDir2);
                                
                                pObj->Shoot(XMLoadFloat3(&pos), look, 30.f);
                            }
                        }
                    }
                }
                return;
            }
        }
    }
    
    if (m_bMouseDownRight)
    {
        auto currHotbarIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
        if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
        {
            //if (
            //    hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby
            //    || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0
            //    || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1
            //    || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2
            //    )
            //{
            //    if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby)
            //    {
            //        hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0;
            //    }
            //    else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0)
            //    {
            //        hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1;
            //    }
            //    else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1)
            //    {
            //        hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2;
            //    }
            //    else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2)
            //    {
            //        hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Standby;

            //        E::CArrowEntity::DESC Desc{};
            //        Desc.sObjectTag = "Arrow";
            //        if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArrowEntity",
            //            "49_ARROW", &Desc))
            //        {
            //            if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CArrowEntity>(handle.value()))
            //            {
            //                const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
            //                //auto pos = GetTransform().GetState(STATE::POSITION);
            //                //auto look = GetTransform().GetState(STATE::LOOK);
            //                auto pos = XMLoadFloat3(&rayOrigin2);
            //                auto look = XMLoadFloat3(&rayDir2);
            //                pObj->Shoot(pos, look, 50.f);
            //            }
            //        }
            //    }

            //    return;
            //}
        }

        

        const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
        auto pos = rayOrigin2;
        if (m_eCameraType != CAMERA_TYPE::FPS)
        {
            pos = GetTransform().GetPosition();
            pos.y += 1.8f;
        }

        CVoxelManager3::BLOCK_RAY_RESULT res;
        if (CGameInstance::Get().VoxelBlockRaycast(pos, rayDir2, 5.f, res))
        {
            if (res.block)
            {
                switch (res.block->GetType())
                {
                case CBlock3::TYPE::CRAFTING_TABLE:
                    GetUIController()->Getinventory()->SetRender(false);
                    GetUIController()->GetCraftingTable()->SetRender(true);
                    return;
                case CBlock3::TYPE::FURNACE:
                    m_openFurnaceLocation = { {res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ} };
                    GetUIController()->Getinventory()->SetRender(false);
                    GetUIController()->GetBlastFurnace()->SetRender(true);
                    return;
                case CBlock3::TYPE::CHEST:
                    m_openChestLocation = { {res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ} };
                    GetUIController()->Getinventory()->SetRender(false);
                    GetUIController()->GetChest()->SetRender(true);
                    return;
                case CBlock3::TYPE::TNT:
                    if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
                    {
                        if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_FlintAndSteel)
                        {
                            if (auto pObj = CGameInstance::Get().GetFirstGameObjectByLayer<CActivatedTNT>("89_ACTIVATED_TNT"))
                            {
                                CGameInstance::Get().VoxelProcessPlayerBlockSet(res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ, CBlock3(CBlock3::TYPE::AIR));
                                _float3 pos = { (_float)res.iWorldBlockX ,  (_float)res.iWorldBlockY ,  (_float)res.iWorldBlockZ };
                                CActivatedTNT::SActivatedTNTData data{};
                                data.vVelocity = {0.f, 3.f, 0.f};
                                data.vPos = pos;
                                pObj->AddBlock(data);
                            }
                            return;
                        }
                        
                    }
                    
                }
            }

            auto worldBX = res.iWorldBlockX;
            auto worldBY = res.iWorldBlockY;
            auto worldBZ = res.iWorldBlockZ;
            if (res.eHitFace == FACE_DIR::POS_X) worldBX += 1;
            else if (res.eHitFace == FACE_DIR::NEG_X) worldBX -= 1;
            else if (res.eHitFace == FACE_DIR::POS_Y) worldBY += 1;
            else if (res.eHitFace == FACE_DIR::NEG_Y) worldBY -= 1;
            else if (res.eHitFace == FACE_DIR::POS_Z) worldBZ += 1;
            else if (res.eHitFace == FACE_DIR::NEG_Z) worldBZ -= 1;

            if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
            {
                if (hotbarItemInfo->block)
                {
                    CGameInstance::Get().VoxelProcessPlayerBlockSet(worldBX, worldBY, worldBZ, hotbarItemInfo->block.value());

                    if (hotbarItemInfo->iCnt <= 1)
                    {
                        hotbarItemInfo = std::nullopt;
                    }
                    else
                    {
                        hotbarItemInfo->iCnt -= 1;
                    }
                    
                }
                else
                {
                    //TODO NotBlock
                    switch (hotbarItemInfo->eItemType)
                    {
                    case CItemObject::ITEM_TYPE::ITEM_Torch:
                    {

                        auto newBlock = CBlock3(CBlock3::TYPE::TORCH_ON);
                        newBlock.SetBlockLight(CBlock3::GetBlockLightByType(newBlock.GetType()));

                        CGameInstance::Get().VoxelProcessPlayerBlockSet(worldBX, worldBY, worldBZ, newBlock);

                        if (hotbarItemInfo->iCnt <= 1)
                        {
                            hotbarItemInfo = std::nullopt;
                        }
                        else
                        {
                            hotbarItemInfo->iCnt -= 1;
                        }
                    }
                        break;
                    }
                }
            }
        }
        
    }
}

CUIController* CPlayerEntity::GetUIController() const
{
    return CGameInstance::Get().GetGameObjectByHandleT<CUIController>(m_hUIController);
}

void CPlayerEntity::ProcessUI(float fTimeDelta)
{
    if (m_bKeyDownE)
    {
        _bool bInvenRender = GetUIController()->Getinventory()->GetRender();
        _bool bCraftingTable = GetUIController()->GetCraftingTable()->GetRender();
        _bool bFurnaceRender = GetUIController()->GetBlastFurnace()->GetRender();
        _bool bChestRender = GetUIController()->GetChest()->GetRender();
        _bool bChest2Render = GetUIController()->GetChest2()->GetRender();

        if (!bFurnaceRender)
        {
            m_openFurnaceLocation = std::nullopt;
        }

        if (!bChestRender)
        {
            m_openChestLocation = std::nullopt;
        }

        if (bFurnaceRender || bCraftingTable || bChestRender || bChest2Render)
        {
            if (bInvenRender)
                GetUIController()->Getinventory()->SetRender(false);

            if(bFurnaceRender)
                GetUIController()->GetBlastFurnace()->SetRender(false);

            if (bCraftingTable)
                GetUIController()->GetCraftingTable()->SetRender(false);

            if(bChestRender)
                GetUIController()->GetChest()->SetRender(false);

            if (bChest2Render)
                GetUIController()->GetChest2()->SetRender(false);
        }
        else
        {
            if (bInvenRender)
                GetUIController()->Getinventory()->SetRender(false);
            else
                GetUIController()->Getinventory()->SetRender(true);
        }


        //if (GetUIController()->Getinventory()->GetRender())
        //{
        //    GetUIController()->Getinventory()->SetRender(false);
        //}
        //else
        //{
        //    GetUIController()->Getinventory()->SetRender(true);
        //}
       
        
    }

    ProcessUIHotbar(fTimeDelta);

    ProcessUIInventory(fTimeDelta);
    ProcessUIInventoryItemOnCursor(fTimeDelta);
    ProcessUIInventoryCrafting(fTimeDelta);

    ProcessUICraftingTable(fTimeDelta);
    ProcessUICraftingItemOnCursor(fTimeDelta);
    ProcessUICraftingTableCrafting(fTimeDelta);

    ProcessUIFurnace(fTimeDelta);
    ProcessUIFurnaceOnCursor(fTimeDelta);

    ProcessUIChest(fTimeDelta);
    ProcessUIChestOnCursor(fTimeDelta);

    ProcessUIStatus(fTimeDelta);


}

void CPlayerEntity::ProcessUIHotbar(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();
    if (!pUIController) return;

    if (m_iMouseMoveZ > 0)
    {
        auto idx = pUIController->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
        pUIController->GetHotBar()->GetHotBarSelect()->SetSelectIdx(++idx);
    }
    else if (m_iMouseMoveZ < 0)
    {
        auto idx = pUIController->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
        pUIController->GetHotBar()->GetHotBarSelect()->SetSelectIdx(--idx);
    }

    pUIController->GetHotBar()->SetHotbarItemData(m_ItemArrHotbar.data(), 9);
}

void CPlayerEntity::ProcessUIInventory(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();
   
    if (pUIController->Getinventory()->GetRender())
    {
        AddRenderPassPlayerInvenUIPass();

        pUIController->Getinventory()->SetInventoryHotbarItemData(m_ItemArrHotbar.data(), 9);
        pUIController->Getinventory()->SetInventoryItemData(m_ItemArrInventory.data(), 9 * 3);
        pUIController->Getinventory()->SetInventoryArmorItemData(m_ItemArrArmor.data(), 4);
        pUIController->Getinventory()->SetInventoryShieldItemData(&m_ItemShiled, 1);
        pUIController->Getinventory()->SetInventoryCraftingItemData(m_ItemArrInvenCrafting.data(), 5);
    }
}

void CPlayerEntity::ProcessUIInventoryCrafting(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pInven = GetUIController()->Getinventory();
    
    // 0   CRAFT_LT,
    // 1   CRAFT_RT,
    // 2   CRAFT_LB,
    // 3   CRAFT_RB,
    // 4   CRAFT_RESULT,

    ;
    std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
    craftingInfo.resize(4);

    for(int i = 0; i < 4; ++i)
        craftingInfo[i].first = m_ItemArrInvenCrafting[i];

    auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 2, 2);

    std::optional<CItemObject::SRecipe> optMatchRecipe{};

    for (const auto& recipe : CItemObject::s_vecRecipies)
    {
        auto recipeTrimmedGrid = CItemObject::GetTrimmedGrid(recipe.pattern, 3, 3);
        if (CItemObject::MatchTrimmedGrid(craftingTrimmedGrid, recipeTrimmedGrid))
        {
            optMatchRecipe = recipe;
            break;
        }
    }

    if (optMatchRecipe)
    {
        m_ItemArrInvenCrafting[4] = optMatchRecipe->result;
        m_ItemInvenCraftingOriginRecipe = optMatchRecipe;
    }
    else
    {
        m_ItemArrInvenCrafting[4] = std::nullopt;
        m_ItemInvenCraftingOriginRecipe = std::nullopt;
    }
    
}

void CPlayerEntity::ProcessUICraftingTable(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();

    if (pUIController->GetCraftingTable()->GetRender())
    {
        pUIController->GetCraftingTable()->SetInventoryHotbarItemData(m_ItemArrHotbar.data(), 9);
        pUIController->GetCraftingTable()->SetInventoryItemData(m_ItemArrInventory.data(), 9 * 3);
        pUIController->GetCraftingTable()->SetCraftingTableCraftingItemData(m_ItemArrCraftingTableCrafting.data(), 10);
    }
}

void CPlayerEntity::ProcessUICraftingTableCrafting(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pCraftingTable = GetUIController()->GetCraftingTable();

    // 0   CRAFT_LT,
    // 1   CRAFT_RT,
    // 2   CRAFT_LB,
    // 3   CRAFT_RB,
    // 4   CRAFT_RESULT,

    ;
    std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
    craftingInfo.resize(9);

    for (int i = 0; i < 9; ++i)
        craftingInfo[i].first = m_ItemArrCraftingTableCrafting[i];

    auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 3, 3);

    std::optional<CItemObject::SRecipe> optMatchRecipe{};

    for (const auto& recipe : CItemObject::s_vecRecipies)
    {
        auto recipeTrimmedGrid = CItemObject::GetTrimmedGrid(recipe.pattern, 3, 3);
        if (CItemObject::MatchTrimmedGrid(craftingTrimmedGrid, recipeTrimmedGrid))
        {
            optMatchRecipe = recipe;
            break;
        }
    }

    if (optMatchRecipe.has_value())
    {
        m_ItemArrCraftingTableCrafting[9] = optMatchRecipe->result;
        m_ItemCraftingTableCraftingOriginRecipe = optMatchRecipe;
    }
    else
    {
        m_ItemArrCraftingTableCrafting[9] = std::nullopt;
        m_ItemCraftingTableCraftingOriginRecipe = std::nullopt;
    }
}

void CPlayerEntity::ProcessUIFurnace(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();

    if (pUIController->GetBlastFurnace()->GetRender())
    {
        pUIController->GetBlastFurnace()->SetInventoryHotbarItemData(m_ItemArrHotbar.data(), 9);
        pUIController->GetBlastFurnace()->SetInventoryItemData(m_ItemArrInventory.data(), 9 * 3);
        if (m_openFurnaceLocation)
        {
           
            auto pFurnaceStorage = CGameInstance::Get().GetWorldFurnaceStorage();
            auto pStorage = pFurnaceStorage->GetStorage(m_openFurnaceLocation.value());
            pUIController->GetBlastFurnace()->SetIngredientItemData(&pStorage->ingredient, 1);
            pUIController->GetBlastFurnace()->SetFuelItemData(&pStorage->fuel, 1);
            pUIController->GetBlastFurnace()->SetResultItemData(&pStorage->result, 1);
            pUIController->GetBlastFurnace()->SetProcess(pStorage->progress);
            pUIController->GetBlastFurnace()->SetFuelProgress(pStorage->fuelProgress);
        }
    }
}

void CPlayerEntity::ProcessUIChest(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();

    if (pUIController->GetChest()->GetRender())
    {
        pUIController->GetChest()->SetInventoryHotbarItemData(m_ItemArrHotbar.data(), 9);
        pUIController->GetChest()->SetInventoryItemData(m_ItemArrInventory.data(), 9 * 3);
        if (m_openChestLocation)
        {
            auto pFurnaceStorage = CGameInstance::Get().GetWorldChestStorage();
            auto pStorage = pFurnaceStorage->GetStorage(m_openChestLocation.value());
            pUIController->GetChest()->SetChestInventoryItemData(&pStorage->items[0], 9 * 3);
        }
    }
}

void CPlayerEntity::ProcessThrowItem(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pUIController = GetUIController();
    if (m_bKeyDownQ)
    {
        auto selectIdx = pUIController->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
        if (auto& hotbarItemInfo = m_ItemArrHotbar[selectIdx])
        {
            if (hotbarItemInfo->block)
            {
                CItemObject::ItemInfo newInfo{ hotbarItemInfo.value()};
                newInfo.iCnt = 1;

                auto cnt = hotbarItemInfo->iCnt;
                if (cnt == 1)
                {
                    hotbarItemInfo = std::nullopt;
                }
                else
                {
                    hotbarItemInfo->iCnt -= 1;
                }

                {
                    auto vStart = GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 0.5f;
                    auto vDir = XMVector3Normalize(vStart - GetTransform().GetState(STATE::POSITION)) * 3.f;

                    _float3 startPos{};
                    XMStoreFloat3(&startPos, vStart);
                    startPos.y += 1.5f;
                    _float3 startDir{};
                    XMStoreFloat3(&startDir, vDir);
                    
                    CItemObject::SpawnDropItemObject(newInfo, startPos, startDir);
                }
            }
            else // not block
            {
                CItemObject::ItemInfo newInfo{ hotbarItemInfo.value()};
               

                if (CItemObject::IsCountableItem(hotbarItemInfo->eItemType))
                {
                    auto cnt = hotbarItemInfo->iCnt;
                    if (cnt == 1)
                    {
                        hotbarItemInfo = std::nullopt;
                    }
                    else
                    {
                        hotbarItemInfo->iCnt -= 1;
                    }

                    newInfo.iCnt = 1;
                }
                else
                {
                    newInfo.iCnt = 0;
                    hotbarItemInfo = std::nullopt;
                }


                {

                    auto vStart = GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 0.5f;
                    auto vDir = XMVector3Normalize(vStart - GetTransform().GetState(STATE::POSITION)) * 3.f;

                    _float3 startPos{};
                    XMStoreFloat3(&startPos, vStart);
                    startPos.y += 1.5f;
                    _float3 startDir{};
                    XMStoreFloat3(&startDir, vDir);

                    CItemObject::SpawnDropItemObject(newInfo, startPos, startDir);
                }
            }
           
            
        }
    }
}

void CPlayerEntity::ProcessHandHeldItem(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto selectIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
    CHandHeldItem* pHandHeldObj{};
    {
        E::CHandHeldItemObject::DESC Desc{};
        Desc.sObjectTag = "CHandHeldItem";

        if (auto pLayer = CGameInstance::Get().GetGameObjectLayer("24_HandHeldItem", "ITEM", "Prototype_GameObject_HandHeldItem", &Desc))
        {
            if (!pLayer->empty())
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CHandHeldItem>(pLayer->front()))
                {
                    pHandHeldObj = pObj;
                    //pObj->AddDropItemObject(info, pos, vel, { CItemObject::GetPackedTexIdByType(info.eItemType) });
                }
            }
        }
    }
    CPlayerFPSArm* pPlayerArm{};
    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT< CPlayerFPSArm>(m_hPlayerFPSArm))
    {
        pPlayerArm = pObj;
    }
    if (m_eCameraType == CAMERA_TYPE::FPS)
    {
       

        if (pHandHeldObj && pPlayerArm)
        {
            uint8_t iLight{ 0xFF };
            auto pos = GetTransform().GetPosition();
            int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
            int32_t blockY = static_cast<int32_t>(std::floor(pos.y + 1));
            int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
            if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
            {
                iLight = optCurrBlock->GetLight();
            }
            if (auto& hotbarItemInfo = m_ItemArrHotbar[selectIdx])
            {
                m_hRightItem = pHandHeldObj->GetHandle();
                //pHandHeldObj->GetTransform().SetParentWorldMatrix(*GetTransform().GetWorldMatrix());
                pHandHeldObj->SetRender(true);
                pPlayerArm->SetRender(false);

                pHandHeldObj->SetItemInfo(hotbarItemInfo.value());
                pHandHeldObj->SetVIBufferID({ "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(hotbarItemInfo.value()) });

                pHandHeldObj->SetLight(iLight);
                pHandHeldObj->SetPlayerHandle(GetHandle());

            }
            else
            {
                auto pos = GetTransform().GetPosition();
                int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
                int32_t blockY = static_cast<int32_t>(std::floor(pos.y + 1));
                int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
                if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
                {
                    pPlayerArm->SetLight(optCurrBlock->GetLight());
                }
                pPlayerArm->SetRender(true);
                pPlayerArm->SetLight(iLight);
                pHandHeldObj->SetRender(false);
                m_hRightItem = m_hPlayerFPSArm;
            }
        }
    }
    else if (m_eCameraType == CAMERA_TYPE::TPS || m_eCameraType == CAMERA_TYPE::TPS_BACK)
    {
        if (pPlayerArm)
        {
            pPlayerArm->SetRender(false);
        }
        
        if (pHandHeldObj)
        {
            uint8_t iLight{ 0xFF };
            auto pos = GetTransform().GetPosition();
            int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
            int32_t blockY = static_cast<int32_t>(std::floor(pos.y + 1));
            int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));
            if (auto optCurrBlock = E::CGameInstance::Get().GetVoxelBlock(blockX, blockY, blockZ))
            {
                iLight = optCurrBlock->GetLight();
            }
            if (auto& hotbarItemInfo = m_ItemArrHotbar[selectIdx])
            {
                m_hRightItem = pHandHeldObj->GetHandle();
                //pHandHeldObj->GetTransform().SetParentWorldMatrix(*GetTransform().GetWorldMatrix());
                pHandHeldObj->SetRender(true);

                pHandHeldObj->SetItemInfo(hotbarItemInfo.value());
                pHandHeldObj->SetVIBufferID({ "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(hotbarItemInfo.value()) });

                pHandHeldObj->SetLight(iLight);
                pHandHeldObj->SetPlayerHandle(GetHandle());

            }
            else
            {
                auto pos = GetTransform().GetPosition();
                int32_t blockX = static_cast<int32_t>(std::floor(pos.x));
                int32_t blockY = static_cast<int32_t>(std::floor(pos.y + 1));
                int32_t blockZ = static_cast<int32_t>(std::floor(pos.z));

                pHandHeldObj->SetRender(false);
                m_hRightItem = m_hPlayerFPSArm;
            }
        }
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
void CPlayerEntity::ProcessActionUpdate(_float fTimeDelta)
{

    //if (CGameInstance::Get().MouseUp(MOUSEKEYSTATE::RB))
    //{
    //    int x = 0;
    //}
    
    ProcessPlayerOpenInvenAction(fTimeDelta);
    ProcessPlayerCameraAction(fTimeDelta);
}

void CPlayerEntity::ProcessItemColliding(_float fTimeDelta)
{
    //Coll_DropBlockCenter

    if (auto pCollGroup = CGameInstance::Get().GetColliderGroup("Coll_DropItemObject"))
    {
        for (auto& pColl : *pCollGroup)
        {
            if (CGameInstance::Get().IntersectColl(pColl, m_pCenterCollider.get()))
            {
                if (auto pObj = Cast<CDropItemObject>(pColl->GetInnerPointer()))
                {
                    if (pColl->GetInnerHint2())
                    {
                        auto pHint = static_cast<CDropBlock::CollHint*>(pColl->GetInnerHint2().get());
                        if (pHint)
                        {
                            const auto& itemInfo = pHint->iter->itemInfo;
                            auto* pUIController = GetUIController();

                            //if (SUCCEEDED(pUIController->Getinventory()->AddItemToInventory(itemInfo)))
                            //{
                            //    pObj->GetDropItemObjects().erase(pHint->iter);
                            //}
                            if (pHint->iter->fElapsedTime > 0.3f)
                            {
                                if (SUCCEEDED(ProcessItemGain(itemInfo)))
                                {
                                    pObj->GetDropItemObjects().erase(pHint->iter);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void CPlayerEntity::ProcessExpOrbColliding(_float fTimeDelta)
{
    if (auto pCollGroup = CGameInstance::Get().GetColliderGroup("Coll_ExpOrb"))
    {
        for (auto& pColl : *pCollGroup)
        {
            if (CGameInstance::Get().IntersectColl(pColl, m_pCenterCollider.get()))
            {
                if (auto pObj = Cast<CExperienceOrb>(pColl->GetInnerPointer()))
                {
                    auto pHint = static_cast<CExperienceOrb::CollHint*>(pColl->GetInnerHint2().get());
                    //const auto& itemInfo = pHint->iter->itemInfo;
                    //auto* pUIController = GetUIController();
                   // auto a = *pHint->iter;
                    if (pHint)
                    {
                        if (SUCCEEDED(ProcessExpOrbGain(pHint->iter->fExp)))
                        {
                            pObj->GetExpOrbObjects().erase(pHint->iter);
                        }
                    }
                    
                }
            }
        }
    }
}

//void CPlayerEntity::PlayerCameraTrace(_float fTimeDelta)
//{
//    if (m_pActivePlayerCamera)
//    {
//        //?
//        const auto& [rayOrigin, rayDir] = m_pActivePlayerCamera->GetRay();
//        CVoxelManager3::BLOCK_RAY_RESULT result;
//        if (auto rayResult = CGameInstance::Get().VoxelBlockRaycast(rayOrigin, rayDir, 5.f, result))
//        {
//
//        }
//
//        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
//        m_pActivePlayerCamera->GetTransform().AddRotation(vUp, fTimeDelta * 10.f * m_iMouseMoveX);
//
//        _float3 euler = m_pActivePlayerCamera->GetTransform().GetRotationEuler();
//
//        float delta = fTimeDelta * 10.f * m_iMouseMoveY;
//        if (m_eCameraType == CAMERA_TYPE::TPS_BACK)
//        {
//            delta *= -1.f;
//        }
//        float next = euler.x + delta;
//
//        if (next <= 89.f && next >= -89.f)
//        {
//            _vector vRight = m_pActivePlayerCamera->GetTransform().GetState(STATE::RIGHT);
//            m_pActivePlayerCamera->GetTransform().AddRotation(vRight, delta);
//        }
//
//        if (m_eCameraType == CAMERA_TYPE::FPS)
//        {
//            if (m_bPlayerCameraLookBack)
//            {
//                m_bPlayerCameraLookBack = false;
//                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
//            }
//
//            auto playerPos = GetTransform().GetPosition();
//            m_pActivePlayerCamera->GetTransform().SetPosition(XMVectorSet(playerPos.x, playerPos.y + 1.62f, playerPos.z + 0.f, 1.f));
//        }
//        else if (m_eCameraType == CAMERA_TYPE::TPS)
//        {
//            if (m_bPlayerCameraLookBack)
//            {
//                m_bPlayerCameraLookBack = false;
//                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
//            }
//
//            auto playerLook = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
//            auto playerPos = GetTransform().GetLoadedPostion();
//
//            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);
//
//            m_pActivePlayerCamera->GetTransform().SetPosition(tmp);
//        }
//        else
//        {
//            if (!m_bPlayerCameraLookBack)
//            {
//                m_bPlayerCameraLookBack = true;
//                m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
//            }
//
//            auto playerLook = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
//            auto playerPos = GetTransform().GetLoadedPostion();
//
//            auto tmp = (playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f)) + (playerLook * -5.f);
//
//            m_pActivePlayerCamera->GetTransform().SetPosition(tmp);
//        }
//    }
//}

void CPlayerEntity::PlayerCameraTrace(_float fTimeDelta)
{
    if (m_pActivePlayerCamera)
    {
        // --- [카메라 회전 처리 (기존 유지)] ---
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

        // --- [카메라 위치 및 충돌 처리] ---
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
        else // TPS 및 TPS_BACK 공통 처리
        {
            if (m_eCameraType == CAMERA_TYPE::TPS)
            {
                if (m_bPlayerCameraLookBack)
                {
                    m_bPlayerCameraLookBack = false;
                    m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
                }
            }
            else // TPS_BACK
            {
                if (!m_bPlayerCameraLookBack)
                {
                    m_bPlayerCameraLookBack = true;
                    m_pActivePlayerCamera->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 180.f);
                }
            }

            // 1. 레이캐스트 시작점 (플레이어 머리 위치)
            auto playerPos = GetTransform().GetLoadedPostion();
            auto vTargetPivot = playerPos + XMVectorSet(0.f, 1.8f, 0.f, 0.f);

            // 2. 카메라가 향할 방향 및 최대 거리
            auto playerLook = m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK);
            _float fMaxDistance = 5.f;
            auto vRayDir = playerLook * -1.f; // 카메라가 바라보는 방향의 반대 (뒤통수 방향)

            // 이상적인 카메라 위치 (충돌이 없을 경우)
            auto vIdealPos = vTargetPivot + (vRayDir * fMaxDistance);
            auto vFinalPos = vIdealPos;

            // [수정된 부분] vIdealPos가 아니라 레이캐스트 시작점인 vTargetPivot을 _float3로 변환해야 합니다.
            _float3 v3TargetPivot;
            _float3 v3RayDir;
            XMStoreFloat3(&v3TargetPivot, vTargetPivot);
            XMStoreFloat3(&v3RayDir, vRayDir);

            // 3. 지형 충돌 레이캐스트 (머리에서 카메라 쪽으로 쏜다)
            CVoxelManager3::BLOCK_RAY_RESULT result{};
            // 시작점을 v3TargetPivot으로 변경!
            if (CGameInstance::Get().VoxelBlockRaycast(v3TargetPivot, v3RayDir, fMaxDistance, result))
            {
                // 부딪혔다면: 충돌 지점(result.fDist)에서 Near Plane 파먹기 방지용 여백(0.2f)을 빼줍니다.
                _float fSafeDistance = std::max(0.f, result.fDist - 0.35f);
                vFinalPos = vTargetPivot + (vRayDir * fSafeDistance);
            }

            // 4. 최종 위치 적용
            m_pActivePlayerCamera->GetTransform().SetPosition(vFinalPos);
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
                c.y = floorf(c.y - halfExtents.y) + 1.f + halfExtents.y + 0.001f;
                m_bOnGround = true;
            }
            else  // 올라가다 천장
            {
                c.y = prevY;
            }
            vVel = XMVectorSetY(vVel, 0.f);
        }
        else
        {
            // [수정] 단순히 겹치지 않는다고 false로 만들면 0.001f 오차 때문에 진동이 발생함.
            // 바닥 쪽으로 살짝(-0.05f) 내린 가상 박스로 진짜 바닥이 없는지 확실히 검사!
            XMFLOAT3 checkGround = { c.x, c.y - 0.05f, c.z };
            if (!CGameInstance::Get().VoxelAABBOverlap(checkGround, halfExtents))
            {
                m_bOnGround = false;
            }
        }

        // X
        float px = c.x;
        c.x += XMVectorGetX(vVel) * fTimeDelta;

        bool bHitXWall = CGameInstance::Get().VoxelAABBOverlap(c, halfExtents);
        bool bFallX = false;

        if (!bHitXWall && m_bKeyPressingShift && m_bOnGround)
        {
            // [수정] 플레이어 몸통 중심(Core)이 블록 위에 있도록 검사 박스 크기를 대폭 줄임
            XMFLOAT3 floorExtents = { halfExtents.x , halfExtents.y, halfExtents.z };
            XMFLOAT3 checkFloor = { c.x, c.y - 0.05f, c.z };
            if (!CGameInstance::Get().VoxelAABBOverlap(checkFloor, floorExtents))
            {
                bFallX = true;
            }
        }

        if (bHitXWall || bFallX)
        {
            c.x = px;
            vVel = XMVectorSetX(vVel, 0.f);
        }

        // Z (X와 동일한 원리 적용)
        float pz = c.z;
        c.z += XMVectorGetZ(vVel) * fTimeDelta;

        bool bHitZWall = CGameInstance::Get().VoxelAABBOverlap(c, halfExtents);
        bool bFallZ = false;

        if (!bHitZWall && m_bKeyPressingShift && m_bOnGround)
        {
            XMFLOAT3 floorExtents = { halfExtents.x , halfExtents.y, halfExtents.z  };
            XMFLOAT3 checkFloor = { c.x, c.y - 0.05f, c.z };
            if (!CGameInstance::Get().VoxelAABBOverlap(checkFloor, floorExtents))
            {
                bFallZ = true;
            }
        }

        if (bHitZWall || bFallZ)
        {
            c.z = pz;
            vVel = XMVectorSetZ(vVel, 0.f);
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


void CPlayerEntity::ReadyPlayerItem()
{
    m_ItemArrHotbar[0] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_WoodPickaxe };
    //m_ItemArrHotbar[0]->fDurability = 0.95f;
    m_ItemArrHotbar[1] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_DiamondHelmet };
    m_ItemArrHotbar[2] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_IronChestplate };
    m_ItemArrHotbar[3] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_GoldLeggings };
    m_ItemArrHotbar[4] = CItemObject::ItemInfo{ CBlock3(CBlock3::TYPE::TNT), 64};
    m_ItemArrHotbar[5] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_FlintAndSteel };
    m_ItemArrHotbar[6] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Bread, 64 };
    m_ItemArrHotbar[7] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Raw_Mutton, 64 };
    m_ItemArrHotbar[8] = CItemObject::ItemInfo{ CItemObject::ITEM_TYPE::ITEM_Bow_Standby };
}

HRESULT CPlayerEntity::ProcessItemGain(const CItemObject::ItemInfo& newItemInfo)
{
    auto funcCheckRoutine = [&](std::optional<CItemObject::ItemInfo>* pArr, size_t size)->HRESULT
        {
            // hotbar check
            for (uint32_t i = 0; i < size; ++i)
            {
                auto& currHotbar = pArr[i];
                if (!currHotbar)
                {
                    currHotbar = newItemInfo;
                    return S_OK;
                }
                else
                {
                    if (newItemInfo.block && currHotbar->block)
                    {
                        if (newItemInfo.block->GetType() == currHotbar->block->GetType())
                        {
                            auto currCnt = currHotbar->iCnt;
                            auto newCnt = newItemInfo.iCnt;

                            if (currCnt + newCnt <= 64)
                            {
                                currHotbar->iCnt += newCnt;
                                return S_OK;
                            }
                        }
                    }
                    else if (newItemInfo.block && !currHotbar->block || !newItemInfo.block && currHotbar->block)
                    {

                    }
                    else
                    {
                        if (newItemInfo.eItemType == currHotbar->eItemType)
                        {
                            if (CItemObject::IsCountableItem(newItemInfo.eItemType))
                            {
                                auto currCnt = currHotbar->iCnt;
                                auto newCnt = newItemInfo.iCnt;

                                if (currCnt + newCnt <= 64)
                                {
                                    currHotbar->iCnt += newCnt;
                                    return S_OK;
                                }
                            }
                        }
                    }
                }
            }

            return E_FAIL;
        };


    if (SUCCEEDED(funcCheckRoutine(m_ItemArrHotbar.data(), 9)))
    {
        return S_OK;
    }

    if (SUCCEEDED(funcCheckRoutine(m_ItemArrInventory.data(), 9 * 3)))
    {
        return S_OK;
    }


    //// hotbar check
    //for (uint32_t i = 0; i < 9; ++i)
    //{
    //    auto& currHotbar = m_ItemArrHotbar[i];
    //    if (!currHotbar)
    //    {
    //        currHotbar = newItemInfo;
    //        return S_OK;
    //    }
    //    else
    //    {
    //        if (newItemInfo.block && currHotbar->block)
    //        {
    //            if (newItemInfo.block->GetType() == currHotbar->block->GetType())
    //            {
    //                auto currCnt = currHotbar->iCnt;
    //                auto newCnt = newItemInfo.iCnt;

    //                if (currCnt + newCnt < 64)
    //                {
    //                    currHotbar->iCnt += newCnt;
    //                    return S_OK;
    //                }
    //            }
    //        }
    //        else if (newItemInfo.block && !currHotbar->block || !newItemInfo.block && currHotbar->block)
    //        {

    //        }
    //        else
    //        {
    //            if (newItemInfo.eItemType == currHotbar->eItemType)
    //            {
    //                if (CItemObject::IsCountableItem(newItemInfo.eItemType) )
    //                {
    //                    auto currCnt = currHotbar->iCnt;
    //                    auto newCnt = newItemInfo.iCnt;

    //                    if (currCnt + newCnt < 64)
    //                    {
    //                        currHotbar->iCnt += newCnt;
    //                        return S_OK;
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}




    return E_FAIL;
}

HRESULT CPlayerEntity::ProcessExpOrbGain(_float fGage)
{
    m_fExperienceGage += fGage;
    if (m_fExperienceGage > 1.f)
    {
        m_fExperienceGage -= 1.f;
        m_iLevel += 1;
    }


    return S_OK;
}

void CPlayerEntity::ProcessUIInventoryItemOnCursor(_float fTimeDelta)
{
    auto* pUIController = GetUIController();
    if (!pUIController) return;
    if (!pUIController->Getinventory()->GetRender())
    {
        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hInventoryUIItemOnCursor))
        {
            // todo: countable item 하나씩 뿌리기
            if (auto pInfo = pObj->GetItemInfoPtr())
            {
                _float3 startPos{};
                XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                startPos.y += 1.f;
                CItemObject::SpawnDropItemObject(*pInfo, startPos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
            }
            pObj->SetPendingDestroyCascade();
        }
        return;
    }

    auto invenSlotToMemberItem = [&](CUIInventory::InventorySlot slot)->std::optional<CItemObject::ItemInfo>*
        {
            std::optional<CItemObject::ItemInfo>* pTargetInfo{};

            //INVENTORY_HOTBAR,
            //    INVENTORY,
            //    ARMOR_HELMET,
            //    ARMOR_CHESTPLATE,
            //    ARMOR_LEGGINGS,
            //    ARMOR_BOOTS,
            //    SHIELD,
            //    CRAFT_LT,
            //    CRAFT_RT,
            //    CRAFT_LB,
            //    CRAFT_RB,
            //    CRAFT_RESULT,
            switch (slot.eType)
            {
            case CUIInventory::SlotType::INVENTORY_HOTBAR:
                return  &m_ItemArrHotbar[slot.typeIdx];
            case CUIInventory::SlotType::INVENTORY:
                return  &m_ItemArrInventory[slot.typeIdx];
            case CUIInventory::SlotType::ARMOR_HELMET:
                return &m_ItemArrArmor[0];
            case CUIInventory::SlotType::ARMOR_CHESTPLATE:
                return &m_ItemArrArmor[1];
            case CUIInventory::SlotType::ARMOR_LEGGINGS:
                return &m_ItemArrArmor[2];
            case CUIInventory::SlotType::ARMOR_BOOTS:
                return &m_ItemArrArmor[3];
            case CUIInventory::SlotType::SHIELD:
                return &m_ItemShiled;
            case CUIInventory::SlotType::CRAFT_LT:
                return &m_ItemArrInvenCrafting[0];
            case CUIInventory::SlotType::CRAFT_RT:
                return &m_ItemArrInvenCrafting[1];
            case CUIInventory::SlotType::CRAFT_LB:
                return &m_ItemArrInvenCrafting[2];
            case CUIInventory::SlotType::CRAFT_RB:
                return &m_ItemArrInvenCrafting[3];
            case CUIInventory::SlotType::CRAFT_RESULT:
                return &m_ItemArrInvenCrafting[4];
            }

            return nullptr;
        };


    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hInventoryUIItemOnCursor))
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

        pObj->SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });


        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
            || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            for (const auto& slot : GetUIController()->Getinventory()->GetInvenSlots())
            {
                auto itemOrigin = pObj->GetOrigin();
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {
                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {

                            // 크래프팅결과
                            if (slot.eType == CUIInventory::SlotType::CRAFT_RESULT)
                            {
                                auto funcGetCraftRes = [&]()
                                    {
                                        if (m_ItemInvenCraftingOriginRecipe)
                                        {
                                            if (pItemInfo->iCnt + m_ItemInvenCraftingOriginRecipe->result.iCnt <= 64)
                                            {
                                                pItemInfo->iCnt += m_ItemInvenCraftingOriginRecipe->result.iCnt;

                                                std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
                                                craftingInfo.resize(4);
                                                for (int i = 0; i < 4; ++i)
                                                {
                                                    craftingInfo[i].first = m_ItemArrInvenCrafting[i];
                                                    if (craftingInfo[i].first)
                                                    {
                                                        craftingInfo[i].second = &m_ItemArrInvenCrafting[i];
                                                    }
                                                }

                                                auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 2, 2);
                                                auto trimmedGrid = CItemObject::GetTrimmedGrid(m_ItemInvenCraftingOriginRecipe->pattern, 3, 3);
                                                for (uint32_t i = 0; i < trimmedGrid.grid.size(); ++i)
                                                {
                                                    auto pCraft = static_cast<std::optional<CItemObject::ItemInfo>*>(craftingTrimmedGrid.grid[i].second);

                                                    if (pCraft)
                                                    {
                                                        if (pCraft->value().iCnt >= trimmedGrid.grid[i].first->iCnt)
                                                        {
                                                            pCraft->value().iCnt -= trimmedGrid.grid[i].first->iCnt;
                                                            if (pCraft->value().iCnt <= 0)
                                                            {
                                                                *pCraft = std::nullopt;
                                                            }
                                                        }
                                                    }
                                                    
                                                }
                                            }
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcGetCraftRes();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcGetCraftRes();
                                        }
                                    }
                                }

                                continue;
                            }
                            
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                auto funcIfMaxSwapElseCntUp = [&]()
                                    {
                                        auto isMaxOver = pItemInfo->iCnt + pTargetInfo->value().iCnt >= 64;
                                        if (isMaxOver)
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        else
                                        {
                                            pTargetInfo->value().iCnt += pItemInfo->iCnt;
                                            pObj->SetPendingDestroy();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcIfMaxSwapElseCntUp();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                    
                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcIfMaxSwapElseCntUp();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                            else
                            {
                                auto funcItemCntAdd = [&]()
                                    {
                                        auto isCanAdd = pTargetInfo->value().iCnt < 64;
                                        if (isCanAdd)
                                        {
                                            pTargetInfo->value().iCnt += 1;

                                            if (pItemInfo->iCnt > 1)
                                            {
                                                pItemInfo->iCnt -= 1;
                                            }
                                            else
                                            {
                                                pObj->SetPendingDestroyCascade();
                                            }
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcItemCntAdd();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                    
                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcItemCntAdd();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }
                    // 타겟이 없는경우 온커서에 있는거 그대로 넘기기
                    else
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (slot.eType == CUIInventory::SlotType::CRAFT_RESULT)
                            {
                                continue;
                            }
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                *pTargetInfo = *pItemInfo;
                                pObj->SetPendingDestroyCascade();
                                
                            }
                            else
                            {
                                if (pItemInfo->block || CItemObject::IsCountableItem(pItemInfo->eItemType))
                                {
                                    if (pItemInfo->iCnt <= 1)
                                    {
                                        *pTargetInfo = *pItemInfo;
                                        pObj->SetPendingDestroyCascade();
                                    }
                                    else
                                    {
                                        CItemObject::ItemInfo copy{ *pItemInfo };
                                        copy.iCnt = 1;

                                        pItemInfo->iCnt -= 1;

                                        *pTargetInfo = copy;
                                    }
                                }
                                else
                                {
                                    *pTargetInfo = *pItemInfo;
                                    pObj->SetPendingDestroyCascade();
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }


                    pObj->SetOrigin(slotOrigin);
                    break;
                }
            }
        }
    }
    // onCursor에 아이템이 없는경우
    else
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB) || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

            for (const auto& slot : GetUIController()->Getinventory()->GetInvenSlots())
            {
                auto itemOrigin = _float2{ (float)mousePos.x, (float)mousePos.y };
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {

                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    // 해당 아이템 슬롯이 잇는경우에만
                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        auto funcSpawnOnCursorItem = [&](const CItemObject::ItemInfo& movedItemInfo)
                            {
                                {
                                    E::CUIItem::DESC Desc{};
                                    Desc.fX = itemOrigin.x;
                                    Desc.fY = itemOrigin.y;
                                    Desc.sObjectTag = "UIItem";
                                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
                                        "80_UI", &Desc))
                                    {
                                        m_hInventoryUIItemOnCursor = handle.value();
                                        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hInventoryUIItemOnCursor))
                                        {
                                            pObj->SetItemInfo(movedItemInfo);
                                        }
                                    }
                                }
                            };


                        auto funcItemDivideToOnCurosr = [&](std::optional<CItemObject::ItemInfo>& targetInfo)
                            {
                                auto currItemCnt = targetInfo.value().iCnt;
                                if (currItemCnt > 1)
                                {
                                    auto dvd2 = currItemCnt / 2;
                                    auto remain = currItemCnt - dvd2;

                                    targetInfo.value().iCnt = remain;

                                    {
                                        CItemObject::ItemInfo newInfo{ targetInfo.value() };
                                        newInfo.iCnt = dvd2;

                                        funcSpawnOnCursorItem(newInfo);
                                    }
                                }
                                else
                                {
                                    {
                                        funcSpawnOnCursorItem(targetInfo.value());
                                    }


                                    *pTargetInfo = std::nullopt;
                                }
                            };

                        if (slot.eType == CUIInventory::SlotType::CRAFT_RESULT)
                        {
                            {
                                if (m_ItemInvenCraftingOriginRecipe)
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());
                                    *pTargetInfo = std::nullopt;

                                    std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
                                    craftingInfo.resize(4);
                                    for (int i = 0; i < 4; ++i)
                                    {
                                        craftingInfo[i].first = m_ItemArrInvenCrafting[i];
                                        if (craftingInfo[i].first)
                                        {
                                            craftingInfo[i].second = &m_ItemArrInvenCrafting[i];
                                        }
                                    }

                                    auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 2, 2);
                                    auto trimmedGrid = CItemObject::GetTrimmedGrid(m_ItemInvenCraftingOriginRecipe->pattern, 3, 3);
                                    for (uint32_t i = 0; i < trimmedGrid.grid.size(); ++i)
                                    {
                                        auto pCraft = static_cast<std::optional<CItemObject::ItemInfo>*>(craftingTrimmedGrid.grid[i].second);
                                        
                                        if (pCraft)
                                        {
                                            if (pCraft->value().iCnt >= trimmedGrid.grid[i].first->iCnt)
                                            {
                                                pCraft->value().iCnt -= trimmedGrid.grid[i].first->iCnt;
                                                if (pCraft->value().iCnt <= 0)
                                                {
                                                    *pCraft = std::nullopt;
                                                }
                                            }
                                        }
                                        
                                    }
                                    
                                }
                            }

                            continue;
                        }


                        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                        {
                            funcSpawnOnCursorItem(pTargetInfo->value());

                            *pTargetInfo = std::nullopt;
                        }
                        else
                        {
                            auto currItemCnt = pTargetInfo->value().iCnt;
                            if (pTargetInfo->value().block)
                            {
                                funcItemDivideToOnCurosr(*pTargetInfo);
                            }
                            else
                            {
                                if (CItemObject::IsCountableItem(pTargetInfo->value().eItemType))
                                {
                                    funcItemDivideToOnCurosr(*pTargetInfo);
                                }
                                else
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());

                                    *pTargetInfo = std::nullopt;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void CPlayerEntity::ProcessUICraftingItemOnCursor(_float fTimeDelta)
{
    auto* pUIController = GetUIController();
    if (!pUIController) return;
    if (!pUIController->GetCraftingTable()->GetRender())
    {
        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hCraftingTableUIItemOnCursor))
        {
            // todo: countable item 하나씩 뿌리기
            if (auto pInfo = pObj->GetItemInfoPtr())
            {
                _float3 startPos{};
                XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                startPos.y += 1.f;
                CItemObject::SpawnDropItemObject(*pInfo, startPos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
            }
            pObj->SetPendingDestroyCascade();
        }
        return;
    }

    auto invenSlotToMemberItem = [&](CUICraftingTable::CraftingTableSlot slot)->std::optional<CItemObject::ItemInfo>*
        {
            std::optional<CItemObject::ItemInfo>* pTargetInfo{};

            //INVENTORY_HOTBAR,
            //    INVENTORY,

            //    CRAFT_LT,
            //    CRAFT_MT,
            //    CRAFT_RT,
            //    CRAFT_LM,
            //    CRAFT_MM,
            //    CRAFT_RM,
            //    CRAFT_LB,
            //    CRAFT_MB,
            //    CRAFT_RB,

            //    CRAFT_RESULT,
            switch (slot.eType)
            {
            case CUICraftingTable::SlotType::INVENTORY_HOTBAR:
                return  &m_ItemArrHotbar[slot.typeIdx];
            case CUICraftingTable::SlotType::INVENTORY:
                return  &m_ItemArrInventory[slot.typeIdx];
            case CUICraftingTable::SlotType::CRAFT_LT:
                return  &m_ItemArrCraftingTableCrafting[0];
            case CUICraftingTable::SlotType::CRAFT_MT:
                return  &m_ItemArrCraftingTableCrafting[1];
            case CUICraftingTable::SlotType::CRAFT_RT:
                return  &m_ItemArrCraftingTableCrafting[2];
            case CUICraftingTable::SlotType::CRAFT_LM:
                return  &m_ItemArrCraftingTableCrafting[3];
            case CUICraftingTable::SlotType::CRAFT_MM:
                return  &m_ItemArrCraftingTableCrafting[4];
            case CUICraftingTable::SlotType::CRAFT_RM:
                return  &m_ItemArrCraftingTableCrafting[5];
            case CUICraftingTable::SlotType::CRAFT_LB:
                return  &m_ItemArrCraftingTableCrafting[6];
            case CUICraftingTable::SlotType::CRAFT_MB:
                return  &m_ItemArrCraftingTableCrafting[7];
            case CUICraftingTable::SlotType::CRAFT_RB:
                return  &m_ItemArrCraftingTableCrafting[8];
            case CUICraftingTable::SlotType::CRAFT_RESULT:
                return  &m_ItemArrCraftingTableCrafting[9];
            }

            return nullptr;
        };


    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hCraftingTableUIItemOnCursor))
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

        pObj->SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });


        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
            || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            for (const auto& slot : GetUIController()->GetCraftingTable()->GetCraftingTableSlots())
            {
                auto itemOrigin = pObj->GetOrigin();
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {
                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {

                            // 크래프팅결과
                            if (slot.eType == CUICraftingTable::SlotType::CRAFT_RESULT)
                            {
                                auto funcGetCraftRes = [&]()
                                    {
                                        if (m_ItemCraftingTableCraftingOriginRecipe)
                                        {
                                            if (pItemInfo->iCnt + m_ItemCraftingTableCraftingOriginRecipe->result.iCnt <= 64)
                                            {
                                                pItemInfo->iCnt += m_ItemCraftingTableCraftingOriginRecipe->result.iCnt;

                                                std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
                                                craftingInfo.resize(9);
                                                for (int i = 0; i < 9; ++i)
                                                {
                                                    craftingInfo[i].first = m_ItemArrCraftingTableCrafting[i];
                                                    if (craftingInfo[i].first)
                                                    {
                                                        craftingInfo[i].second = &m_ItemArrCraftingTableCrafting[i];
                                                    }
                                                }

                                                auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 3, 3);
                                                auto trimmedGrid = CItemObject::GetTrimmedGrid(m_ItemCraftingTableCraftingOriginRecipe->pattern, 3, 3);
                                                for (uint32_t i = 0; i < trimmedGrid.grid.size(); ++i)
                                                {
                                                    auto pCraft = static_cast<std::optional<CItemObject::ItemInfo>*>(craftingTrimmedGrid.grid[i].second);

                                                    if (pCraft)
                                                    {
                                                        if (pCraft->value().iCnt >= trimmedGrid.grid[i].first->iCnt)
                                                        {
                                                            pCraft->value().iCnt -= trimmedGrid.grid[i].first->iCnt;
                                                            if (pCraft->value().iCnt <= 0)
                                                            {
                                                                *pCraft = std::nullopt;
                                                            }
                                                        }
                                                    }
                                                    
                                                }
                                            }
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcGetCraftRes();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcGetCraftRes();
                                        }
                                    }
                                }

                                continue;
                            }

                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                auto funcIfMaxSwapElseCntUp = [&]()
                                    {
                                        auto isMaxOver = pItemInfo->iCnt + pTargetInfo->value().iCnt >= 64;
                                        if (isMaxOver)
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        else
                                        {
                                            pTargetInfo->value().iCnt += pItemInfo->iCnt;
                                            pObj->SetPendingDestroy();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcIfMaxSwapElseCntUp();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcIfMaxSwapElseCntUp();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                            else
                            {
                                auto funcItemCntAdd = [&]()
                                    {
                                        auto isCanAdd = pTargetInfo->value().iCnt < 64;
                                        if (isCanAdd)
                                        {
                                            pTargetInfo->value().iCnt += 1;

                                            if (pItemInfo->iCnt > 1)
                                            {
                                                pItemInfo->iCnt -= 1;
                                            }
                                            else
                                            {
                                                pObj->SetPendingDestroyCascade();
                                            }
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcItemCntAdd();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcItemCntAdd();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }
                    // 타겟이 없는경우 온커서에 있는거 그대로 넘기기
                    else
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (slot.eType == CUICraftingTable::SlotType::CRAFT_RESULT)
                            {
                                continue;
                            }
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                *pTargetInfo = *pItemInfo;
                                pObj->SetPendingDestroyCascade();

                            }
                            else
                            {
                                if (pItemInfo->block || CItemObject::IsCountableItem(pItemInfo->eItemType))
                                {
                                    if (pItemInfo->iCnt <= 1)
                                    {
                                        *pTargetInfo = *pItemInfo;
                                        pObj->SetPendingDestroyCascade();
                                    }
                                    else
                                    {
                                        CItemObject::ItemInfo copy{ *pItemInfo };
                                        copy.iCnt = 1;

                                        pItemInfo->iCnt -= 1;

                                        *pTargetInfo = copy;
                                    }
                                }
                                else
                                {
                                    *pTargetInfo = *pItemInfo;
                                    pObj->SetPendingDestroyCascade();
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }


                    pObj->SetOrigin(slotOrigin);
                    break;
                }
            }
        }
    }
    // onCursor에 아이템이 없는경우
    else
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB) || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

            for (const auto& slot : GetUIController()->GetCraftingTable()->GetCraftingTableSlots())
            {
                auto itemOrigin = _float2{ (float)mousePos.x, (float)mousePos.y };
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {

                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    // 해당 아이템 슬롯이 잇는경우에만
                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        auto funcSpawnOnCursorItem = [&](const CItemObject::ItemInfo& movedItemInfo)
                            {
                                {
                                    E::CUIItem::DESC Desc{};
                                    Desc.fX = itemOrigin.x;
                                    Desc.fY = itemOrigin.y;
                                    Desc.sObjectTag = "UIItem";
                                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
                                        "80_UI", &Desc))
                                    {
                                        m_hCraftingTableUIItemOnCursor = handle.value();
                                        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hCraftingTableUIItemOnCursor))
                                        {
                                            pObj->SetItemInfo(movedItemInfo);
                                        }
                                    }
                                }
                            };


                        auto funcItemDivideToOnCurosr = [&](std::optional<CItemObject::ItemInfo>& targetInfo)
                            {
                                auto currItemCnt = targetInfo.value().iCnt;
                                if (currItemCnt > 1)
                                {
                                    auto dvd2 = currItemCnt / 2;
                                    auto remain = currItemCnt - dvd2;

                                    targetInfo.value().iCnt = remain;

                                    {
                                        CItemObject::ItemInfo newInfo{ targetInfo.value() };
                                        newInfo.iCnt = dvd2;

                                        funcSpawnOnCursorItem(newInfo);
                                    }
                                }
                                else
                                {
                                    {
                                        funcSpawnOnCursorItem(targetInfo.value());
                                    }


                                    *pTargetInfo = std::nullopt;
                                }
                            };

                        if (slot.eType == CUICraftingTable::SlotType::CRAFT_RESULT)
                        {
                            {
                                if (m_ItemCraftingTableCraftingOriginRecipe)
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());
                                    *pTargetInfo = std::nullopt;

                                    std::vector<std::pair<std::optional<CItemObject::ItemInfo>, void*>> craftingInfo{};
                                    craftingInfo.resize(9);
                                    for (int i = 0; i < 9; ++i)
                                    {
                                        craftingInfo[i].first = m_ItemArrCraftingTableCrafting[i];
                                        if (craftingInfo[i].first)
                                        {
                                            craftingInfo[i].second = &m_ItemArrCraftingTableCrafting[i];
                                        }
                                    }

                                    auto craftingTrimmedGrid = CItemObject::GetTrimmedGrid(craftingInfo, 3, 3);
                                    auto trimmedGrid = CItemObject::GetTrimmedGrid(m_ItemCraftingTableCraftingOriginRecipe->pattern, 3, 3);
                                    for (uint32_t i = 0; i < trimmedGrid.grid.size(); ++i)
                                    {
                                        auto pCraft = static_cast<std::optional<CItemObject::ItemInfo>*>(craftingTrimmedGrid.grid[i].second);
                                        if (pCraft)
                                        {
                                            if (pCraft->value().iCnt >= trimmedGrid.grid[i].first->iCnt)
                                            {
                                                pCraft->value().iCnt -= trimmedGrid.grid[i].first->iCnt;
                                                if (pCraft->value().iCnt <= 0)
                                                {
                                                    *pCraft = std::nullopt;
                                                }
                                            }
                                        }
                                        
                                    }

                                }
                            }

                            continue;
                        }


                        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                        {
                            funcSpawnOnCursorItem(pTargetInfo->value());

                            *pTargetInfo = std::nullopt;
                        }
                        else
                        {
                            auto currItemCnt = pTargetInfo->value().iCnt;
                            if (pTargetInfo->value().block)
                            {
                                funcItemDivideToOnCurosr(*pTargetInfo);
                            }
                            else
                            {
                                if (CItemObject::IsCountableItem(pTargetInfo->value().eItemType))
                                {
                                    funcItemDivideToOnCurosr(*pTargetInfo);
                                }
                                else
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());

                                    *pTargetInfo = std::nullopt;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void CPlayerEntity::ProcessUIFurnaceOnCursor(_float fTimeDelta)
{
    auto* pUIController = GetUIController();
    if (!pUIController) return;
    if (!pUIController->GetBlastFurnace()->GetRender())
    {
        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hFurnaceUIItemOnCursor))
        {
            // todo: countable item 하나씩 뿌리기
            if (auto pInfo = pObj->GetItemInfoPtr())
            {
                _float3 startPos{};
                XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                startPos.y += 1.f;
                CItemObject::SpawnDropItemObject(*pInfo, startPos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
            }
            pObj->SetPendingDestroyCascade();
        }
        return;
    }
    if (!m_openFurnaceLocation) return;

    auto invenSlotToMemberItem = [&](CUIBlastFurnace::FurnaceSlot slot)->std::optional<CItemObject::ItemInfo>*
        {

            auto pFurnaceStorage = CGameInstance::Get().GetWorldFurnaceStorage();
            auto pStorage = pFurnaceStorage->GetStorage(m_openFurnaceLocation.value());

            std::optional<CItemObject::ItemInfo>* pTargetInfo{};

            //    INVENTORY_HOTBAR,
            //    INVENTORY,

            //    INGREDIENT,
            //    FUEL,
            //    RESULT,

            //    CRAFT_RESULT,
            switch (slot.eType)
            {
            case CUIBlastFurnace::SlotType::INVENTORY_HOTBAR:
                return  &m_ItemArrHotbar[slot.typeIdx];
            case CUIBlastFurnace::SlotType::INVENTORY:
                return  &m_ItemArrInventory[slot.typeIdx];
            case CUIBlastFurnace::SlotType::INGREDIENT:
                return  &pStorage->ingredient;
            case CUIBlastFurnace::SlotType::FUEL:
                return  &pStorage->fuel;
            case CUIBlastFurnace::SlotType::RESULT:
                return  &pStorage->result;
            }

            return nullptr;
        };


    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hFurnaceUIItemOnCursor))
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

        pObj->SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });


        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
            || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            for (const auto& slot : GetUIController()->GetBlastFurnace()->GetFurnaceSlots())
            {
                auto itemOrigin = pObj->GetOrigin();
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {
                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                auto funcIfMaxSwapElseCntUp = [&]()
                                    {
                                        auto isMaxOver = pItemInfo->iCnt + pTargetInfo->value().iCnt >= 64;
                                        if (isMaxOver)
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        else
                                        {
                                            pTargetInfo->value().iCnt += pItemInfo->iCnt;
                                            pObj->SetPendingDestroy();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcIfMaxSwapElseCntUp();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcIfMaxSwapElseCntUp();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                            else
                            {
                                auto funcItemCntAdd = [&]()
                                    {
                                        auto isCanAdd = pTargetInfo->value().iCnt < 64;
                                        if (isCanAdd)
                                        {
                                            pTargetInfo->value().iCnt += 1;

                                            if (pItemInfo->iCnt > 1)
                                            {
                                                pItemInfo->iCnt -= 1;
                                            }
                                            else
                                            {
                                                pObj->SetPendingDestroyCascade();
                                            }
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcItemCntAdd();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcItemCntAdd();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }
                    // 타겟이 없는경우 온커서에 있는거 그대로 넘기기
                    else
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                *pTargetInfo = *pItemInfo;
                                pObj->SetPendingDestroyCascade();

                            }
                            else
                            {
                                if (pItemInfo->block || CItemObject::IsCountableItem(pItemInfo->eItemType))
                                {
                                    if (pItemInfo->iCnt <= 1)
                                    {
                                        *pTargetInfo = *pItemInfo;
                                        pObj->SetPendingDestroyCascade();
                                    }
                                    else
                                    {
                                        CItemObject::ItemInfo copy{ *pItemInfo };
                                        copy.iCnt = 1;

                                        pItemInfo->iCnt -= 1;

                                        *pTargetInfo = copy;
                                    }
                                }
                                else
                                {
                                    *pTargetInfo = *pItemInfo;
                                    pObj->SetPendingDestroyCascade();
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }


                    pObj->SetOrigin(slotOrigin);
                    break;
                }
            }
        }
    }
    // onCursor에 아이템이 없는경우
    else
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB) || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

            for (const auto& slot : GetUIController()->GetBlastFurnace()->GetFurnaceSlots())
            {
                auto itemOrigin = _float2{ (float)mousePos.x, (float)mousePos.y };
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {

                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    // 해당 아이템 슬롯이 잇는경우에만
                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        auto funcSpawnOnCursorItem = [&](const CItemObject::ItemInfo& movedItemInfo)
                            {
                                {
                                    E::CUIItem::DESC Desc{};
                                    Desc.fX = itemOrigin.x;
                                    Desc.fY = itemOrigin.y;
                                    Desc.sObjectTag = "UIItem";
                                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
                                        "80_UI", &Desc))
                                    {
                                        m_hFurnaceUIItemOnCursor = handle.value();
                                        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hFurnaceUIItemOnCursor))
                                        {
                                            pObj->SetItemInfo(movedItemInfo);
                                        }
                                    }
                                }
                            };


                        auto funcItemDivideToOnCurosr = [&](std::optional<CItemObject::ItemInfo>& targetInfo)
                            {
                                auto currItemCnt = targetInfo.value().iCnt;
                                if (currItemCnt > 1)
                                {
                                    auto dvd2 = currItemCnt / 2;
                                    auto remain = currItemCnt - dvd2;

                                    targetInfo.value().iCnt = remain;

                                    {
                                        CItemObject::ItemInfo newInfo{ targetInfo.value() };
                                        newInfo.iCnt = dvd2;

                                        funcSpawnOnCursorItem(newInfo);
                                    }
                                }
                                else
                                {
                                    {
                                        funcSpawnOnCursorItem(targetInfo.value());
                                    }


                                    *pTargetInfo = std::nullopt;
                                }
                            };



                        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                        {
                            funcSpawnOnCursorItem(pTargetInfo->value());

                            *pTargetInfo = std::nullopt;
                        }
                        else
                        {
                            auto currItemCnt = pTargetInfo->value().iCnt;
                            if (pTargetInfo->value().block)
                            {
                                funcItemDivideToOnCurosr(*pTargetInfo);
                            }
                            else
                            {
                                if (CItemObject::IsCountableItem(pTargetInfo->value().eItemType))
                                {
                                    funcItemDivideToOnCurosr(*pTargetInfo);
                                }
                                else
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());

                                    *pTargetInfo = std::nullopt;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}

void CPlayerEntity::ProcessUIChestOnCursor(_float fTimeDelta)
{
    auto* pUIController = GetUIController();
    if (!pUIController) return;
    if (!pUIController->GetChest()->GetRender())
    {
        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hChestUIItemOnCursor))
        {
            // todo: countable item 하나씩 뿌리기
            if (auto pInfo = pObj->GetItemInfoPtr())
            {
                _float3 startPos{};
                XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                startPos.y += 1.f;
                CItemObject::SpawnDropItemObject(*pInfo, startPos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
            }
            pObj->SetPendingDestroyCascade();
        }
        return;
    }
    if (!m_openChestLocation) return;

    auto invenSlotToMemberItem = [&](CUIChest::ChestSlot slot)->std::optional<CItemObject::ItemInfo>*
        {

            auto pChestStorage = CGameInstance::Get().GetWorldChestStorage();
            auto pStorage = pChestStorage->GetStorage(m_openChestLocation.value());

            std::optional<CItemObject::ItemInfo>* pTargetInfo{};

            switch (slot.eType)
            {
            case CUIChest::SlotType::INVENTORY_HOTBAR:
                return  &m_ItemArrHotbar[slot.typeIdx];
            case CUIChest::SlotType::INVENTORY:
                return  &m_ItemArrInventory[slot.typeIdx];
            case CUIChest::SlotType::CHEST_INVENTORY:
                return &pStorage->items[slot.typeIdx];
            }

            return nullptr;
        };


    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hChestUIItemOnCursor))
    {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

        pObj->SetOrigin(_float2{ (float)mousePos.x, (float)mousePos.y });


        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB)
            || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            for (const auto& slot : GetUIController()->GetChest()->GetChestSlots())
            {
                auto itemOrigin = pObj->GetOrigin();
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {
                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                auto funcIfMaxSwapElseCntUp = [&]()
                                    {
                                        auto isMaxOver = pItemInfo->iCnt + pTargetInfo->value().iCnt >= 64;
                                        if (isMaxOver)
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                        else
                                        {
                                            pTargetInfo->value().iCnt += pItemInfo->iCnt;
                                            pObj->SetPendingDestroy();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcIfMaxSwapElseCntUp();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcIfMaxSwapElseCntUp();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                            else
                            {
                                auto funcItemCntAdd = [&]()
                                    {
                                        auto isCanAdd = pTargetInfo->value().iCnt < 64;
                                        if (isCanAdd)
                                        {
                                            pTargetInfo->value().iCnt += 1;

                                            if (pItemInfo->iCnt > 1)
                                            {
                                                pItemInfo->iCnt -= 1;
                                            }
                                            else
                                            {
                                                pObj->SetPendingDestroyCascade();
                                            }
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }
                                    };
                                if (pTargetInfo->value().block && pItemInfo->block)
                                {
                                    if (pTargetInfo->value().block->GetType() == pItemInfo->block->GetType())
                                    {
                                        funcItemCntAdd();
                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }

                                }
                                else if (!pTargetInfo->value().block && pItemInfo->block || pTargetInfo->value().block && !pItemInfo->block)
                                {
                                    std::swap(pTargetInfo->value(), *pItemInfo);
                                    pObj->RePerUI();
                                }
                                else
                                {
                                    if (CItemObject::IsCountableItem(pItemInfo->eItemType))
                                    {
                                        if (pItemInfo->eItemType == pTargetInfo->value().eItemType)
                                        {
                                            funcItemCntAdd();
                                        }
                                        else
                                        {
                                            std::swap(pTargetInfo->value(), *pItemInfo);
                                            pObj->RePerUI();
                                        }

                                    }
                                    else
                                    {
                                        std::swap(pTargetInfo->value(), *pItemInfo);
                                        pObj->RePerUI();
                                    }
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }
                    // 타겟이 없는경우 온커서에 있는거 그대로 넘기기
                    else
                    {
                        if (auto pItemInfo = pObj->GetItemInfoPtr())
                        {
                            if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                            {
                                *pTargetInfo = *pItemInfo;
                                pObj->SetPendingDestroyCascade();

                            }
                            else
                            {
                                if (pItemInfo->block || CItemObject::IsCountableItem(pItemInfo->eItemType))
                                {
                                    if (pItemInfo->iCnt <= 1)
                                    {
                                        *pTargetInfo = *pItemInfo;
                                        pObj->SetPendingDestroyCascade();
                                    }
                                    else
                                    {
                                        CItemObject::ItemInfo copy{ *pItemInfo };
                                        copy.iCnt = 1;

                                        pItemInfo->iCnt -= 1;

                                        *pTargetInfo = copy;
                                    }
                                }
                                else
                                {
                                    *pTargetInfo = *pItemInfo;
                                    pObj->SetPendingDestroyCascade();
                                }
                            }
                        }
                        else
                        {
                            pObj->SetPendingDestroyCascade();
                        }
                    }


                    pObj->SetOrigin(slotOrigin);
                    break;
                }
            }
        }
    }
    // onCursor에 아이템이 없는경우
    else
    {
        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB) || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
        {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

            for (const auto& slot : GetUIController()->GetChest()->GetChestSlots())
            {
                auto itemOrigin = _float2{ (float)mousePos.x, (float)mousePos.y };
                auto slotOrigin = slot.vOriginPos;

                auto slotMinX = slotOrigin.x - 8 * MC_UI_SCALE;
                auto slotMinY = slotOrigin.y - 8 * MC_UI_SCALE;
                auto slotMaxX = slotOrigin.x + 8 * MC_UI_SCALE;
                auto slotMaxY = slotOrigin.y + 8 * MC_UI_SCALE;


                if (itemOrigin.x > slotMinX
                    && itemOrigin.x < slotMaxX
                    && itemOrigin.y > slotMinY
                    && itemOrigin.y < slotMaxY)
                {

                    std::optional<CItemObject::ItemInfo>* pTargetInfo{ invenSlotToMemberItem(slot) };

                    // 해당 아이템 슬롯이 잇는경우에만
                    if (pTargetInfo && pTargetInfo->has_value())
                    {
                        auto funcSpawnOnCursorItem = [&](const CItemObject::ItemInfo& movedItemInfo)
                            {
                                {
                                    E::CUIItem::DESC Desc{};
                                    Desc.fX = itemOrigin.x;
                                    Desc.fY = itemOrigin.y;
                                    Desc.sObjectTag = "UIItem";
                                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
                                        "80_UI", &Desc))
                                    {
                                        m_hChestUIItemOnCursor = handle.value();
                                        if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIItem>(m_hChestUIItemOnCursor))
                                        {
                                            pObj->SetItemInfo(movedItemInfo);
                                        }
                                    }
                                }
                            };


                        auto funcItemDivideToOnCurosr = [&](std::optional<CItemObject::ItemInfo>& targetInfo)
                            {
                                auto currItemCnt = targetInfo.value().iCnt;
                                if (currItemCnt > 1)
                                {
                                    auto dvd2 = currItemCnt / 2;
                                    auto remain = currItemCnt - dvd2;

                                    targetInfo.value().iCnt = remain;

                                    {
                                        CItemObject::ItemInfo newInfo{ targetInfo.value() };
                                        newInfo.iCnt = dvd2;

                                        funcSpawnOnCursorItem(newInfo);
                                    }
                                }
                                else
                                {
                                    {
                                        funcSpawnOnCursorItem(targetInfo.value());
                                    }


                                    *pTargetInfo = std::nullopt;
                                }
                            };



                        if (CGameInstance::Get().MouseDown(MOUSEKEYSTATE::LB))
                        {
                            funcSpawnOnCursorItem(pTargetInfo->value());

                            *pTargetInfo = std::nullopt;
                        }
                        else
                        {
                            auto currItemCnt = pTargetInfo->value().iCnt;
                            if (pTargetInfo->value().block)
                            {
                                funcItemDivideToOnCurosr(*pTargetInfo);
                            }
                            else
                            {
                                if (CItemObject::IsCountableItem(pTargetInfo->value().eItemType))
                                {
                                    funcItemDivideToOnCurosr(*pTargetInfo);
                                }
                                else
                                {
                                    funcSpawnOnCursorItem(pTargetInfo->value());

                                    *pTargetInfo = std::nullopt;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}
//
//void CPlayerEntity::SpawnDropItemObject(const CItemObject::ItemInfo& info, _float3 pos, _float3 vel)
//{
//    if (info.block)
//    {
//        E::CDropItemObject::DESC Desc{};
//        Desc.sObjectTag = "CDropBlock_Cube";
//        Desc.viBufferId = { "MC_ITEM_VIBuffer", CDropItemObject::GetVIBufferName(info) };
//
//        if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropBlock", &Desc))
//        {
//            if (!pLayer->empty())
//            {
//                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropBlock>(pLayer->front()))
//                {
//                    auto type = info.block->GetType();
//
//                    std::vector<uint32_t> texs{};
//                    for (uint32_t i = 0; i < ETOUI(FACE_DIR::END); ++i)
//                    {
//                        texs.push_back(PackTexId(9, ETOUI(CBlock3::GetTexType(type, static_cast<FACE_DIR>(i)))));
//                    }
//
//                    pObj->AddDropItemObject(info, pos, vel, texs);
//                }
//            }
//        }
//    }
//    else
//    {
//        {
//            E::CDropItemObject::DESC Desc{};
//            Desc.sObjectTag = "CDropItem";
//            Desc.viBufferId = { "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(info) };
//
//            if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropItem", &Desc))
//            {
//                if (!pLayer->empty())
//                {
//                    if (au//}to pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropItemObject>(pLayer->front()))
//                    {
//                        pObj->AddDropItemObject(info, pos, vel, { CItemObject::GetPackedTexIdByType(info.eItemType) });
//                    }
//                }
//            }
//        }
//    }


void CPlayerEntity::TakeDamage(int32_t iDamage)
{
    m_iHalfHealth = std::clamp(m_iHalfHealth - iDamage, 0, 20);
    //Coll_PlayerCenter
    if (m_pActivePlayerCamera)
    {
        m_pActivePlayerCamera->TriggerCameraShake(0.1f, 0.1f);
    }


}

void CPlayerEntity::JudgeDeathUpdate(int32_t iDamage)
{
    if (m_iHalfHealth == 0)
    {
        if (!m_bDeath)
        {
            m_eCameraType = CAMERA_TYPE::TPS;
            int x = 0;
            auto pos = GetTransform().GetPosition();
            pos.y += 1.5f;

            auto funcDropItems = [](std::optional<CItemObject::ItemInfo> itemInfo, _float3 pos)
                
                {
                    if (itemInfo->block)
                    {
                        for (uint32_t i = 0; i < itemInfo->iCnt; ++i)
                        {
                            auto copy = itemInfo.value();
                            copy.iCnt = 1;
                            CItemObject::SpawnDropItemObject(copy, pos, { Randf(-5.f, 5.f), Randf(3.f, 5.f), Randf(-5.f, 5.f) });
                        }
                    }
                    else
                    {
                        if (CItemObject::IsCountableItem(itemInfo->eItemType))
                        {
                            for (uint32_t i = 0; i < itemInfo->iCnt; ++i)
                            {
                                auto copy = itemInfo.value();
                                copy.iCnt = 1;
                                CItemObject::SpawnDropItemObject(copy, pos, { Randf(-5.f, 5.f), Randf(3.f, 5.f), Randf(-5.f, 5.f) });
                            }
                        }
                        else
                        {
                            CItemObject::SpawnDropItemObject(itemInfo.value(), pos, { Randf(-5.f, 5.f), Randf(3.f, 5.f), Randf(-5.f, 5.f) });
                        }
                    }
                
                };

            for (uint32_t i = 0; i < 9; ++i)
            {
                if (auto itemInfo = m_ItemArrHotbar[i])
                {
                    funcDropItems(itemInfo, pos);
                }

                m_ItemArrHotbar[i] = std::nullopt;
            }

            for (uint32_t i = 0; i < 9 * 3; ++i)
            {
                if (auto itemInfo = m_ItemArrInventory[i])
                {
                    funcDropItems(itemInfo, pos);
                }

                m_ItemArrInventory[i] = std::nullopt;
            }

            for (uint32_t i = 0; i < 4; ++i)
            {
                if (auto itemInfo = m_ItemArrArmor[i])
                {
                    funcDropItems(itemInfo, pos);
                }

                m_ItemArrArmor[i] = std::nullopt;
            }

            if (auto pObj = CGameInstance::Get().GetFirstGameObjectByLayer<CExperienceOrb>("56_ExperienceOrb"))
            {
                for (uint32_t i = 0; i < m_iLevel; ++i)
                {
                    auto pos = GetTransform().GetPosition();
                    pos.x += Randf(-4.f, 4.f);
                    pos.z += Randf(-4.f, 4.f);
                    pObj->AddOrb(pos, {}, rand() % 16, 1.f);
                }
                if (m_fExperienceGage > 0.f)
                {
                    auto pos = GetTransform().GetPosition();
                    pos.x += Randf(-4.f, 4.f);
                    pos.z += Randf(-4.f, 4.f);
                    pObj->AddOrb(pos, {}, rand() % 16, m_fExperienceGage);
                }
            }
            
            m_iLevel = 0;
            m_fExperienceGage = 0.f;
            //while(m_f)
            
            //ProcessUIHotbar(0.f);
            //CItemObject::SpawnDropItemObject(ItemInfo, pos, { Randf(-1.f, 1.f), Randf(0.5f, 2.f), Randf(-1.f, 1.f) });
        }
        GetUIController()->GetDeathScreen()->SetPlayer(GetHandle());
        m_bDeath = true;
    }
    else
    {
        m_bDeath = false;
    }
}

void CPlayerEntity::ReSpawnFromDeath()
{
    m_bDeath = false;
    m_iHalfHealth = 20;
    m_iHalfHunger = 20;
    GetTransform().SetPosition(_float3{ 0.5f, 70.f, 0.5f });

    GetTransform().SetQuaternion(XMVectorSet(0.f, 0.f, 0.f, 1.f));

    m_fDeathAnimTimer = 0.f;

    m_eCameraType = CAMERA_TYPE::FPS;
}

void CPlayerEntity::ProcessUIStatus(_float fTimeDelta)
{
    ProcessUIStatusHealth(fTimeDelta);
    ProcessUIStatusHunger(fTimeDelta);
    ProcessUIStatusArmor(fTimeDelta);
    ProcessUIStatusLevel(fTimeDelta);
    ProcessUIStatusBreath(fTimeDelta);
}
void CPlayerEntity::ProcessUIStatusHealth(_float fTimeDelta)
{
    GetUIController()->GetHealthBar()->SetHalfHealth(m_iHalfHealth);
}
void CPlayerEntity::ProcessUIStatusHunger(_float fTimeDelta)
{
    GetUIController()->GetHungerBar()->SetHalfHunger(m_iHalfHunger);
}
void CPlayerEntity::ProcessUIStatusArmor(_float fTimeDelta)
{
    uint32_t halfArmor = 0;

    // Helmet (0, 1, 2, 3, 3)
    if (m_ItemArrArmor[0])
    {
        switch (m_ItemArrArmor[0]->eItemType)
        {
        case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:     halfArmor += 1; break;
        case CItemObject::ITEM_TYPE::ITEM_IronHelmet:       halfArmor += 2; break;
        case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:       halfArmor += 2; break;
        case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:    halfArmor += 3; break;
        case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:  halfArmor += 3; break;
        }
    }

    // Chestplate (4, 6, 5, 8, 8)
    if (m_ItemArrArmor[1])
    {
        switch (m_ItemArrArmor[1]->eItemType)
        {
        case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:    halfArmor += 4; break;
        case CItemObject::ITEM_TYPE::ITEM_IronChestplate:      halfArmor += 6; break;
        case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:      halfArmor += 5; break;
        case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:   halfArmor += 8; break;
        case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate: halfArmor += 8; break;
        }
    }

    // Leggings (3, 5, 3, 6, 6)
    if (m_ItemArrArmor[2])
    {
        switch (m_ItemArrArmor[2]->eItemType)
        {
        case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:    halfArmor += 3; break;
        case CItemObject::ITEM_TYPE::ITEM_IronLeggings:      halfArmor += 5; break;
        case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:      halfArmor += 3; break;
        case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:   halfArmor += 6; break;
        case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings: halfArmor += 6; break;
        }
    }

    // Boots (1, 2, 1, 3, 3)
    if (m_ItemArrArmor[3])
    {
        switch (m_ItemArrArmor[3]->eItemType)
        {
        case CItemObject::ITEM_TYPE::ITEM_CopperBoots:    halfArmor += 1; break;
        case CItemObject::ITEM_TYPE::ITEM_IronBoots:      halfArmor += 2; break;
        case CItemObject::ITEM_TYPE::ITEM_GoldBoots:      halfArmor += 1; break;
        case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:   halfArmor += 3; break;
        case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots: halfArmor += 3; break;
        }
    }
    m_iRealHealfArmor = halfArmor;
    // 최종적으로 20을 넘지 않도록 제한
    if (halfArmor > 20) halfArmor = 20;
    m_iHalfArmor = halfArmor;

    if (m_iHalfArmor == 0)
    {
        GetUIController()->GetArmorBar()->SetRender(false);
    }
    else
    {
        GetUIController()->GetArmorBar()->SetRender(true);
        GetUIController()->GetArmorBar()->SetHalfArmor(m_iHalfArmor);
    }
}
void CPlayerEntity::ProcessUIStatusLevel(_float fTimeDelta)
{
    GetUIController()->GetExperienceBar()->SetLevel(m_iLevel);
    GetUIController()->GetExperienceBar()->SetGage(m_fExperienceGage);
}
void CPlayerEntity::ProcessUIStatusBreath(_float fTimeDelta)
{
    if (m_iBreath == 10)
    {
        GetUIController()->GetBreathBar()->SetRender(false);
    }
    else
    {
        GetUIController()->GetBreathBar()->SetRender(true);
        GetUIController()->GetBreathBar()->SetBreathCnt(m_iBreath);
    }
}

void CPlayerEntity::ProcessHungerTimer(_float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    if (!m_bDeath)
    {
        m_fHungerTimer += fTimeDelta;
        if (m_fHungerTimer > 5.f)
        {
            m_fHungerTimer = 0.f;

            if (m_iHalfHunger > 0)
            {
                m_iHalfHunger -= 1;
            }
        }
        if (m_iHalfHunger == 0)
        {
            m_fHungerDamageTimer += fTimeDelta;

            if (m_fHungerDamageTimer > 1.f)
            {
                m_fHungerDamageTimer = 0.f;
                TakeDamage(1);
            }
        }
    }
    
}

void CPlayerEntity::ProcessHealthRegenTimer(_float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    if (m_iHalfHunger >= 20)
    {
        m_fHealthRegenTimer += fTimeDelta;
        if (m_fHealthRegenTimer > 1.f)
        {
            m_fHealthRegenTimer = 0.f;
            m_iHalfHealth = std::clamp(m_iHalfHealth + 1, 0, 20);
        }
    }
}

void CPlayerEntity::ProcessArmorEntities(_float fTimeDelta)
{
    size_t HelmetIdx = 0;
    size_t ChestplateIdx = 1;
    size_t LeggingsIdx = 2;
    size_t BootsIdx = 3;

    auto* pHelemt = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[HelmetIdx]);
    auto* pChestplate = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[ChestplateIdx]);
    auto* pLeggings = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[LeggingsIdx]);
    auto* pBoots = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[BootsIdx]);

    // Helmet
    {
        if (m_ItemArrArmor[HelmetIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[HelmetIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pHelemt)
            {
                if (pHelemt->GetArmorMade() != made)
                {
                    pHelemt->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::HELMET;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorHelmet";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hArmorEntities[HelmetIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pHelemt)
            {
                pHelemt->SetPendingDestroyCascade();
            }
        }
    }


    // Chestplate
    {
        if (m_ItemArrArmor[ChestplateIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[ChestplateIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pChestplate)
            {
                if (pChestplate->GetArmorMade() != made)
                {
                    pChestplate->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::CHESTPLATE;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorChestplate";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hArmorEntities[ChestplateIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pChestplate)
            {
                pChestplate->SetPendingDestroyCascade();
            }
        }
    }

    // Leggings
    {
        if (m_ItemArrArmor[LeggingsIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[LeggingsIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pLeggings)
            {
                if (pLeggings->GetArmorMade() != made)
                {
                    pLeggings->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::LEGGINGS;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorLeggings";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hArmorEntities[LeggingsIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pLeggings)
            {
                pLeggings->SetPendingDestroyCascade();
            }
        }
    }


    // Boots
    {
        if (m_ItemArrArmor[BootsIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[BootsIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronBoots:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pBoots)
            {
                if (pBoots->GetArmorMade() != made)
                {
                    pBoots->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::BOOTS;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorBoots";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hArmorEntities[BootsIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pBoots)
            {
                pBoots->SetPendingDestroyCascade();
            }
        }
    }



    // second 
    {
        auto* pHelemt = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[HelmetIdx]);
        auto* pChestplate = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[ChestplateIdx]);
        auto* pLeggings = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[LeggingsIdx]);
        auto* pBoots = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hArmorEntities[BootsIdx]);

        if (m_eCameraType == CAMERA_TYPE::TPS)
        {
            auto pos = GetTransform().GetLoadedPostion();
            auto root = m_pComEntityModel->GetBone("root");
            auto waist =m_pComEntityModel->GetBone("waist");
            auto body = m_pComEntityModel->GetBone("body");
            auto head = m_pComEntityModel->GetBone("head");
            auto hat = m_pComEntityModel->GetBone("hat");
            auto rightArm = m_pComEntityModel->GetBone("rightArm");
            auto leftArm = m_pComEntityModel->GetBone("leftArm");
            auto rightLeg = m_pComEntityModel->GetBone("rightLeg");
            auto leftLeg = m_pComEntityModel->GetBone("leftLeg");

            

            if (pHelemt)
            {
                pHelemt->SetRender(true);
                pHelemt->GetTransform().SetPosition(pos);

                auto waist2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto head2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("head");
                auto hat2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("hat");
                _float3 pluswaistrot;
                XMStoreFloat3(&pluswaistrot, XMLoadFloat3(root->GetRotation()) + XMLoadFloat3(waist->GetRotation()));
                waist2->SetRotation(pluswaistrot);
                body2->SetRotation(*body->GetRotation());
                head2->SetRotation(*head->GetRotation());
                hat2->SetRotation(*hat->GetRotation());
            }
            if (pChestplate)
            {
                pChestplate->SetRender(true);
                pChestplate->GetTransform().SetPosition(pos);

                auto waist2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightArm2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightArm");
                auto lefttArm2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftArm");
                _float3 pluswaistrot;
                XMStoreFloat3(&pluswaistrot, XMLoadFloat3(root->GetRotation()) + XMLoadFloat3(waist->GetRotation()));
                waist2->SetRotation(pluswaistrot);
                body2->SetRotation(*body->GetRotation());
                rightArm2->SetRotation(*rightArm->GetRotation());
                lefttArm2->SetRotation(*leftArm->GetRotation());
            }
            if (pLeggings)
            {
                pLeggings->SetRender(true);
                pLeggings->GetTransform().SetPosition(pos);

                auto waist2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightLeg2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightLeg");
                auto leftLeg2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftLeg");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                rightLeg2->SetRotation(*rightLeg->GetRotation());
                leftLeg2->SetRotation(*leftLeg->GetRotation());
            }
            if (pBoots)
            {
                pBoots->SetRender(true);
                pBoots->GetTransform().SetPosition(pos);

                auto waist2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightLeg2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightLeg");
                auto leftLeg2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftLeg");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                rightLeg2->SetRotation(*rightLeg->GetRotation());
                leftLeg2->SetRotation(*leftLeg->GetRotation());
            }
        }
        else
        {
            if (pHelemt)
            {
                pHelemt->SetRender(false);
            }
            if (pChestplate)
            {
                pChestplate->SetRender(false);
            }
            if (pLeggings)
            {
                pLeggings->SetRender(false);
            }
            if (pBoots)
            {
                pBoots->SetRender(false);
            }
        }
    }

}

void CPlayerEntity::ProcessPlayerOpenInvenArmorEntities(_float fTimeDelta)
{
    size_t HelmetIdx = 0;
    size_t ChestplateIdx = 1;
    size_t LeggingsIdx = 2;
    size_t BootsIdx = 3;

    auto* pHelemt = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[HelmetIdx]);
    auto* pChestplate = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[ChestplateIdx]);
    auto* pLeggings = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[LeggingsIdx]);
    auto* pBoots = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[BootsIdx]);

    // Helmet
    {
        if (m_ItemArrArmor[HelmetIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[HelmetIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperHelmet:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronHelmet:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldHelmet:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondHelmet:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteHelmet:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pHelemt)
            {
                if (pHelemt->GetArmorMade() != made)
                {
                    pHelemt->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::HELMET;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorHelmet";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hPlayerOpenInvenArmorEntities[HelmetIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pHelemt)
            {
                pHelemt->SetPendingDestroyCascade();
            }
        }
    }


    // Chestplate
    {
        if (m_ItemArrArmor[ChestplateIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[ChestplateIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperChestplate:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronChestplate:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldChestplate:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondChestplate:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteChestplate:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pChestplate)
            {
                if (pChestplate->GetArmorMade() != made)
                {
                    pChestplate->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::CHESTPLATE;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorChestplate";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hPlayerOpenInvenArmorEntities[ChestplateIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pChestplate)
            {
                pChestplate->SetPendingDestroyCascade();
            }
        }
    }

    // Leggings
    {
        if (m_ItemArrArmor[LeggingsIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[LeggingsIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperLeggings:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronLeggings:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldLeggings:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondLeggings:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteLeggings:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pLeggings)
            {
                if (pLeggings->GetArmorMade() != made)
                {
                    pLeggings->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::LEGGINGS;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorLeggings";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hPlayerOpenInvenArmorEntities[LeggingsIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pLeggings)
            {
                pLeggings->SetPendingDestroyCascade();
            }
        }
    }


    // Boots
    {
        if (m_ItemArrArmor[BootsIdx])
        {
            E::CArmorEntity::ARMOR_MADE made{ E::CArmorEntity::ARMOR_MADE::END };
            switch (m_ItemArrArmor[BootsIdx]->eItemType)
            {
            case CItemObject::ITEM_TYPE::ITEM_CopperBoots:
                made = CArmorEntity::ARMOR_MADE::COPPER;
                break;
            case CItemObject::ITEM_TYPE::ITEM_IronBoots:
                made = CArmorEntity::ARMOR_MADE::IRON;
                break;
            case CItemObject::ITEM_TYPE::ITEM_GoldBoots:
                made = CArmorEntity::ARMOR_MADE::GOLD;
                break;
            case CItemObject::ITEM_TYPE::ITEM_DiamondBoots:
                made = CArmorEntity::ARMOR_MADE::DIAMOND;
                break;
            case CItemObject::ITEM_TYPE::ITEM_NetheriteBoots:
                made = CArmorEntity::ARMOR_MADE::NETHERITE;
                break;
            }

            if (pBoots)
            {
                if (pBoots->GetArmorMade() != made)
                {
                    pBoots->SetPendingDestroyCascade();
                }
            }
            else
            {
                if (made != E::CArmorEntity::ARMOR_MADE::END)
                {
                    E::CArmorEntity::DESC Desc{};
                    Desc.eArmorType = E::CArmorEntity::ARMOR_TYPE::BOOTS;
                    Desc.eArmorMade = made;
                    Desc.sObjectTag = "ArmorBoots";
                    if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArmorEntity",
                        "45_ARMOR", &Desc))
                    {
                        m_hPlayerOpenInvenArmorEntities[BootsIdx] = handle.value();
                    }
                }
            }
        }
        else
        {
            if (pBoots)
            {
                pBoots->SetPendingDestroyCascade();
            }
        }
    }



    // second 
    {
        auto* pHelemt = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[HelmetIdx]);
        auto* pChestplate = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[ChestplateIdx]);
        auto* pLeggings = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[LeggingsIdx]);
        auto* pBoots = CGameInstance::Get().GetGameObjectByHandleT<CArmorEntity>(m_hPlayerOpenInvenArmorEntities[BootsIdx]);

        
            auto pos = GetTransform().GetLoadedPostion();
            auto root = m_pComInventoryPlayerEntityModel->GetBone("root");
            auto waist = m_pComInventoryPlayerEntityModel->GetBone("waist");
            auto body = m_pComInventoryPlayerEntityModel->GetBone("body");
            auto head = m_pComInventoryPlayerEntityModel->GetBone("head");
            auto hat = m_pComInventoryPlayerEntityModel->GetBone("hat");
            auto rightArm = m_pComInventoryPlayerEntityModel->GetBone("rightArm");
            auto leftArm = m_pComInventoryPlayerEntityModel->GetBone("leftArm");
            auto rightLeg = m_pComInventoryPlayerEntityModel->GetBone("rightLeg");
            auto leftLeg = m_pComInventoryPlayerEntityModel->GetBone("leftLeg");



            if (pHelemt)
            {
                pHelemt->SetRender(true);
                //pHelemt->GetTransform().SetPosition(pos);

                auto waist2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto head2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("head");
                auto hat2 = pHelemt->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("hat");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                head2->SetRotation(*head->GetRotation());
                hat2->SetRotation(*hat->GetRotation());
            }
            if (pChestplate)
            {
                pChestplate->SetRender(true);
                //pChestplate->GetTransform().SetPosition(pos);

                auto waist2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightArm2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightArm");
                auto lefttArm2 = pChestplate->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftArm");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                rightArm2->SetRotation(*rightArm->GetRotation());
                lefttArm2->SetRotation(*leftArm->GetRotation());
            }
            if (pLeggings)
            {
                pLeggings->SetRender(true);
                //pLeggings->GetTransform().SetPosition(pos);

                auto waist2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightLeg2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightLeg");
                auto leftLeg2 = pLeggings->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftLeg");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                rightLeg2->SetRotation(*rightLeg->GetRotation());
                leftLeg2->SetRotation(*leftLeg->GetRotation());
            }
            if (pBoots)
            {
                pBoots->SetRender(true);
                //pBoots->GetTransform().SetPosition(pos);

                auto waist2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("waist");
                auto body2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("body");
                auto rightLeg2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("rightLeg");
                auto leftLeg2 = pBoots->GetComponent<CComEntityModel>("Com_EntityModel")->GetBone("leftLeg");

                waist2->SetRotation(*root->GetRotation());
                body2->SetRotation(*body->GetRotation());
                rightLeg2->SetRotation(*rightLeg->GetRotation());
                leftLeg2->SetRotation(*leftLeg->GetRotation());
            }
        //{
        //    if (pHelemt)
        //    {
        //        pHelemt->SetRender(false);
        //    }
        //    if (pChestplate)
        //    {
        //        pChestplate->SetRender(false);
        //    }
        //    if (pLeggings)
        //    {
        //        pLeggings->SetRender(false);
        //    }
        //    if (pBoots)
        //    {
        //        pBoots->SetRender(false);
        //    }
        //}
    }

}

void CPlayerEntity::ProcessPlayerOpenInvenAction(_float fTimeDelta)
{
    if (GetUIController()->Getinventory()->GetRender())
    {
        if (auto cam = CGameInstance::Get().GetGameCamera("PlayerInvenUI"))
        {
            m_pComInventoryPlayerEntityModel->ResetBonesChannel();

            {
                static float fTmp2 = 0;
                fTmp2 += fTimeDelta;;
                float lifeTime = fTmp2;
                float bob = (cosf(lifeTime * XMConvertToRadians(103.2f)) * 2.865f) + 2.865f;

                if (auto pLeftArm = m_pComInventoryPlayerEntityModel->GetBone("leftArm"))
                {
                    pLeftArm->GetRotation()->z += XMConvertToRadians(-bob);
                }

                if (auto pRightArm = m_pComInventoryPlayerEntityModel->GetBone("rightArm"))
                {
                    pRightArm->GetRotation()->z += XMConvertToRadians(bob);
                }
            }

            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(CGameInstance::Get().GetHwnd(), &mousePos);

            _float2 clientSize = CGameInstance::Get().GetClientScreenSize();
            //_float2 clientSize = {256.f, 256.f };

            float ndcX = (2.0f * mousePos.x / (clientSize.x)) - 1.0f;
            float ndcY = 1.0f - (2.0f * mousePos.y / (clientSize.y));

            auto pHeadBone = m_pComInventoryPlayerEntityModel->GetBone("head");



            // 1. 역행렬 미리 계산 (V * P의 역행렬)
            _matrix V = cam->GetView();
            _matrix P = cam->GetProj();
            _matrix invVP = XMMatrixInverse(nullptr, V * P);

            // 2. 마우스 좌표(NDC)를 월드 좌표로 변환 (Z=0은 Near Plane)
            _vector mouseNear = XMVector3TransformCoord(XMVectorSet(ndcX, ndcY, 0.0f, 1.0f), invVP);
            _vector mouseFar = XMVector3TransformCoord(XMVectorSet(ndcX, ndcY, 1.0f, 1.0f), invVP);

            // 3. 카메라와 캐릭터 머리 사이의 거리(약 4.0f)만큼 떨어진 지점을 타겟으로 설정
            // 마우스 레이 상에서 캐릭터 머리 Z깊이와 동일한 지점의 월드 좌표를 찾습니다.
            _vector dir = XMVector3Normalize(mouseFar - mouseNear);
            _float3 targetPos;
            // 캐릭터 머리가 대략 (0,0,0) 근처라면, Z축 기준으로 타겟을 잡습니다.
            XMStoreFloat3(&targetPos, mouseNear + dir * 4.0f);

            // 4. 머리 위치에서 타겟을 향하는 방향 벡터 계산
            _float3 headWorldPos = *pHeadBone->GetTranslatoin();
            headWorldPos.x -= 0.5f;
            headWorldPos.y -= 1.5f;
            _float3 lookDir = { -targetPos.x - headWorldPos.x, -targetPos.y - headWorldPos.y, -targetPos.z - headWorldPos.z };

            // 5. 회전 계산 (이전 코드 재활용)
            float targetYaw = atan2f(lookDir.x, lookDir.z);
            float distXZ = sqrtf(lookDir.x * lookDir.x + lookDir.z * lookDir.z);
            float targetPitch = atan2f(-lookDir.y, distXZ);

            pHeadBone->GetRotation()->y = -targetYaw;
            pHeadBone->GetRotation()->x = -targetPitch;


        }
    }
}

void CPlayerEntity::ProcessPlayerCameraAction(_float fTimeDelta)
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
            if (m_hRightItem)
            {
                if (auto pItem = CGameInstance::Get().GetGameObjectByHandle(m_hRightItem.value()))
                {
                    tmpCamera->GetTransform().Update();
                    XMMATRIX matCameraWorld = tmpCamera->GetTransform().GetLoadedCombinedWorldMatrix();

                    XMMATRIX matScale = XMMatrixScaling(1.f, 1.f, 1.f);
                    XMMATRIX matTrans = XMMatrixTranslation(0.25f, -0.22f, 0.4f);
                    XMMATRIX matBaseOffset = matScale * matTrans;

                    XMMATRIX matFinalRotation = XMMatrixIdentity();


                    //static bool  bIsSwinging = false;
                    static float fSwingProgress = 0.f;
                    static float fEatingProgress = 0.f;
                    static float fBowPullingProgress = 0.f;

                   

                    if (auto pArm = Cast<CPlayerFPSArm>(pItem))
                    {
                        if (m_bMousePressingLeft)
                        {
                            // normal swing
                            if (m_ePlay != PLAY::NORMAL_SWING)
                            {
                                m_ePlay = PLAY::NORMAL_SWING;
                                fSwingProgress = 0.f;
                            }
                        }
                        else if (m_bMousePressingRight)
                        {
                            // normal swing
                            if (m_ePlay != PLAY::NORMAL_SWING)
                            {
                                m_ePlay = PLAY::NORMAL_SWING;
                                fSwingProgress = 0.f;
                            }
                        }
                    }
                    else if (auto pHandHeld = Cast<CHandHeldItem>(pItem))
                    {
                        if (auto pInfo = pHandHeld->GetItemInfo())
                        {
                            if (pInfo->block)
                            {
                                if (m_bMousePressingLeft)
                                {
                                    // normal swing
                                    if (m_ePlay != PLAY::NORMAL_SWING)
                                    {
                                        m_ePlay = PLAY::NORMAL_SWING;
                                        fSwingProgress = 0.f;
                                    }
                                }
                                else if (m_bMousePressingRight)
                                {
                                    // normal swing
                                    if (m_ePlay != PLAY::NORMAL_SWING)
                                    {
                                        m_ePlay = PLAY::NORMAL_SWING;
                                        fSwingProgress = 0.f;
                                    }
                                }

                                matFinalRotation = XMMatrixRotationRollPitchYaw(
                                    XMConvertToRadians(-15.f),
                                    XMConvertToRadians(35.f),
                                    XMConvertToRadians(0.f)
                                );
                            }
                            else
                            {
                                if (auto icnt = CItemObject::IsEatableItem(pInfo->eItemType))
                                {
                                    if (m_bMousePressingLeft)
                                    {
                                        // normal swing
                                        if (m_ePlay != PLAY::NORMAL_SWING)
                                        {
                                            m_ePlay = PLAY::NORMAL_SWING;
                                            fSwingProgress = 0.f;
                                        }
                                    }
                                    else if (m_bMousePressingRight)
                                    {
                                        // eating
                                        if (m_ePlay != PLAY::EATING)
                                        {
                                            m_ePlay = PLAY::EATING;
                                            fEatingProgress = 0.f;
                                        }
                                        
                                    }
                                }
                                else if (
                                    pInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby
                                    || pInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0
                                    || pInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1
                                    || pInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2
                                    )
                                {
                                    if (m_bMousePressingLeft)
                                    {
                                        // normal swing
                                        if (m_ePlay != PLAY::NORMAL_SWING)
                                        {
                                            m_ePlay = PLAY::NORMAL_SWING;
                                            fSwingProgress = 0.f;
                                        }
                                    }
                                    else if (m_bMousePressingRight || m_bMouseUpRight)
                                    {
                                        // bow pulling
                                        if (m_ePlay != PLAY::BOW_PULLING)
                                        {
                                            m_ePlay = PLAY::BOW_PULLING;
                                            fBowPullingProgress = 0.f;
                                        }
                                    }
                                    else
                                    {
                                        m_ePlay = PLAY::BOW_PULLING;
                                        fBowPullingProgress = 0.f;
                                    }
                                }
                                else
                                {
                                    if (m_bMousePressingLeft)
                                    {
                                        // normal swing
                                        if (m_ePlay != PLAY::NORMAL_SWING)
                                        {
                                            m_ePlay = PLAY::NORMAL_SWING;
                                            fSwingProgress = 0.f;
                                        }
                                    }
                                    else if (m_bMousePressingRight)
                                    {
                                        // normal swing
                                        if (m_ePlay != PLAY::NORMAL_SWING)
                                        {
                                            m_ePlay = PLAY::NORMAL_SWING;
                                            fSwingProgress = 0.f;
                                        }
                                    }
                                }
                                XMMATRIX matToPivot = XMMatrixTranslation(0.f, 0.1f, 0.1f);

                                XMMATRIX matRot = XMMatrixRotationRollPitchYaw(
                                    XMConvertToRadians(0.f),
                                    XMConvertToRadians(90.f),
                                    XMConvertToRadians(0.f)
                                );

                                matFinalRotation = matRot * matToPivot;
                            }
                        }
                    }

                    XMMATRIX matGlobalSwing = XMMatrixIdentity();
                    switch (m_ePlay)
                    {
                    case PLAY::IDLE:


                        break;
                    case PLAY::NORMAL_SWING:
                    {
                        fSwingProgress += fTimeDelta * 5.f;

                        if (fSwingProgress >= 1.f)
                        {
                            fSwingProgress = 1.f;
                            m_ePlay = PLAY::IDLE;
                        }


                        float fAngleFactor = sinf(fSwingProgress * XM_PI); // 0.0 -> 1.0 -> 0.0 

                        matGlobalSwing = XMMatrixRotationRollPitchYaw(
                            fAngleFactor * XMConvertToRadians(90.f),
                            0.f,
                            0.f
                        );
                    }
                        break;
                    case PLAY::EATING:
                    {
                        fEatingProgress += fTimeDelta * 1.f;

                        if (fEatingProgress >= 1.f)
                        {
                            fEatingProgress = 1.f;
                            m_ePlay = PLAY::IDLE;

                            
                            auto currHotbarIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
                            if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
                            {
                                if (auto iCnt = CItemObject::IsEatableItem(hotbarItemInfo->eItemType))
                                {
                                    if (hotbarItemInfo->iCnt > 0)
                                    {
                                        m_fHungerTimer = 0.f;
                                        m_iHalfHunger = std::clamp(m_iHalfHunger + (int32_t)iCnt, 0, 20);
                                        hotbarItemInfo->iCnt -= 1;

                                        if (hotbarItemInfo->iCnt == 0)
                                        {
                                            hotbarItemInfo = std::nullopt;
                                        }
                                    }
                                    
                                }
                            }
                        }
                        

                        if (auto pHandHeld = Cast<CHandHeldItem>(pItem))
                        {
                            if (auto pInfo = pHandHeld->GetItemInfo())
                            {
                                if (CItemObject::IsEatableItem(pInfo->eItemType))
                                {
                                    _float3 startPos{};
                                    XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 0.3f);
                                    startPos.y += 1.5f;
                                    //auto currpos = GetTransform().GetPosition();
                                    //currpos.y += 1.8f;

                                    auto iPackedTexID = CItemObject::GetPackedTexIdByType(pInfo->eItemType);
                                    CGameInstance::Get().AddParticleRenderDestruct(
                                        startPos,
                                        iPackedTexID,
                                        1);
                                }
                            }
                        }

                       

                        //CGameInstance::Get().AddParticleRenderDestruct(res.block.value(), { (float)res.iWorldBlockX + 0.5f, (float)res.iWorldBlockY + 0.5f, (float)res.iWorldBlockZ + 0.5f });

                        float fAngleFactor = sinf(fEatingProgress * XM_PI * 2.f * 4.f); // 0.0 -> 1.0 -> 0.0 
                        auto tmp = XMMatrixTranslation(-0.15f, -0.2f, 0.);
                        auto tmp2 = XMMatrixRotationRollPitchYaw(
                            0.f,
                            XMConvertToRadians(-90.f),
                            0.f
                        );

                        auto tmp3 = XMMatrixTranslation(-0, fAngleFactor * 0.1f, 0.);
                        matGlobalSwing = tmp3 * tmp2 * tmp;
                    }

                        break;
                    case PLAY::BOW_PULLING:
                        fBowPullingProgress += fTimeDelta * 1.f;


                        auto currHotbarIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
                        if (auto& hotbarItemInfo = m_ItemArrHotbar[currHotbarIdx])
                        {
                            if (
                                hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby
                                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0
                                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1
                                || hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2
                                )
                            {
                                auto funcShootArrow = [&](_float fSpeed)
                                    {
                                        {
                                            E::CArrowEntity::DESC Desc{};
                                            Desc.sObjectTag = "Arrow";
                                            if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ArrowEntity",
                                                "49_ARROW", &Desc))
                                            {
                                                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CArrowEntity>(handle.value()))
                                                {
                                                    const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
                                                    auto pos = GetTransform().GetPosition();
                                                    pos.y += 1.8f;
                                                    //auto look = GetTransform().GetState(STATE::LOOK);
                                                    //auto pos = XMLoadFloat3(&rayOrigin2);
                                                    auto look = XMLoadFloat3(&rayDir2);

                                                    pObj->Shoot(XMLoadFloat3(&pos), look, fSpeed);
                                                }
                                            }
                                        }
                                    };
                                if (fBowPullingProgress <= 0.1f)
                                {
                                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Standby;
                                }
                                else if (fBowPullingProgress <= 0.4f)
                                {
                                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0;
                                    if (m_bMouseUpRight)
                                    {
                                        funcShootArrow(15.f);
                                    }
                                }
                                else if (fBowPullingProgress <= 0.7f)
                                {
                                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1;
                                    if (m_bMouseUpRight)
                                    {
                                        funcShootArrow(25.f);
                                    }
                                }
                                else
                                {
                                    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2;
                                    if (m_bMouseUpRight)
                                    {
                                        funcShootArrow(45.f);
                                    }
                                }
                                //else if (fBowPullingProgress <= 0.6f)
                                //{

                                //}

                                //if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Standby)
                                //{
                                //    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0;
                                //}
                                //else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_0)
                                //{
                                //    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1;
                                //}
                                //else if (hotbarItemInfo->eItemType == CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_1)
                                //{
                                //    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Pulling_2;
                                //}
                                //else // ITEM_Bow_Pulling_2
                                //{
                                //    hotbarItemInfo->eItemType = CItemObject::ITEM_TYPE::ITEM_Bow_Standby;
                                //}
                            }
                        }
                        if (fBowPullingProgress >= 1.f)
                        {
                            
                        }

                        break;
                    }



                    //if (
                    //    (CGameInstance::Get().MousePressing(MOUSEKEYSTATE::LB) || CGameInstance::Get().MouseDown(MOUSEKEYSTATE::RB))
                    //    && !bIsSwinging)
                    //{
                    //    bIsSwinging = true;
                    //    fSwingProgress = 0.f;
                    //}


                    //XMMATRIX matGlobalSwing = XMMatrixIdentity();

                    //if (bIsSwinging)
                    //{
                    //    fSwingProgress += fTimeDelta * 5.f;

                    //    if (fSwingProgress >= 1.f)
                    //    {
                    //        fSwingProgress = 1.f;
                    //        bIsSwinging = false; //
                    //    }


                    //    float fAngleFactor = sinf(fSwingProgress * XM_PI); // 0.0 -> 1.0 -> 0.0 

                    //    if (false)
                    //    {
                    //        auto tmp = XMMatrixTranslation(-0.15f, -0.2f, 0.);

                    //        auto tmp2 = XMMatrixRotationRollPitchYaw(
                    //            0.f,
                    //            XMConvertToRadians(-90.f),
                    //            0.f
                    //        );
                    //        
                    //        auto tmp3 = XMMatrixTranslation(-0, fAngleFactor * 0.1f, 0.);

                    //        matGlobalSwing = tmp3 * tmp2 * tmp;
                    //    }
                    //    else
                    //    {
                    //        matGlobalSwing = XMMatrixRotationRollPitchYaw(
                    //            fAngleFactor * XMConvertToRadians(90.f),
                    //            0.f,
                    //            0.f
                    //        );
                    //    }
                    //    
                    //}



                    XMMATRIX matFinalParent = matFinalRotation * matGlobalSwing * matBaseOffset * matCameraWorld;

                    _float4x4 matParent;
                    XMStoreFloat4x4(&matParent, matFinalParent);
                    pItem->GetTransform().SetParentWorldMatrix(matParent);
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
        // camera control
        {
            PlayerCameraTrace(fTimeDelta);
        }


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
                float attackBodyRotY = (float)m_iMouseMoveX;  // 현재 head 회전값

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


        if (m_hRightItem)
        {
            if (auto pItem = CGameInstance::Get().GetGameObjectByHandle(m_hRightItem.value()))
            {
                auto pRightItem = m_pComEntityModel->GetBone("rightItem");
                auto pos = GetTransform().GetPosition();

                // 1. 기존 로컬 행렬 가져옴
                _matrix matLocal = XMLoadFloat4x4(pRightItem->GetCombinedTransformationMatrix());

                // 2. Y축 회전 적용
                float fRotationY = XMConvertToRadians(90.0f);
                _matrix matRotateY = XMMatrixRotationY(fRotationY) * XMMatrixRotationX(XMConvertToRadians(90.0f));
                _matrix matResult = matRotateY * matLocal;

                // 3. 앞으로 전진 (Local Forward Vector 활용)
                float fForwardDistance = 0.3f; // 전진할 거리 (수치를 조절하세요)
                _vector vForward = matResult.r[1]; // 행렬의 3번째 열이 Local Forward 방향입니다.
                _vector vOffset = XMVectorScale(vForward, fForwardDistance);

                // 4. 위치 성분에 오프셋 더하기
                matResult.r[3] = XMVectorAdd(matResult.r[3], vOffset); // 전진 이동 적용
                matResult.r[3] = XMVectorAdd(matResult.r[3], XMLoadFloat3(&pos)); // 월드 위치 적용

                _float4x4 asdf;
                XMStoreFloat4x4(&asdf, matResult);
                pItem->GetTransform().SetParentWorldMatrix(asdf);
            }
        }
        
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
