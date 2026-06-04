#pragma once

#pragma once
#include "UIObject.h"

#include "UIHotBar.h"

NS_BEGIN(Engine)

//class CUIHotBar;
class CUIHealthBar;
class CUIHungerBar;
class CUIArmorBar;
class CUIBreathBar;
class CUIExperienceBar;
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

private:
	CHandle m_hHotBar{};
	CHandle m_hHealthBar{};
	CHandle m_hHungerBar{};
	CHandle m_hArmorBar{};
	CHandle m_hBreathBar{};
	CHandle m_hExperienceBar{};
	CHandle m_hCrosshair{};

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
