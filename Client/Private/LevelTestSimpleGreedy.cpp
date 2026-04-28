#include "LevelTestSimpleGreedy.h"
#include "GameInstance.h"

#include "FlyCamera.h"
#include "TestSimpleGreedyChunk.h"

NS_USING(Client)

CLevelTestSimpleGreedy::CLevelTestSimpleGreedy()
{

}

CLevelTestSimpleGreedy::~CLevelTestSimpleGreedy()
{
}

HRESULT CLevelTestSimpleGreedy::Initialize()
{
	E::CGameInstance::Get().GameObjectAllReset();
	E::CGameInstance::Get().GameObjectLayerInitialize(E::ETOUI(LEVEL_TEST_SIMPLE_GREEDY_LAYERS::END), LevelTestSimpleGreedyLayersToString);

	{
		E::CCameraObject::CAMERA_DESC Desc{};
		Desc.eProj = E::CCameraObject::PROJ::PERSPECTIVE;
		Desc.vAt = { 0.f, 0.f, 0.f };
		Desc.vEye = { 0.f, 0.f, -5.f };
		Desc.fAspect = { g_iWinSizeX / (E::_float)g_iWinSizeY };
		Desc.fFovY = 75.f;
		Desc.fNear = 0.1f;
		Desc.fFar = 100.f;
		Desc.sObjectTag = "FlyCam";

		if (auto flyCam = E::CGameInstance::Get().AddGameObjectToLayer("CAMERAS", "Prototype_GameObject_FlyCamera",
			E::ETOUI(LEVEL_TEST_SIMPLE_GREEDY_LAYERS::CAMERA), &Desc))
		{
			if (FAILED(E::CGameInstance::Get().SetCameraObject("GAME", flyCam.value())))
			{
				int x = 0;
			}
		}
	}

	if (FAILED(E::CGameInstance::Get()
		.AddPrototype("LEVEL_TEST_SIMPLE_GREEDY", "Prototype_GameObject_TestSImpleGreedyChunk",
			CTestSimpleGreedyChunk::Create())))
	{
	}

	{
		E::CGameObject::GAMEOBJECT_DESC Desc{};
		Desc.sObjectTag = "Chunk";
		if (!(E::CGameInstance::Get().AddGameObjectToLayer("LEVEL_TEST_SIMPLE_GREEDY", "Prototype_GameObject_TestSImpleGreedyChunk",
			E::ETOUI(LEVEL_TEST_SIMPLE_GREEDY_LAYERS::OBJECTS), &Desc)))
		{
			return E_FAIL;
		}
	}
    return S_OK;
}

void CLevelTestSimpleGreedy::Update(E::_float fTimeDelta)
{
}

HRESULT CLevelTestSimpleGreedy::Render()
{
    return S_OK;
}

void CLevelTestSimpleGreedy::UpdateGUI()
{
}

void CLevelTestSimpleGreedy::FrameStart(E::_float fTimeDelta)
{
}

Engine::UPtr<CLevelTestSimpleGreedy> CLevelTestSimpleGreedy::Create()
{
	auto	pInstance = E::ToUPtr(new CLevelTestSimpleGreedy{});

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevelTestSimpleGreedy");
	}

	return pInstance;
}

void CLevelTestSimpleGreedy::Free()
{
	E::CGameInstance::Get().DelPrototype("LEVEL_TEST_SIMPLE_GREEDY");
	E::CGameInstance::Get().DelResource("LEVEL_TEST_SIMPLE_GREEDY");
	CLevel::Free();
}
