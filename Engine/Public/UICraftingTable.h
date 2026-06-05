#pragma once
#include "UIObject.h"
NS_BEGIN(Engine)
class CComConstantBuffer;
class ENGINE_DLL CUICraftingTable final : public E::CUIObject
{
public:
	DECLARE_DERIVED_TYPE(CUICraftingTable, CUIObject)

private:
	CUICraftingTable();
	~CUICraftingTable() override;

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
	_bool GetRender() const { return m_bRender; }

private:
	_bool m_bRender{ false };

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
	CComConstantBuffer* m_pComCBufferPerUI{};

public:
	static E::UPtr<CUICraftingTable> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END