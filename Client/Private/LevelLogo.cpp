#include "pch.h"
#include "LevelLogo.h"
#include "GameInstance.h"
#include "LevelLoading.h"

#include "FlyCamera.h"

#include "ResCBuffer.h"
#include "BackGround.h"
#include "UiCamera.h"


NS_USING(Client)

CLevelLogo::CLevelLogo(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	: CLevel{ pDevice, pContext }
{
}

CLevelLogo::~CLevelLogo()
{
}

HRESULT CLevelLogo::Initialize()
{
	Engine::CGameInstance::Get().GameObjectAllReset();

	{
		CBackGround::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "BackGround";
		if (!(E::CGameInstance::Get().AddGameObjectToLayer("LEVEL_LOGO", "Prototype_GameObject_BackGround",
			"Layer_BackGround", &Desc)))
		{
			return E_FAIL;
		}
	}

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
			"Layer_Camera", &Desc))
		{
			if (FAILED(E::CGameInstance::Get().SetCameraObject("GAME", flyCam.value())))
			{
				int x = 0;
			}
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
			"Layer_Camera", &Desc))
		{
			if (FAILED(E::CGameInstance::Get().SetCameraObject("UI", uiCam.value())))
			{
				int x = 0;
			}



			const auto* t = E::CGameInstance::GetConst().GetGameObjectByHandleT<E::CUICamera>(uiCam.value());

			auto* t2 = E::CGameInstance::Get().GetGameObjectByHandleT<E::CUICamera>(uiCam.value());
		}


	}


	return S_OK;
}

void CLevelLogo::Update(E::_float fTimeDelta)
{
}

HRESULT CLevelLogo::Render()
{
	return S_OK;
}

void CLevelLogo::UpdateGUI()
{
	ImGui::Begin("LEVEL: CLevel_Logo");
	//if (ImGui::Button("ChangeLevelTo: GamePlay"))
	//{
	//	if (FAILED(Engine::CGameInstance::Get().ChangeLevel(CLevelLoading::Create(m_pDevice, m_pContext, LEVEL::GAMEPLAY))))
	//	{
	//		MSG_BOX("ChangeLevelTo: GamePlay Failed");
	//	}
	//}

	

	ImGui::End();
}

void CLevelLogo::FrameStart(E::_float fTimeDelta)
{

}

Engine::UPtr<CLevelLogo> CLevelLogo::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	auto	pInstance = Engine::UPtr<CLevelLogo>(new CLevelLogo(pDevice, pContext));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
	}

	return pInstance;
}

void CLevelLogo::Free()
{
	//E::CGameInstance::Get().GameObjectAllReset();
	CLevel::Free();
}
