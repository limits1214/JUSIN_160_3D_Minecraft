#include "LevelTestWorld.h"
#include "GameInstance.h"

#include "Level_Defines.h"
#include "CameraObject.h"

#include "PigEntity.h"
#include "CowEntity.h"

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
			if (FAILED(E::CGameInstance::Get().SetCameraObject("GAME", flyCam.value())))
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
	}

	{
		{
			E::CPigEntity::DESC Desc{};
			Desc.sObjectTag = "Pig";
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_PigEntity",
				E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
			{

			}
		}

		{
			E::CCowEntity::DESC Desc{};
			Desc.sObjectTag = "Cow";
			if (auto pig = E::CGameInstance::Get().AddGameObjectToLayer("ENTITY", "Prototype_GameObject_CowEntity",
				E::ETOUI(LEVEL_TEST_WORLD_LAYERS::ENTITY), &Desc))
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
