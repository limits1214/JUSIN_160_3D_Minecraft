#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)

class ENGINE_DLL CUIExperienceBarGage final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUIExperienceBarGage, CUIObject)

private:
	CUIExperienceBarGage();
	~CUIExperienceBarGage() override;

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
	_float m_fGage = { 0.5f };
	_bool m_bCalcCoord = { false };

public:
	static E::UPtr<CUIExperienceBarGage> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END