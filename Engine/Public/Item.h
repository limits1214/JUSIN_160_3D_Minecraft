#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CItem : public CItemObject
{
public:
	DECLARE_DERIVED_TYPE(CItem, CItemObject)

private:
	explicit CItem();
	~CItem() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	static UPtr<CItem> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END