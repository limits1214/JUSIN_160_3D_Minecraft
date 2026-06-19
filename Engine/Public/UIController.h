#pragma once

#pragma once
#include "UIObject.h"

#include "UIHotBar.h"
#include "UIInventory.h"

#include "UICraftingTable.h"
#include "UIBlastFurnace.h"
#include "UIEnchantingTable.h"
#include "UIChest.h"
#include "UIChest2.h"
#include "UITextBg.h"
#include "UIHealthBar.h"
#include "UIHungerBar.h"
#include "UIArmorBar.h"
#include "UIBreathBar.h"
#include "UIExperienceBar.h"

NS_BEGIN(Engine)

class CUICrosshair;

class ENGINE_DLL CUIController final : public E::CUIObject
{
public:
	typedef struct tagDelegateUIDesc
	{
		std::pair<StringID, StringID> ProtoPairID{};
		std::string_view LayerID{};
	} DELEGATE_UI_DESC;
	typedef struct tagDESC : public CUIObject::UIOBJECT_DESC
	{
		DELEGATE_UI_DESC HotBar{};
		DELEGATE_UI_DESC HotBarSelect{};

		DELEGATE_UI_DESC HealthBar{};
		DELEGATE_UI_DESC HealthBarIcon{};

		DELEGATE_UI_DESC ExperienceBar{};
		DELEGATE_UI_DESC ExperienceBarGage{};

		DELEGATE_UI_DESC ArmorBar{};
		DELEGATE_UI_DESC ArmorBarIcon{};

		DELEGATE_UI_DESC HungerBar{};
		DELEGATE_UI_DESC HungerBarIcon{};

		DELEGATE_UI_DESC BreathBar{};
		DELEGATE_UI_DESC BreathBarIcon{};

		DELEGATE_UI_DESC Crosshair{};

		DELEGATE_UI_DESC Inventory{};

		DELEGATE_UI_DESC CraftingTable{};
		DELEGATE_UI_DESC BlastFurnace{};
		DELEGATE_UI_DESC EnchantingTable{};

		DELEGATE_UI_DESC Chest{};
		DELEGATE_UI_DESC Chest2{};

		DELEGATE_UI_DESC TextBg{};
		
	}DESC;
public:
	DECLARE_DERIVED_TYPE(CUIController, CUIObject)

private:
	CUIController();
	~CUIController() override;

public:
	void UpdateGUI() override;

public:
	CUIHotBar* GetHotBar() const;
	CUIHealthBar* GetHealthBar() const;
	CUIHungerBar* GetHungerBar() const;
	CUIArmorBar* GetArmorBar() const;
	CUIBreathBar* GetBreathBar() const;
	CUIExperienceBar* GetExperienceBar() const;
	CUICrosshair* GetCrosshair() const;
	CUIInventory* Getinventory() const;
	CUICraftingTable* GetCraftingTable() const;
	CUIBlastFurnace* GetBlastFurnace() const;
	CUIEnchantingTable* GetEnchantingTable() const;
	CUIChest* GetChest() const;
	CUIChest2* GetChest2() const;
	CUITextBg* GetTextBg() const;

private:
	CHandle m_hHotBar{};
	CHandle m_hHealthBar{};
	CHandle m_hHungerBar{};
	CHandle m_hArmorBar{};
	CHandle m_hBreathBar{};
	CHandle m_hExperienceBar{};
	CHandle m_hCrosshair{};
	CHandle m_hInventory{};
	CHandle m_hCraftingTable{};
	CHandle m_hBlastFurnace{};
	CHandle m_hEnchantingTable{};
	CHandle m_hChest{};
	CHandle m_hChest2{};
	CHandle m_hTextBg{};

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;
	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	static E::UPtr<CUIController> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END
