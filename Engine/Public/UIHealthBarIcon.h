#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class CTransform;
class ENGINE_DLL CUIHealthBarIcon final : public E::CUIObject
{
public:
	enum class HEALTH_ICON_BG_TYPE
	{
		NORMAL,
	};
	enum class HEALTH_ICON_TYPE
	{
		EMPTY,
		FULL,
		HALF,
	};
public:
	DECLARE_DERIVED_TYPE(CUIHealthBarIcon, CUIObject)

private:
	CUIHealthBarIcon();
	~CUIHealthBarIcon() override;

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
	CTransform* m_pComOverlayTransform{};
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

private:
	_bool m_bRender{ true };

public:
	void SetIconType(HEALTH_ICON_TYPE eIconType) { m_eIconType = eIconType; };
	void SetIconBgType(HEALTH_ICON_BG_TYPE eIconBgType) { m_eIconBgType = eIconBgType; };

private:
	HEALTH_ICON_TYPE m_eIconType{ HEALTH_ICON_TYPE::EMPTY };
	HEALTH_ICON_BG_TYPE m_eIconBgType{ HEALTH_ICON_BG_TYPE::NORMAL };

public:
	static E::UPtr<CUIHealthBarIcon> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END