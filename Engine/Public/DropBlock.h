
#pragma once
#include "DropItemObject.h"

NS_BEGIN(Engine)
class CResDynamicBuffer;
class ENGINE_DLL CDropBlock : public CDropItemObject
{
public:
	DECLARE_DERIVED_TYPE(CDropBlock, CDropItemObject)

private:
	explicit CDropBlock();
	CDropBlock(const CDropBlock& rhs);
	~CDropBlock() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:
	std::vector<VTX_DROP_BLOCK_INSTANCED_DATA> m_vecInstancedData{};
	uint32_t m_iNumElements{ 1000 };
	SPtr<CResDynamicBuffer> m_pResInstancedBuffer{};

public:
	static UPtr<CDropBlock> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END