#include "UIController.h"
#include "GameInstance.h"

#include "UIHotBar.h"
#include "UIHealthBar.h"
#include "UIHungerBar.h"
#include "UIArmorBar.h"
#include "UIBreathBar.h"
#include "UIExperienceBar.h"
#include "UICrosshair.h"

NS_USING(Engine)

CUIController::CUIController()
{

}

CUIController::~CUIController()
{
}

void CUIController::UpdateGUI()
{
}

CUIHotBar* CUIController::GetHotBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIHotBar>(m_hHotBar);
}

CUIHealthBar* CUIController::GetHealthBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIHealthBar>(m_hHealthBar);
}

CUIHungerBar* CUIController::GetHungerBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIHungerBar>(m_hHungerBar);
}

CUIArmorBar* CUIController::GetArmorBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIArmorBar>(m_hArmorBar);
}

CUIBreathBar* CUIController::GetBreathBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIBreathBar>(m_hBreathBar);
}

CUIExperienceBar* CUIController::GetExperienceBar() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIExperienceBar>(m_hExperienceBar);
}

CUICrosshair* CUIController::GetCrosshair() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUICrosshair>(m_hCrosshair);
}

CUIInventory* CUIController::Getinventory() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIInventory>(m_hInventory);
}

CUICraftingTable* CUIController::GetCraftingTable() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUICraftingTable>(m_hCraftingTable);
}

CUIBlastFurnace* CUIController::GetBlastFurnace() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIBlastFurnace>(m_hBlastFurnace);
}

CUIEnchantingTable* CUIController::GetEnchantingTable() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIEnchantingTable>(m_hEnchantingTable);
}

CUIChest* CUIController::GetChest() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIChest>(m_hChest);
}

CUIChest2* CUIController::GetChest2() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUIChest2>(m_hChest2);
}

CUITextBg* CUIController::GetTextBg() const
{
	return CGameInstance::Get().GetGameObjectByHandleT<CUITextBg>(m_hTextBg);
}

HRESULT CUIController::Initialize(void* pArg)
{
	auto* pDesc = static_cast<CUIController::DESC*>(pArg);

	if (FAILED(CUIObject::Initialize(pArg)))
		return E_FAIL;
	
	{
		{
			auto protoID = pDesc->HotBar.ProtoPairID;
			auto layerID = pDesc->HotBar.LayerID;
			E::CUIObject::UIOBJECT_DESC Desc{};
			Desc.sObjectTag = "Hotbar";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
				layerID, &Desc))
			{
				m_hHotBar = handle.value();

				if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandleT<CUIHotBar>(handle.value()))
				{
					auto protoID = pDesc->HotBarSelect.ProtoPairID;
					auto layerID = pDesc->HotBarSelect.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "HotBarSelect";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
					{
						pBarObj->SetHotBarSelect(handle.value());

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
		auto protoID = pDesc->HealthBar.ProtoPairID;
		auto layerID = pDesc->HealthBar.LayerID;
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "HealthBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hHealthBar = handle.value();
			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					auto protoID = pDesc->HealthBarIcon.ProtoPairID;
					auto layerID = pDesc->HealthBarIcon.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "HealthIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
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
		auto protoID = pDesc->HungerBar.ProtoPairID;
		auto layerID = pDesc->HungerBar.LayerID;
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "HungerBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hHungerBar = handle.value();

			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					auto protoID = pDesc->HungerBarIcon.ProtoPairID;
					auto layerID = pDesc->HungerBarIcon.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "HungerBarIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
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
		auto protoID = pDesc->BreathBar.ProtoPairID;
		auto layerID = pDesc->BreathBar.LayerID;
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "BreathBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hBreathBar = handle.value();

			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					auto protoID = pDesc->BreathBarIcon.ProtoPairID;
					auto layerID = pDesc->BreathBarIcon.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "BreathBarIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
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
		auto protoID = pDesc->ArmorBar.ProtoPairID;
		auto layerID = pDesc->ArmorBar.LayerID;
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "ArmorBar";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hArmorBar = handle.value();

			if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
			{
				for (uint32_t i = 0; i < 10; ++i)
				{
					auto protoID = pDesc->ArmorBarIcon.ProtoPairID;
					auto layerID = pDesc->ArmorBarIcon.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "ArmorBarIcon";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
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
		{
			auto protoID = pDesc->ExperienceBar.ProtoPairID;
			auto layerID = pDesc->ExperienceBar.LayerID;
			E::CUIObject::UIOBJECT_DESC Desc{};
			Desc.sObjectTag = "ExperienceBar";
			if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
				layerID, &Desc))
			{
				m_hExperienceBar = handle.value();

				if (auto pBarObj = E::CGameInstance::Get().GetGameObjectByHandle(handle.value()))
				{
					auto protoID = pDesc->ExperienceBarGage.ProtoPairID;
					auto layerID = pDesc->ExperienceBarGage.LayerID;
					E::CUIObject::UIOBJECT_DESC Desc{};
					Desc.sObjectTag = "ExperienceBarGage";
					if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
						layerID, &Desc))
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
		auto protoID = pDesc->Crosshair.ProtoPairID;
		auto layerID = pDesc->Crosshair.LayerID;
		
		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Crosshair";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hCrosshair = handle.value();
		}
	}

	{
		auto protoID = pDesc->Inventory.ProtoPairID;
		auto layerID = pDesc->Inventory.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Inventory";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hInventory = handle.value();
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIInventory>(m_hInventory))
			{
				pObj->SetUIController(GetHandle());
			}
		}
	}

	{
		auto protoID = pDesc->CraftingTable.ProtoPairID;
		auto layerID = pDesc->CraftingTable.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "CraftingTable";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hCraftingTable = handle.value();
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUICraftingTable>(m_hCraftingTable))
			{
				pObj->SetUIController(GetHandle());
			}
		}
	}

	{
		auto protoID = pDesc->BlastFurnace.ProtoPairID;
		auto layerID = pDesc->BlastFurnace.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "BlastFurnace";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hBlastFurnace = handle.value();
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIBlastFurnace>(m_hBlastFurnace))
			{
				pObj->SetUIController(GetHandle());
			}
		}
	}

	{
		auto protoID = pDesc->EnchantingTable.ProtoPairID;
		auto layerID = pDesc->EnchantingTable.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "EnchantingTable";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hEnchantingTable = handle.value();
		}
	}

	{
		auto protoID = pDesc->Chest.ProtoPairID;
		auto layerID = pDesc->Chest.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Chest";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hChest = handle.value();
			if (auto pObj = CGameInstance::Get().GetGameObjectByHandleT<CUIChest>(m_hChest))
			{
				pObj->SetUIController(GetHandle());
			}
		}
	}

	{
		auto protoID = pDesc->Chest2.ProtoPairID;
		auto layerID = pDesc->Chest2.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "Chest2";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hChest2 = handle.value();
		}
	}

	{
		auto protoID = pDesc->TextBg.ProtoPairID;
		auto layerID = pDesc->TextBg.LayerID;

		E::CUIObject::UIOBJECT_DESC Desc{};
		Desc.sObjectTag = "TextBg";
		if (auto handle = E::CGameInstance::Get().AddGameObjectToLayer(protoID.first, protoID.second,
			layerID, &Desc))
		{
			m_hTextBg = handle.value();
		}
	}
    return S_OK;
}

void CUIController::PriorityUpdate(E::_float fTimeDelta)
{
}

void CUIController::Update(E::_float fTimeDelta)
{
}

void CUIController::LateUpdate(E::_float fTimeDelta)
{
}

HRESULT CUIController::Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx)
{
    return S_OK;
}

E::UPtr<CUIController> CUIController::Create()
{
	auto pInstance = E::ToUPtr(new CUIController{});
	if (FAILED(pInstance->InitializePrototype()))
	{
		MSG_BOX("Failed to Created : CUIController");
		return nullptr;
	}
	return  pInstance;
}


E::UPtr<E::CPrototype> CUIController::Clone(void* pArg)
{
	auto	pInstance = E::ToUPtr(new CUIController{ *this });
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CUIController");
		return nullptr;
	}

	return pInstance;
}

