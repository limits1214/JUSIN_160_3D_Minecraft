#pragma once
#include "HandHeldItemObject.h"

NS_BEGIN(Engine)
class CComConstantBuffer;
class CResDynamicBuffer;
class ENGINE_DLL CHandHeldItem : public CHandHeldItemObject
{
public:
	DECLARE_DERIVED_TYPE(CHandHeldItem, CHandHeldItemObject)

protected:
	explicit CHandHeldItem();
	~CHandHeldItem() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	CComConstantBuffer* m_pComCBufferPerObject{};
private:
	uint32_t m_iNumElements{ 1 };
	std::vector<VTX_DROP_ITEM_INSTANCED_DATA> m_vecItemInstancedData{};
	SPtr<CResDynamicBuffer> m_pResItemInstancedBuffer{};
private:
	std::vector<VTX_DROP_BLOCK_INSTANCED_DATA> m_vecBlockInstancedData{};
	SPtr<CResDynamicBuffer> m_pResBlockInstancedBuffer{};


public:
	static UPtr<CHandHeldItem> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END