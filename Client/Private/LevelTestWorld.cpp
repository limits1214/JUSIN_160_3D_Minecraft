#include "LevelTestWorld.h"
#include "GameInstance.h"

#include "Level_Defines.h"
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

#include "FallingVoxel.h"

NS_USING(Client)
CLevelTestWorld::CLevelTestWorld()
{
}

CLevelTestWorld::~CLevelTestWorld()
{
}

HRESULT CLevelTestWorld::Initialize()
{
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
		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_CowEntity", E::CCowEntity::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_PigEntity", E::CPigEntity::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_ChickenEntity", E::CChickenEntity::Create())))
		{
			int x = 0;
		}
		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_PlayerEntity", E::CPlayerEntity::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_SkeletonEntity", E::CSkeletonEntity::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_DropItem", E::CDropItem::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_ExperienceOrb", E::CExperienceOrb::Create())))
		{
			int x = 0;
		}
		
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UICrosshair", E::CUICrosshair::Create())))
		{
			int x = 0;
		}
		//UIHotbar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHotbar", E::CUIHotBar::Create())))
		{
			int x = 0;
		}
		//UIHotbarSelect
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHotbarSelect", E::CUIHotBarSelect::Create())))
		{
			int x = 0;
		}
		//UIHealthBar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHealthBar", E::CUIHealthBar::Create())))
		{
			int x = 0;
		}
		//UIHealthIcon
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHealthBarIcon", E::CUIHealthBarIcon::Create())))
		{
			int x = 0;
		}
		//UIExperienceBar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIExperienceBar", E::CUIExperienceBar::Create())))
		{
			int x = 0;
		}
		//UIExperienceBarGage
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIExperienceBarGage", E::CUIExperienceBarGage::Create())))
		{
			int x = 0;
		}
		//UIArmorBar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIArmorBar", E::CUIArmorBar::Create())))
		{
			int x = 0;
		}
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIArmorBarIcon", E::CUIArmorBarIcon::Create())))
		{
			int x = 0;
		}
		//UIHungerBar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHungerBar", E::CUIHungerBar::Create())))
		{
			int x = 0;
		}
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHungerBarIcon", E::CUIHungerBarIcon::Create())))
		{
			int x = 0;
		}
		//CUIBreathBar
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIBreathBar", E::CUIBreathBar::Create())))
		{
			int x = 0;
		}
		if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIBreathBarIcon", E::CUIBreathBarIcon::Create())))
		{
			int x = 0;
		}


		if (FAILED(E::CGameInstance::Get().AddPrototype("BLOCK_OUTLINE", "Prototype_GameObject_BlockOutline", E::CBlockOutline::Create())))
		{
			int x = 0;
		}

		//PlayerFPSArm
		if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_PlayerFPSArm", E::CPlayerFPSArm::Create())))
		{
			int x = 0;
		}

		if (FAILED(E::CGameInstance::Get().AddPrototype("DESTROY_STAGE", "Prototype_GameObject_DestroyStage", E::CDestroyStage::Create())))
		{
			int x = 0;
		}

		//CFallingVoxel
		if (FAILED(E::CGameInstance::Get().AddPrototype("FALLING_VOXEL", "Prototype_GameObject_FallingVoxel", E::CFallingVoxel::Create())))
		{
			int x = 0;
		}
	}

	{
		//{
		//	E::CPigEntity::DESC Desc{};
		//	Desc.sObjectTag = "Pig";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PigEntity",
		//		E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
		//	{
		//	}
		//}

		//{
		//	E::CCowEntity::DESC Desc{};
		//	Desc.sObjectTag = "Cow";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CowEntity",
		//		E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
		//	{
		//	}
		//}

		//{
		//	E::CChickenEntity::DESC Desc{};
		//	Desc.sObjectTag = "Chicken";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_ChickenEntity",
		//		E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
		//	{
		//	}
		//}
		
		{
			E::CPlayerEntity::DESC Desc{};
			Desc.sObjectTag = "Player";
			if (auto playerHandle = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PlayerEntity",
				"00_ENTITY", &Desc))
			{
				if (auto playerObj = E::CGameInstance::Get().GetGameObjectByHandleT<E::CPlayerEntity>(playerHandle.value()))
				{
					{
						E::CDropItem::DESC Desc{};
						Desc.sObjectTag = "DropItem_WoodPickaxe";
						Desc.viBufferId = { "MC_ITEM_VIBuffer", "WoodPickaxe" };
						if (auto woodPixaxeHandle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
							"01_DROPITEM", &Desc))
						{
							//playerObj->SetRightItemHandle(woodPixaxeHandle);
							if (auto woodPixaxeObj = E::CGameInstance::Get().GetGameObjectByHandle(woodPixaxeHandle.value()))
							{
								woodPixaxeObj->GetTransform().SetPosition(XMVectorSet(0.f, 0.2f, 0.4f, 1.f));
								woodPixaxeObj->GetTransform().SetRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), -90);
								//woodPixaxeObj->SetParentNode(playerObj);
							}
						}
					}


					{
						E::CPlayerFPSArm::DESC Desc{};
						Desc.sObjectTag = "PlayerFPSArm";
						if (auto fpsArmHandle = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_PlayerFPSArm",
							"10_PlayerArm", &Desc))
						{
							playerObj->SetRightItemHandle(fpsArmHandle);
							if (auto fpsArmObj = E::CGameInstance::Get().GetGameObjectByHandle(fpsArmHandle.value()))
							{
								fpsArmObj->GetTransform().SetPosition(XMVectorSet(0.1f, -0.15f, 0.05f, 1.f));
								//fpsArmObj->GetTransform().AddRotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), -90.f);
								//fpsArmObj->GetTransform().AddRotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), 30.f);

								fpsArmObj->GetTransform().AddQuaternion(XMQuaternionRotationRollPitchYaw(
									XMConvertToRadians(-90.f),
									XMConvertToRadians(30.f),
									XMConvertToRadians(0.f)
								));

								fpsArmObj->GetTransform().SetScale(XMVectorSet(0.3f, 0.3f, 0.3f, 1.f));
								//woodPixaxeObj->SetParentNode(playerObj);
							}
						}
					}
				}
			}
		}

		//{
		//	E::CDropItem::DESC Desc{};
		//	Desc.sObjectTag = "DropItem_WoodPickaxe";
		//	Desc.viBufferId = { "MC_ITEM_VIBuffer", "WoodPickaxe" };
		//	if (auto woodPixaxe = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
		//		"01_DROPITEM", &Desc))
		//	{
		//	}
		//}

		//{
		//	E::CSkeletonEntity::DESC Desc{};
		//	Desc.sObjectTag = "Skeleton";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_SkeletonEntity",
		//		E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
		//	{
		//	}
		//}


		{
			E::CDropItem::DESC Desc{};
			Desc.sObjectTag = "DropItem_CubeItemDirt";
			Desc.viBufferId = { "MC_ITEM_VIBuffer", "CubeItemDirt" };
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
				"01_DROPITEM", &Desc))
			{
			}
		}

		{
			E::CExperienceOrb::DESC Desc{};
			Desc.sObjectTag = "ExperienceOrb";
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_ExperienceOrb",
				"01_DROPITEM", &Desc))
			{
			}
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


	{
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Crosshair";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UICrosshair",
			"80_CrossHair", &Desc))
		{
		}

	}

	{
		//Prototype_GameObject_UIHotbar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Hotbar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHotbar",
			"80_UI", &Desc))
		{
		}
	}

	{
		//Prototype_GameObject_UIHotbarSelect
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "HotbarSelect";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHotbarSelect",
			"80_UI", &Desc))
		{
			
		}
	}

	{
		//Prototype_GameObject_UIHealthBar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "HealthBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHealthBar",
			"80_UI", &Desc))
		{
			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					//Prototype_GameObject_UIHealthIcon
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "HealthIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHealthBarIcon",
						"80_UIHealthIcon", &Desc))
					{
						if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
						{
							pObj->SetParentNode(pBarObj);
						}
					}
				}
			}
		}
	}

	{
		//Prototype_GameObject_UIHealthIcon
		//E::CUIObject::UIOBJECT_DESC Desc{};
		//Desc.sObjectTag = "HealthIcon";
		//if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHealthIcon",
		//	"80_UI", &Desc))
		//{
		//}
	}

	{
		//Prototype_GameObject_UIExperienceBar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "ExperienceBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIExperienceBar",
			"80_UI", &Desc))
		{
		}
	}
	{
		//Prototype_GameObject_UIExperienceBarGage
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "ExperienceBarGage";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIExperienceBarGage",
			"80_UI", &Desc))
		{
		}
	}
	{
		//Prototype_GameObject_UIArmorBar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "ArmorBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIArmorBar",
			"80_UI", &Desc))
		{
			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					//Prototype_GameObject_UIArmorBarICon
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "ArmorIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIArmorBarIcon",
						"80_UI", &Desc))
					{
						if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
						{
							pObj->SetParentNode(pBarObj);
						}
					}
				}
			}
		}
	}

	{
		//Prototype_GameObject_UIHungerBar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "HungerBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHungerBar",
			"80_UI", &Desc))
		{
			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					//Prototype_GameObject_UIHungerBarIcon
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "HungerIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIHungerBarIcon",
						"80_UI", &Desc))
					{
						if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
						{
							pObj->SetParentNode(pBarObj);
						}
					}
				}
			}
		}
	}



	{
		//Prototype_GameObject_UIBreathBar
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "BreathBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIBreathBar",
			"80_UI", &Desc))
		{
			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					//Prototype_GameObject_UIBreathBarIcon
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "BreathIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("UI", "Prototype_GameObject_UIBreathBarIcon",
						"80_UI", &Desc))
					{
						if (auto pObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
						{
							pObj->SetParentNode(pBarObj);
						}
					}
				}
			}
		}
	}

	// destroy stage
	{
		//"DESTROY_STAGE", "Prototype_GameObject_DestroyStage"
		E::CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.sObjectTag = "DestroyStage";

		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer("DESTROY_STAGE", "Prototype_GameObject_DestroyStage",
			"82_DestroyStage", &Desc))
		{
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

void CLevelTestWorld::Update(E::_float fTimeDelta)
{
}

HRESULT CLevelTestWorld::Render()
{
	return S_OK;
}

void CLevelTestWorld::UpdateGUI()
{
}

void CLevelTestWorld::FrameStart(E::_float fTimeDelta)
{
}

Engine::UPtr<CLevelTestWorld> CLevelTestWorld::Create()
{
	auto	pInstance = E::ToUPtr(new CLevelTestWorld{});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevelTestWorld");
	}

	return pInstance;
}

void CLevelTestWorld::Free()
{
	CLevel::Free();
}
