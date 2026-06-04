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

