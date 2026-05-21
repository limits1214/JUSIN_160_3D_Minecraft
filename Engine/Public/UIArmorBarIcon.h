#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIArmorBarIcon final : public E::CUIObject
{
public:
	enum class ARMOR_ICON_TYPE
	{
		EMPTY,
		FULL,
		HALF,
	};
public:
	DECLARE_DERIVED_TYPE(CUIArmorBarIcon, CUIObject)

private:
	CUIArmorBarIcon();
	~CUIArmorBarIcon() override;

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
	void SetIconType(ARMOR_ICON_TYPE eIconType) { m_eIconType = eIconType; };

private:
	ARMOR_ICON_TYPE m_eIconType{ ARMOR_ICON_TYPE::EMPTY };

public:
	static E::UPtr<CUIArmorBarIcon> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END