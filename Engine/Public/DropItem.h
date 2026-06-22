#pragma once
#include "DropItemObject.h"

NS_BEGIN(Engine)
class CCollider;
class CResDynamicBuffer;
class ENGINE_DLL CDropItem : public CDropItemObject
{

public:
	DECLARE_DERIVED_TYPE(CDropItem, CDropItemObject)

private:
	explicit CDropItem();
	CDropItem(const CDropItem& rhs);
	~CDropItem() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;


	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	std::vector<VTX_DROP_ITEM_INSTANCED_DATA> m_vecInstancedData{};
	uint32_t m_iNumElements{ 1000 };
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CDropItem> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END