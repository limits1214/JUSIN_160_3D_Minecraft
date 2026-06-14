#include "LevelOverWorld.h"
#include "GameInstance.h"

#include "CameraObject.h"

#include "PigEntity.h"
#include "CowEntity.h"
#include "ChickenEntity.h"
#include "PlayerEntity.h"
#include "SkeletonEntity.h"
#include "DropItem.h"
#include "ExperienceOrb.h"
#include "UICrosshair.h"
#include "UICamera.h"

#include "BlockOutline.h"

#include "PlayerFPSArm.h"
#include "DestroyStage.h"
#include "UIHotbar.h"
#include "UIHotbarSelect.h"
#include "UIHealthBar.h"
#include "UIHealthBarIcon.h"
#include "UIExperienceBar.h"
#include "UIExperienceBarGage.h"
#include "UIArmorBar.h"
#include "UIArmorBarIcon.h"
#include "UIHungerBar.h"
#include "UIHungerBarIcon.h"
#include "UIBreathBar.h"
#include "UIBreathBarIcon.h"
#include "UIController.h"

#include "FallingVoxel.h"
#include "ChestStorage.h"

#include "UIItem.h"

#include "Skybox.h"

//#include "HandHeldItemObject.h"
#include "HandHeldItem.h"

NS_USING(Client)

CLevelOverWorld::CLevelOverWorld()
{
}


CLevelOverWorld::~CLevelOverWorld()
{
}

HRESULT CLevelOverWorld::Initialize()
{
	E::CGameInstance::Get().GameObjectAllReset();

	E::CGameInstance::Get().GameObjectAllReset();

	{
		E::CCameraObject::CAMERA_DESC Desc{};
		Desc.eProj = E::CCameraObject::PROJ::PERSPECTIVE;
		Desc.vAt = { 0.f, 0.f, 0.f };
		Desc.vEye = { 0.f, 10.f, -10.f };
		Desc.fAspect = { g_iWinSizeX / (E::_float)g_iWinSizeY };
		Desc.fFovY = 75.f;
		Desc.fNear = 0.1f;
		Desc.fFar = 1000.f;
		Desc.sObjectTag = "FlyCam";

		if (auto flyCam = E::CGameInstance::Get().AddGameObjectToLayer("CAMERAS", "Prototype_GameObject_FlyCamera",
			"99_CAMERA", &Desc))
		{
			if (FAILED(E::CGameInstance::Get().RegistGameCamera("FLY", flyCam.value())))
			{
				int x = 0;
			}
			E::CGameInstance::Get().SetActiveGameCamera("FLY");
		}
	}

	{
		E::CCameraObject::CAMERA_DESC Desc{};
		Desc.eProj = E::CCameraObject::PROJ::PERSPECTIVE;
		Desc.vAt = { 0.f, 0.f, 0.f };
		Desc.vEye = { 0.f, 10.f, -10.f };
		Desc.fAspect = { g_iWinSizeX / (E::_float)g_iWinSizeY };
		Desc.fFovY = 75.f;
		Desc.fNear = 0.1f;
		Desc.fFar = 1000.f;
		Desc.sObjectTag = "PlayerCam";

		if (auto flyCam = E::CGameInstance::Get().AddGameObjectToLayer("CAMERAS", "Prototype_GameObject_PlayerCamera",
			"99_CAMERA", &Desc))
		{
			if (FAILED(E::CGameInstance::Get().RegistGameCamera("Player", flyCam.value())))
			{
				int x = 0;
			}
		}
	}




	{

		E::CPlayerEntity::DESC Desc{};
		Desc.sObjectTag = "Player";
		if (auto playerHandle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PlayerEntity",
			"01_ENTITY", &Desc))
		{
			if (auto playerObj = E::CGameInstance::Get().GetGameObjectByHandleT<E::CPlayerEntity>(playerHandle.value()))
			{
				


				


				{
					// destroy stage
					{
						//"DESTROY_STAGE", "Prototype_GameObject_DestroyStage"
						E::CGameObject::GAMEOBJECT_DESC Desc{};
						Desc.sObjectTag = "DestroyStage";

						if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("DESTROY_STAGE", "Prototype_GameObject_DestroyStage",
							"82_DestroyStage", &Desc))
						{
							playerObj->SetDestroyState(handle.value());
						}
					}
				}


				{
					E::CUIController::DESC ControllerDesc{};
					ControllerDesc.sObjectTag = "UIController";
					ControllerDesc.HotBar = E::CUIController::DELEGATE_UI_DESC{
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHotbar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.HotBarSelect = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHotbarSelect"},
						.LayerID = "80_UI",
					};

					ControllerDesc.HealthBar = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHealthBar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.HealthBarIcon = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHealthBarIcon"},
						.LayerID = "80_UIHealthIcon",
					};

					ControllerDesc.ArmorBar = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIArmorBar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.ArmorBarIcon = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIArmorBarIcon"},
						.LayerID = "80_UIHealthIcon",
					};

					ControllerDesc.HungerBar = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHungerBar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.HungerBarIcon = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIHungerBarIcon"},
						.LayerID = "80_UIHungerIcon",
					};

					ControllerDesc.BreathBar = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIBreathBar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.BreathBarIcon = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIBreathBarIcon"},
						.LayerID = "80_UIBreathIcon",
					};


					ControllerDesc.ExperienceBar = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIExperienceBar"},
						.LayerID = "80_UI",
					};
					ControllerDesc.ExperienceBarGage = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIExperienceBarGage"},
						.LayerID = "80_UIArmorIcon",
					};

					ControllerDesc.Crosshair = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UICrosshair"},
						.LayerID = "80_CrossHair",
					};

					ControllerDesc.Inventory = {
						.ProtoPairID = { "UI", "Prototype_GameObject_UIInventory" },
						.LayerID = "80_UI"
					};

					ControllerDesc.CraftingTable = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UICraftingTable"},
						.LayerID = "80_UI"
					};

					ControllerDesc.BlastFurnace = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIBlastFurnace"},
						.LayerID = "80_UI"
					};

					ControllerDesc.EnchantingTable = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIEnchantingTable"},
						.LayerID = "80_UI"
					};

					ControllerDesc.Chest = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIChest"},
						.LayerID = "80_UI"
					};

					ControllerDesc.Chest2 = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UIChest2"},
						.LayerID = "80_UI"
					};

					ControllerDesc.TextBg = {
						.ProtoPairID = {"UI", "Prototype_GameObject_UITextBg"},
						.LayerID = "80_UI"
					};

					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIController",
						"79_UIController", &ControllerDesc))
					{
						playerObj->SetUIController(handle.value());
					}
				}
			}
		}
	}

	//{
	//	E::CUIItem::DESC Desc{};
	//	Desc.sObjectTag = "UIItem";
	//	if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIItem",
	//		"80_UI", &Desc))
	//	{
	//	}
	//}

		//{
		//	E::CDropItem::DESC Desc{};
		//	Desc.sObjectTag = "DropItem_CubeItemDirt";
		//	Desc.viBufferId = { "MC_ITEM_VIBuffer", "CubeItemDirt" };
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
		//		"01_DROPITEM", &Desc))
		//	{
		//	}
		//}

		//{
		//	E::CExperienceOrb::DESC Desc{};
		//	Desc.sObjectTag = "ExperienceOrb";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_ExperienceOrb",
		//		"01_DROPITEM", &Desc))
		//	{
		//	}
		//}



	// handheld
	if(false)
	{
		E::CItemObject::ItemInfo info{};
		info.eItemType = E::CItemObject::ITEM_TYPE::ITEM_WoodPickaxe;
		E::CHandHeldItemObject::DESC Desc{};
		Desc.sObjectTag = "HandHeldItem";
		Desc.viBufferId = { "MC_ITEM_VIBuffer", E::CDropItemObject::GetVIBufferName(info) };
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_HandHeldItem",
			"33_HandHeld", &Desc))
		{
			if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandleT<E::CHandHeldItem>(handle.value()))
			{
				
				pObj->SetItemInfo(info);
			}
			int x = 0;
		}
	}


	// falling voxel
	{
		//CFallingVoxel
		{
			E::CFallingVoxel::DESC Desc{};
			Desc.sObjectTag = "CFallingVoxelDirt";

			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("FALLING_VOXEL", "Prototype_GameObject_FallingVoxel",
				"88_FALLING_VOXEL", &Desc))
			{
				int x = 0;
			}
		}

		//E::CGameInstance::Get().getga
	}

	//block outline
	{
		//CBlockOutline
		E::CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.sObjectTag = "BlockOutline";

		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("BLOCK_OUTLINE", "Prototype_GameObject_BlockOutline",
			"81_BlockOutline", &Desc))
		{
		}
	}

	
	//skybox
	{
		E::CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.sObjectTag = "Skybox";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("SKYBOX", "Prototype_GameObject_Skybox",
			"00_Skybox", &Desc))
		{
			int x = 0;
		}
	}

	// cloud
	//if(false)
	{
		E::CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.sObjectTag = "Cloud";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("CLOUD", "Prototype_GameObject_Cloud",
			"00_Cloud", &Desc))
		{
			int x = 0;
		}
	}



	{
		E::CCameraObject::CAMERA_DESC Desc{};
		Desc.eProj = E::CCameraObject::PROJ::ORTHOGRAPHIC;
		Desc.fNear = 0.f;
		Desc.fFar = 1.f;
		Desc.fWidth = g_iWinSizeX;
		Desc.fHeight = g_iWinSizeY;
		Desc.sObjectTag = "UICam";
		Desc.vEye = { 0.f, 0.f, -0.1f };

		if (auto uiCam = E::CGameInstance::Get().AddGameObjectToLayer("CAMERAS", "Prototype_GameObject_UICamera",
			"99_CAMERA", &Desc))
		{
			if (FAILED(E::CGameInstance::Get().RegistUICamera("UI", uiCam.value())))
			{
				int x = 0;
			}
			E::CGameInstance::Get().SetActiveUICamera("UI");
		}
	}
    return S_OK;
}

void CLevelOverWorld::Update(E::_float fTimeDelta)
{
}

HRESULT CLevelOverWorld::Render()
{
    return S_OK;
}

void CLevelOverWorld::UpdateGUI()
{
}

void CLevelOverWorld::FrameStart(E::_float fTimeDelta)
{
}

Engine::UPtr<CLevelOverWorld> CLevelOverWorld::Create()
{
	auto	pInstance = E::ToUPtr(new CLevelOverWorld{});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevelOverWorld");
	}

	return pInstance;
}