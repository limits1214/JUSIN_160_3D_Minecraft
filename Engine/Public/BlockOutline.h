#pragma once
#include "ItemObject.h"

NS_BEGIN(Engine)
class CResDynamicVIBuffer;
class ENGINE_DLL CBlockOutline : public CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CBlockOutline, CGameObject)

private:
	explicit CBlockOutline();
	~CBlockOutline() override;

public:
	HRESULT InitializePrototype(void* pArg = nullptr) override;
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	SPtr<CResDynamicVIBuffer> m_pVIBuffer{};

private:
	_bool m_bRender{ true };

public:
	static UPtr<CBlockOutline> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END