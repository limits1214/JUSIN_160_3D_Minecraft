#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)

class ENGINE_DLL CUIHotBarSelect final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUIHotBarSelect, CUIObject)

private:
	CUIHotBarSelect();
	~CUIHotBarSelect() override;

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


public:
	int32_t GetSelectIdx() const { return   std::clamp(m_iSelectIdx, 0, 8); }
	void SetSelectIdx(int32_t idx) { m_iSelectIdx = std::clamp(idx, 0, 8); };
private:
	int32_t m_iSelectIdx{};

public:
	static E::UPtr<CUIHotBarSelect> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END