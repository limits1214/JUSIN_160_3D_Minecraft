#pragma once
#include "VoxelObject.h"

#include "ItemObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CChestStorage : public CVoxelObject
{
public:
	typedef struct tagDesc : CVoxelObject::DESC
	{
		//std::pair<StringID, StringID> viBufferId{};
	}DESC;

public:
	DECLARE_DERIVED_TYPE(CChestStorage, CVoxelObject)

private:
	explicit CChestStorage();
	~CChestStorage() override;

public:
	HRESULT Initialize(void* pArg) override;
	void PriorityUpdate(E::_float fTimeDelta) override;
	void Update(E::_float fTimeDelta) override;
	void LateUpdate(E::_float fTimeDelta) override;

	HRESULT Render(ID3D11DeviceContext* pContext, const E::RENDER_CTX& ctx) override;

private:


public:
	static UPtr<CChestStorage> Create();
	UPtr<CPrototype> Clone(void* pArg) override;
};

NS_END