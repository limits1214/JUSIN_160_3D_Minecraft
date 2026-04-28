#pragma once
#include "UIObject.h"
NS_BEGIN(Client)

class CTestSimpleGreedyChunk final : public E::CGameObject
{
public:
	DECLARE_DERIVED_TYPE(CTestSimpleGreedyChunk, E::CGameObject)

private:
	CTestSimpleGreedyChunk();
	~CTestSimpleGreedyChunk() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;
	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

public:
	static E::UPtr<CTestSimpleGreedyChunk> Create();
	E::UPtr<E::CPrototype> Clone(void* pArg) override;
};

NS_END