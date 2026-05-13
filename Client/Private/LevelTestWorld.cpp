#include "LevelTestWorld.h"
#include "GameInstance.h"

#include "Level_Defines.h"
#include "CameraObject.h"

#include "PigEntity.h"
#include "CowEntity.h"
#include "ChickenEntity.h"
#include "PlayerSteveEntity.h"
#include "SkeletonEntity.h"
#include "DropItem.h"
#include "ExperienceOrbItem.h"
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
	E::CGameInstance::Get().GameObjectLayerInitialize(E::ETOUI(LEVEL_TEST_WORLD_LAYERS::END), LevelTestWorldLayersToString);


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
			E::ETOUI(LEVEL_TEST_WORLD_LAYERS::CAMERA), &Desc))
		{
			if (FAILED(E::CGameInstance::Get().RegistGameCamera("FLY", flyCam.value())))
			{
				int x = 0;
			}
			E::CGameInstance::Get().SetActiveGameCamera("FLY");
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
		if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_PlayerSteveEntity", E::CPlayerSteveEntity::Create())))
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

		if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_ExperienceOrb", E::CExperienceOrbItem::Create())))
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
		//{
		//	E::CPlayerSteveEntity::DESC Desc{};
		//	Desc.sObjectTag = "PlayerSteve";
		//	if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PlayerSteveEntity",
		//		E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
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
				E::ETOUI(LEVEL_TEST_WORLD_LAYERS::DROPITEM), &Desc))
			{
			}
		}

		{
			E::CDropItem::DESC Desc{};
			Desc.sObjectTag = "DropItem_WoodPickaxe";
			Desc.viBufferId = { "MC_ITEM_VIBuffer", "WoodPickaxe" };
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_DropItem",
				E::ETOUI(LEVEL_TEST_WORLD_LAYERS::DROPITEM), &Desc))
			{
			}
		}

		{
			E::CExperienceOrbItem::DESC Desc{};
			Desc.sObjectTag = "ExperienceOrb";
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ITEM", "Prototype_GameObject_ExperienceOrb",
				E::ETOUI(LEVEL_TEST_WORLD_LAYERS::DROPITEM), &Desc))
			{
			}
		}

	}
	//if (auto flyCam = E::CGameInstance::Get().AddGameObjectToLayer("CAMERAS", "Prototype_GameObject_FlyCamera",
	//	E::ETOUI(LEVEL_TEST_WORLD_LAYERS::CAMERA), &Desc))
	//{
	//	
	//}

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
