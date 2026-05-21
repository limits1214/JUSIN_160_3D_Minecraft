#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIHungerBarIcon final : public E::CUIObject
{
public:
	enum class HUNGER_ICON_BG_TYPE
	{
		NORMAL,
	};
	enum class HUNGER_ICON_TYPE
	{
		EMPTY,
		FULL,
		HALF,
	};
public:
	DECLARE_DERIVED_TYPE(CUIHungerBarIcon, CUIObject)

private:
	CUIHungerBarIcon();
	~CUIHungerBarIcon() override;

public:
	void UpdateGUI() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;
	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	void SetRender(_bool b) { m_bRender = b; }

private:
	_bool m_bRender{ true };

private:
	CComTransform* m_pComOverlayTransform{};
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	void SetIconType(HUNGER_ICON_TYPE eIconType) { m_eIconType = eIconType; };
	void SetIconBgType(HUNGER_ICON_BG_TYPE eIconBgType) { m_eIconBgType = eIconBgType; };

private:
	HUNGER_ICON_TYPE m_eIconType{ HUNGER_ICON_TYPE::EMPTY };
	HUNGER_ICON_BG_TYPE m_eIconBgType{ HUNGER_ICON_BG_TYPE::NORMAL };

public:
	static E::UPtr<CUIHungerBarIcon> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END