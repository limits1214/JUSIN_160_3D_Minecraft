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

    if (ImGui::Button("Test Drop Item"))
    {
        if(1)
        {
            CItemObject::ItemInfo ItemInfo{};
            //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_WoodPickaxe;
            ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;

            //CUIItem::GetPerUIByType(ItemInfo);
           
            ;

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

                    //ItemInfo.eItemUIType = CUIItem::TYPE::ITEM_CooperPickaxe;
                    ItemInfo.eItemType = CItemObject::ITEM_TYPE::ITEM_CooperPickaxe;
                    woodPixaxeObj->AddDropItemObject(ItemInfo, pos, {}, { CItemObject::GetPackedTexIdByType(CItemObject::ITEM_TYPE::ITEM_CooperPickaxe) });
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
    auto* pDesc = static_cast<DESC*>(pArg);
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
    m_hFurnaceStorage = pDesc->hFurnaceStorage;
   
    //m_pCenterCollider = CCollSphere::Create({0.f, 0.3f, 0.f}, 0.3f);
    m_pCenterCollider = CCollBox::Create({ 0.f, 1.f, 0.f }, { 0.25f, 0.9f, 0.25f });

    ReadyPlayerItem();

    // TEST
    CItemObject::RecipeInitialize();
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
    }
}

void CPlayerEntity::Update(E::_float fTimeDelta)
{
    ProcessActionUpdate(fTimeDelta);


    ProcessThrowItem(fTimeDelta);
    ProcessRightClick(fTimeDelta);

    ProcessDestroyStage(fTimeDelta);

    ProcessUI(fTimeDelta);


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
    ProcessItemColliding(fTimeDelta);
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

void CPlayerEntity::ProcessDestroyStage(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    auto* pDestroyStage = GetDestroyStage();
    if (!pDestroyStage) return;

    static float fElapsed = 0;
    

    //
    
    //CGameInstance::Get().VoxelBlockRaycast()

    const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
    std::optional<std::pair<XMINT3, uint8_t>> currDestoryTarget{};
    CVoxelManager3::BLOCK_RAY_RESULT res;
    if (CGameInstance::Get().VoxelBlockRaycast(rayOrigin2, rayDir2, 5.f, res))
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

        pDestroyStage->SetFrameIndex(fElapsed / (goal / 9));


        CGameInstance::Get().AddParticleRenderDestruct(res.block.value(), { (float)res.iWorldBlockX + 0.5f, (float)res.iWorldBlockY + 0.5f, (float)res.iWorldBlockZ + 0.5f });

        if (pDestroyStage->GetFrameIndex() == 9)
        {
            CBlock3 newBlock{};
            newBlock.SetType(CBlock3::TYPE::AIR);
            CGameInstance::Get().VoxelProcessPlayerBlockSet(res.iWorldBlockX, res.iWorldBlockY, res.iWorldBlockZ, newBlock);
        

            CItemObject::ItemInfo ItemInfo{};
            ItemInfo.block = res.block;
            ItemInfo.iCnt = 1;

          

            DestroyStageEndItemConverter(ItemInfo);
            

        
            

            auto pos = _float3{ (float)res.iWorldBlockX, (float)res.iWorldBlockY,  (float)res.iWorldBlockZ };
            pos.x += 0.5f;
            pos.y += 0.5f;
            pos.z += 0.5f;

            SpawnDropItemObject(ItemInfo, pos, { 0.f, 2.f, 0.f });
        }
    }
    else
    {
        pDestroyStage->SetRender(false);
        m_bDestoryStageStart = false;
    }
}

void CPlayerEntity::DestroyStageEndItemConverter(CItemObject::ItemInfo& info)
{
    switch (info.block->GetType())
    {
    case CBlock3::TYPE::GRASS:
        info.block->SetType(CBlock3::TYPE::DIRT);
        return;
    case CBlock3::TYPE::STONE:
        info.block->SetType(CBlock3::TYPE::COBBLESTONE);
        return;
    case CBlock3::TYPE::STONE_COAL_ORE:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Coal;
        return;
    case CBlock3::TYPE::TORCH_ON:
        info.block = std::nullopt;
        info.eItemType = CItemObject::ITEM_TYPE::ITEM_Torch;
        return;
    }
}

CDestroyStage* CPlayerEntity::GetDestroyStage() const
{
    return CGameInstance::Get().GetGameObjectByHandleT<CDestroyStage>(m_hDestroyStage);
}

void CPlayerEntity::ProcessRightClick(float fTimeDelta)
{
    if (!m_pActivePlayerCamera) return;
    
    if (m_bMouseDownRight)
    {
        const auto& [rayOrigin2, rayDir2] = m_pActivePlayerCamera->GetRay();
        CVoxelManager3::BLOCK_RAY_RESULT res;
        if (CGameInstance::Get().VoxelBlockRaycast(rayOrigin2, rayDir2, 5.f, res))
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
                    GetUIController()->Getinventory()->SetRender(false);
                    GetUIController()->GetChest()->SetRender(true);
                    return;
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

            auto currHotbarIdx = GetUIController()->GetHotBar()->GetHotBarSelect()->GetSelectIdx();
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
            auto pFurnaceStorage = CGameInstance::Get().GetGameObjectByHandleT<CFurnaceStorage>(m_hFurnaceStorage);
            auto pStorage = pFurnaceStorage->GetStorage(m_openFurnaceLocation.value());
            pUIController->GetBlastFurnace()->SetIngredientItemData(&pStorage->ingredient, 1);
            pUIController->GetBlastFurnace()->SetFuelItemData(&pStorage->fuel, 1);
            pUIController->GetBlastFurnace()->SetResultItemData(&pStorage->result, 1);
            pUIController->GetBlastFurnace()->SetProcess(pStorage->progress);
            pUIController->GetBlastFurnace()->SetFuelProgress(pStorage->fuelProgress);
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
                    _float3 startPos{};
                    XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                    startPos.y += 1.f;

                    SpawnDropItemObject(newInfo, startPos, { 0.f, 2.f, 0.f });
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
                    _float3 startPos{};
                    XMStoreFloat3(&startPos, GetTransform().GetState(STATE::POSITION) + m_pActivePlayerCamera->GetTransform().GetState(STATE::LOOK) * 2.f);
                    startPos.y += 1.f;

                    SpawnDropItemObject(newInfo, startPos, { 0.f, 2.f, 0.f });
                }
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
                    auto tmp = tmpCamera->GetTransform().GetLoadedCombinedWorldMatrix();
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



        // camera control
        {
            PlayerCameraTrace(fTimeDelta);
        }
    }
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
                    auto pHint = static_cast<CDropBlock::CollHint*>(pColl->GetInnerHint2());
                    const auto& itemInfo = pHint->iter->itemInfo;
                    auto* pUIController = GetUIController();

                    //if (SUCCEEDED(pUIController->Getinventory()->AddItemToInventory(itemInfo)))
                    //{
                    //    pObj->GetDropItemObjects().erase(pHint->iter);
                    //}

                    if (SUCCEEDED(ProcessItemGain(itemInfo)))
                    {
                        pObj->GetDropItemObjects().erase(pHint->iter);
                    }
                }
            }
        }
    }
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


void CPlayerEntity::ReadyPlayerItem()
{
    CItemObject::ItemInfo info{};
    info.eItemType = CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;
    m_ItemArrHotbar[0] = info;

    CItemObject::ItemInfo stick{};
    stick.eItemType = CItemObject::ITEM_TYPE::ITEM_Stick;
    stick.iCnt = 1;
    m_ItemArrHotbar[1] = stick;

    CItemObject::ItemInfo coal{};
    coal.eItemType = CItemObject::ITEM_TYPE::ITEM_Coal;
    coal.iCnt = 1;
    m_ItemArrHotbar[2] = coal;


    CItemObject::ItemInfo craftingtable{};
    craftingtable.block = CBlock3(CBlock3::TYPE::CRAFTING_TABLE);
    craftingtable.iCnt = 1;
    m_ItemArrHotbar[3] = craftingtable;

    CItemObject::ItemInfo chest{};
    chest.block = CBlock3(CBlock3::TYPE::CHEST);
    chest.iCnt = 1;
    m_ItemArrHotbar[4] = chest;

    CItemObject::ItemInfo furnace{};
    furnace.block = CBlock3(CBlock3::TYPE::FURNACE);
    furnace.iCnt = 1;
    m_ItemArrHotbar[5] = furnace;
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
                SpawnDropItemObject(*pInfo, startPos, { 0.f, 2.f, 0.f });
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
                SpawnDropItemObject(*pInfo, startPos, { 0.f, 2.f, 0.f });
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
                SpawnDropItemObject(*pInfo, startPos, { 0.f, 2.f, 0.f });
            }
            pObj->SetPendingDestroyCascade();
        }
        return;
    }
    if (!m_openFurnaceLocation) return;

    auto invenSlotToMemberItem = [&](CUIBlastFurnace::FurnaceSlot slot)->std::optional<CItemObject::ItemInfo>*
        {

            auto pFurnaceStorage = CGameInstance::Get().GetGameObjectByHandleT<CFurnaceStorage>(m_hFurnaceStorage);
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

void CPlayerEntity::SpawnDropItemObject(const CItemObject::ItemInfo& info, _float3 pos, _float3 vel)
{
    if (info.block)
    {
        E::CDropItemObject::DESC Desc{};
        Desc.sObjectTag = "CDropBlock_Cube";
        Desc.viBufferId = { "MC_ITEM_VIBuffer", CDropItemObject::GetVIBufferName(info) };

        if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropBlock", &Desc))
        {
            if (!pLayer->empty())
            {
                if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropBlock>(pLayer->front()))
                {
                    auto type = info.block->GetType();

                    std::vector<uint32_t> texs{};
                    for (uint32_t i = 0; i < ETOUI(FACE_DIR::END); ++i)
                    {
                        texs.push_back(PackTexId(9, ETOUI(CBlock3::GetTexType(type, static_cast<FACE_DIR>(i)))));
                    }

                    pObj->AddDropItemObject(info, pos, vel, texs);
                }
            }
        }
    }
    else
    {
        {
            E::CDropItemObject::DESC Desc{};
            Desc.sObjectTag = "CDropItem";
            Desc.viBufferId = { "MC_ITEM_VIBuffer",  CDropItemObject::GetVIBufferName(info) };

            if (auto pLayer = CGameInstance::Get().GetGameObjectLayer(CDropItemObject::GetDropItemLayer(info), "ITEM", "Prototype_GameObject_DropItem", &Desc))
            {
                if (!pLayer->empty())
                {
                    if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CDropItemObject>(pLayer->front()))
                    {
                        pObj->AddDropItemObject(info, pos, vel, { CItemObject::GetPackedTexIdByType(info.eItemType) });
                    }
                }
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
