#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)

class ENGINE_DLL CUIBreathBar final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUIBreathBar, CUIObject)

private:
	CUIBreathBar();
	~CUIBreathBar() override;

public:
	void UpdateGUI() override;

public:
	_bool GetRender() const { return m_bRender; }

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
	int32_t m_iCurrentHalfBreathCnt{ 5 };

public:
	static E::UPtr<CUIBreathBar> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END