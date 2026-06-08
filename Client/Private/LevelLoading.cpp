#include "pch.h"
#include "LevelLoading.h"
#include "GameInstance.h"
#include "LevelLogo.h"
#include "Resources.h"
#include "BackGround.h"

#include "LevelTestSimpleGreedy.h"
#include "LevelTestWorld.h"
#include "LevelOverWorld.h"
NS_USING(Client)

CLevelLoading::CLevelLoading(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex) noexcept
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
	, m_eNextLevelIndex(eNextLevelIndex)
{
}

CLevelLoading::~CLevelLoading()
{
}

HRESULT CLevelLoading::Initialize()
{
	Engine::CGameInstance::Get().GameObjectAllReset();


	return S_OK;
}

void CLevelLoading::Update(E::_float fTimeDelta)
{
	if (!m_bThreadStart)
	{
		m_bThreadStart = true;

		ThreadStart();
	}

	LoadingCheck();

}

HRESULT CLevelLoading::Render()
{
	return S_OK;
}

void CLevelLoading::UpdateGUI()
{
	ImGui::Begin("LEVEL: CLevelLoading");
	ImGui::End();
}

void CLevelLoading::FrameEnd(E::_float fTimeDelta)
{
	if (m_bLoadEnd)
	{
		LoadEnd();
		return;
	}
}

HRESULT CLevelLoading::LoadEnd()
{
	Engine::UPtr<CLevel>	pNewLevel{};
	switch (m_eNextLevelIndex)
	{
	case LEVEL::LOGO:
		pNewLevel = CLevelLogo::Create();
		break;
	case LEVEL::TEST_SIMPLE_GREEDY:
		pNewLevel = CLevelTestSimpleGreedy::Create();
		break;
	case LEVEL::TEST_WORLD:
		pNewLevel = CLevelTestWorld::Create();
		break;
	case LEVEL::THE_OVERWORLD:
		pNewLevel = CLevelOverWorld::Create();
		break;
	}
	assert(pNewLevel);

	if (FAILED(Engine::CGameInstance::Get().ChangeLevel(std::move(pNewLevel))))
	{
		MSG_BOX("ChangeLevelFailed in loading");
		return E_FAIL;
	}
	return S_OK;
}

void CLevelLoading::ThreadStart()
{
	switch (m_eNextLevelIndex)
	{
	case LEVEL::LOGO:
	{
		m_futLoadFinish = E::CGameInstance::Get().WorkerEnqueueWithFuture("LOADING_LOGO", [this]()
			{
				if (FAILED(E::CGameInstance::Get().AddPrototype("LEVEL_LOGO", "Prototype_GameObject_BackGround", CBackGround::Create())))
				{
					return false;
				}

				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				return  true;
			});

		if (auto res = E::CGameInstance::Get().AddResource("LEVEL_LOGO", "TEX_SHM", E::CResTexture2D::Create("./Resources/Texture/SHM.png")))
		{
			res->Load();
		}

	}
	break;
	case LEVEL::THE_OVERWORLD:
	{
		m_futLoadFinish = E::CGameInstance::Get().WorkerEnqueueWithFuture("LOADING_OVERWORLD", [this]()
			{
				if (FAILED(LoadingOverWorldLevel()))
				{
					return false;
				}

				return  true;
			});
	}
	break;

	default:
		m_bLoadEnd = true;
		break;
	}

}

void CLevelLoading::LoadingCheck()
{
	if (m_futLoadFinish.valid())
	{
		if (m_futLoadFinish.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
		{
			m_bLoadEnd = m_futLoadFinish.get();

			if (!m_bLoadEnd)
			{
				MSG_BOX("LOADING FAILD");
			}
		}
	}
}

#include "CowEntity.h"
#include "PigEntity.h"
#include "ChickenEntity.h"
#include "PlayerEntity.h"
#include "SkeletonEntity.h"
#include "DropItem.h"
#include "DropBlock.h"
#include "ExperienceOrbItem.h"
#include "UICrossHair.h"
#include "UIHotBar.h"
#include "UIHotBarSelect.h"
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
#include "UIInventory.h"
#include "UICraftingTable.h"
#include "UIBlastFurnace.h"
#include "UIEnchantingTable.h"
#include "UIChest.h"
#include "UIChest2.h"

#include "UIItem.h"

#include "BlockOutline.h"
#include "PlayerFPSArm.h"
#include "DestroyStage.h"
#include "FallingVoxel.h"

HRESULT CLevelLoading::LoadingOverWorldLevel()
{
	if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_CowEntity", E::CCowEntity::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_PigEntity", E::CPigEntity::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_ChickenEntity", E::CChickenEntity::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_PlayerEntity", E::CPlayerEntity::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("ENTITY", "Prototype_GameObject_SkeletonEntity", E::CSkeletonEntity::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_DropItem", E::CDropItem::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_DropBlock", E::CDropBlock::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_ExperienceOrb", E::CExperienceOrbItem::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UICrosshair", E::CUICrosshair::Create())))
	{
		return E_FAIL;
	}
	//UIHotbar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHotbar", E::CUIHotBar::Create())))
	{
		return E_FAIL;
	}
	//UIHotbarSelect
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHotbarSelect", E::CUIHotBarSelect::Create())))
	{
		return E_FAIL;
	}
	//UIHealthBar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHealthBar", E::CUIHealthBar::Create())))
	{
		return E_FAIL;
	}
	//UIHealthIcon
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHealthBarIcon", E::CUIHealthBarIcon::Create())))
	{
		return E_FAIL;
	}
	//UIExperienceBar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIExperienceBar", E::CUIExperienceBar::Create())))
	{
		return E_FAIL;
	}
	//UIExperienceBarGage
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIExperienceBarGage", E::CUIExperienceBarGage::Create())))
	{
		return E_FAIL;
	}
	//UIArmorBar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIArmorBar", E::CUIArmorBar::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIArmorBarIcon", E::CUIArmorBarIcon::Create())))
	{
		return E_FAIL;
	}
	//UIHungerBar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHungerBar", E::CUIHungerBar::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIHungerBarIcon", E::CUIHungerBarIcon::Create())))
	{
		return E_FAIL;
	}
	//CUIBreathBar
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIBreathBar", E::CUIBreathBar::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIBreathBarIcon", E::CUIBreathBarIcon::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIController", E::CUIController::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIInventory", E::CUIInventory::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UICraftingTable", E::CUICraftingTable::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIBlastFurnace", E::CUIBlastFurnace::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIEnchantingTable", E::CUIEnchantingTable::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIChest", E::CUIChest::Create())))
	{
		return E_FAIL;
	}
	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIChest2", E::CUIChest2::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("UI", "Prototype_GameObject_UIItem", E::CUIItem::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("BLOCK_OUTLINE", "Prototype_GameObject_BlockOutline", E::CBlockOutline::Create())))
	{
		return E_FAIL;
	}

	//PlayerFPSArm
	if (FAILED(E::CGameInstance::Get().AddPrototype("ITEM", "Prototype_GameObject_PlayerFPSArm", E::CPlayerFPSArm::Create())))
	{
		return E_FAIL;
	}

	if (FAILED(E::CGameInstance::Get().AddPrototype("DESTROY_STAGE", "Prototype_GameObject_DestroyStage", E::CDestroyStage::Create())))
	{
		return E_FAIL;
	}

	//CFallingVoxel
	if (FAILED(E::CGameInstance::Get().AddPrototype("FALLING_VOXEL", "Prototype_GameObject_FallingVoxel", E::CFallingVoxel::Create())))
	{
		return E_FAIL;
	}



	return S_OK;
}

Engine::UPtr<CLevelLoading> CLevelLoading::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, LEVEL eNextLevelIndex)
{
	auto	pInstance = Engine::UPtr<CLevelLoading>(new CLevelLoading(pDevice, pContext, eNextLevelIndex));

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevelLoading");
		return nullptr;
	}

	return pInstance;
}

void CLevelLoading::Free()
{
	//E::CGameInstance::Get().GameObjectAllReset();
	CLevel::Free();
}
