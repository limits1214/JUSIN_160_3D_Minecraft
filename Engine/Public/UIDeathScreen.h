
#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUIDeathScreen final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUIDeathScreen, CUIObject)

private:
	CUIDeathScreen();
	~CUIDeathScreen() override;

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
	_bool m_bRender{ false };

public:
	void SetPlayer(CHandle h) { m_hPlayer = h; }
private:
	CHandle m_hPlayer{};

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	static E::UPtr<CUIDeathScreen> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END